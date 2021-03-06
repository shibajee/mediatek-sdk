/*
 * ProFTPD: mod_quotatab -- a module for managing FTP byte/file quotas via
 *                          centralized tables
 *
 * Copyright (c) 2001-2007 TJ Saunders
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307, USA.
 *
 * As a special exemption, TJ Saunders gives permission to link this program
 * with OpenSSL, and distribute the resulting executable, without including
 * the source code for OpenSSL in the source distribution.
 *
 * This is mod_quotatab, contrib software for proftpd 1.2 and above.
 * For more information contact TJ Saunders <tj@castaglia.org>.  It is based
 * the ideas in Eric Estabrook's mod_quota, available from
 * ftp://pooh.urbanrage.com/pub/c/.  This module, however, has been written
 * from scratch to implement quotas in a different way.
 *
 * $Id: //WIFI_SOC/MP/SDK_4_3_0_0/RT288x_SDK/source/user/proftpd/contrib/mod_quotatab.c#1 $
 */

#include "mod_quotatab.h"

typedef struct regtab_obj {
  struct regtab_obj *prev, *next;
 
  /* Table source type name */
  const char *regtab_name;

  /* Initialization function for this type of table source */
  quota_table_t *(*regtab_open)(pool *, quota_tabtype_t, const char *);

  /* Flags for this type of table: QUOTATAB_LIMIT_SRC, QUOTATAB_TALLY_SRC.
   * Used to restrict some sources to only certain table types (ie
   * LDAP to limit tables).
   */
  unsigned int regtab_srcs;

} quota_regtab_t;

module quotatab_module;

/* Quota objects for the current session */
static quota_table_t *limit_tab;
static quota_table_t *tally_tab;

/* Memory pools for this module */
static pool *quotatab_pool = NULL;
static pool *quotatab_backend_pool = NULL;

/* List of registered quotatab sources */
static quota_regtab_t *quotatab_backends = NULL;
static unsigned int quotatab_nbackends = 0;

/* Logging data */
static int quota_logfd = -1; 
static char *quota_logname = NULL;

static unsigned char allow_site_quota = TRUE;
static unsigned char use_dirs = FALSE;
static unsigned char use_quotas = FALSE;
static unsigned char have_quota_entry = FALSE;
static unsigned char have_quota_limit_table = FALSE;
static unsigned char have_quota_tally_table = FALSE;
static unsigned char have_quota_lock = FALSE;
#if defined(HAVE_REGEX_H) && defined(HAVE_REGCOMP)
static regex_t *quota_exclude_re = NULL;
static const char *quota_exclude_filter = NULL;
#endif

static quota_units_t byte_units = BYTE;

/* For transmitting number of bytes from PRE_CMD to POST_CMD handlers
 * (for use by APPE, DELE, MKD, RMD, RNTO, STOR, STOU, XMKD, and XRMD
 * commands)
 */
static off_t quotatab_disk_bytes;

/* It is the case where sometimes a command may be denied by a PRE_CMD
 * handler of this module, in which case an appropriate error response is
 * added to the response chain.  When the matching POST_CMD_ERR handler
 * then runs, it will add another response.  Sometimes, however, the
 * POST_CMD_ERR handlers may be run for other reasons, and the response
 * added is not a duplicate.  Use this flag to signal when a response has
 * already been added.
 */
static unsigned char have_err_response = FALSE;

/* convenience macros */
#define DISPLAY_BYTES_IN(x) \
    quota_display_bytes((x)->tmp_pool, \
    quotatab_tally.bytes_in_used, quotatab_limit.bytes_in_avail, IN)

#define DISPLAY_BYTES_OUT(x) \
    quota_display_bytes((x)->tmp_pool, \
    quotatab_tally.bytes_out_used, quotatab_limit.bytes_out_avail, OUT)

#define DISPLAY_BYTES_XFER(x) \
    quota_display_bytes((x)->tmp_pool, \
    quotatab_tally.bytes_xfer_used, quotatab_limit.bytes_xfer_avail, XFER)

#define DISPLAY_FILES_IN(x) \
    quota_display_files((x)->tmp_pool, \
    quotatab_tally.files_in_used, quotatab_limit.files_in_avail, IN)

#define DISPLAY_FILES_OUT(x) \
    quota_display_files((x)->tmp_pool, \
    quotatab_tally.files_out_used, quotatab_limit.files_out_avail, OUT)

#define DISPLAY_FILES_XFER(x) \
    quota_display_files((x)->tmp_pool, \
    quotatab_tally.files_xfer_used, quotatab_limit.files_xfer_avail, XFER)

#define QUOTATAB_TALLY_READ \
  if (!quotatab_limit.quota_per_session) { \
    if (quotatab_read() < 0) \
      quotatab_log("error: unable to read tally: %s", strerror(errno)); \
  }

#define QUOTATAB_TALLY_WRITE(bi, bo, bx, fi, fo, fx) \
  if (quotatab_write((bi), (bo), (bx), (fi), (fo), (fx)) < 0) \
    quotatab_log("error: unable to write tally: %s", strerror(errno));

static unsigned long quotatab_opts = 0UL;
#define QUOTA_OPT_SCAN_ON_LOGIN		0x0001

#define QUOTA_SCAN_FL_VERBOSE		0x0001

/* necessary prototypes */
MODRET quotatab_pre_stor(cmd_rec *);
MODRET quotatab_post_stor(cmd_rec *);
MODRET quotatab_post_stor_err(cmd_rec *);
static int quotatab_rlock(quota_tabtype_t);
static int quotatab_unlock(quota_tabtype_t);
static int quotatab_wlock(quota_tabtype_t);

/* Support routines
 */

/* Quota units routines */

static char *quota_display_bytes(pool *p, double bytes_used,
    double bytes_avail, quota_xfer_t xfer_type) {
  double adj_used = 0.0, adj_avail = 0.0;
  char *display = (char *) pcalloc(p, 80);
  char *xferstr = NULL;

  switch (xfer_type) {
    case IN:
      xferstr = "upload";
      break;

    case OUT:
      xferstr = "download";
      break;

    case XFER:
      xferstr = "transfer";
      break;

    default:
      break;
  }

  switch (byte_units) {

    case BYTE:
      /* no manipulation needed */
      sprintf(display, "%.2f of %.2f %s byte%s", bytes_used, bytes_avail,
        xferstr, bytes_avail != 1.0 ? "s" : "");
      break;

    case KILO:
      /* divide by 1024.0 */
      adj_used = (bytes_used / 1024.0);
      adj_avail = (bytes_avail / 1024.0);
      sprintf(display, "%.2f of %.2f %s Kb", adj_used, adj_avail, xferstr);

      break;

    case MEGA:
      /* divide by 1024.0 * 1024.0 */
      adj_used = (bytes_used / (1024.0 * 1024.0));
      adj_avail = (bytes_avail / (1024.0 * 1024.0));
      sprintf(display, "%.2f of %.2f %s Mb", adj_used, adj_avail, xferstr);

      break;

    case GIGA:
      /* divide by 1024.0 * 1024.0 * 1024.0 */
      adj_used = (bytes_used / (1024.0 * 1024.0 * 1024.0));
      adj_avail = (bytes_avail / (1024.0 * 1024.0 * 1024.0));
      sprintf(display, "%.2f of %.2f %s Gb", adj_used, adj_avail, xferstr);

      break;

    /* always have a fallthrough case, even if it _should_ never be reached
     */
    default:
      quotatab_log("warning: unknown QuotaDisplayUnits");
      break;
  }

  return display;
}

static char *quota_display_files(pool *p, unsigned int files_used,
    unsigned int files_avail, quota_xfer_t xfer_type) {
  char *display = (char *) pcalloc(p, 80);
  char *xferstr = NULL;

  switch (xfer_type) {
    case IN:
      xferstr = "upload";
      break;

    case OUT:
      xferstr = "download";
      break;

    case XFER:
      xferstr = "transfer";
      break;

    default:
      break;
  }

  sprintf(display, "%u of %u %s %s", files_used, files_avail, xferstr,
    files_avail != 1.0 ? "files" : "file");

  return display;
}

static char *quota_display_site_bytes(pool *p, double bytes_used,
    double bytes_avail, quota_xfer_t xfer_type) {
  double adj_used = 0.0, adj_avail = 0.0;
  char *display = (char *) pcalloc(p, 80);

  switch (byte_units) {
    case BYTE:
      /* no calculation needed */

      if (bytes_avail > 0.0)
        sprintf(display, "bytes:\t%.2f/%.2f", bytes_used, bytes_avail);
      else
        sprintf(display, "bytes:\tunlimited");
      break;

    case KILO:
      /* divide by 1024.0 */
      adj_used = (bytes_used / 1024.0);
      adj_avail = (bytes_avail / 1024.0);

      if (adj_avail > 0.0)
        sprintf(display, "Kb:\t%s%.2f/%.2f", xfer_type != IN ? "" : "\t",
          adj_used, adj_avail);
      else
        sprintf(display, "Kb:\tunlimited");
      break;

    case MEGA:
      /* divide by 1024.0 * 1024.0 */
      adj_used = (bytes_used / (1024.0 * 1024.0));
      adj_avail = (bytes_avail / (1024.0 * 1024.0));

      if (adj_avail > 0.0)
        sprintf(display, "Mb:\t%s%.2f/%.2f", xfer_type != IN ? "" : "\t",
          adj_used, adj_avail);
      else
        sprintf(display, "Mb:\tunlimited");
      break;

    case GIGA:
      /* divide by 1024.0 * 1024.0 * 1024.0 */
      adj_used = (bytes_used / (1024.0 * 1024.0 * 1024.0));
      adj_avail = (bytes_avail / (1024.0 * 1024.0 * 1024.0));

      if (adj_avail > 0.0)
        sprintf(display, "Gb:\t%s%.2f/%.2f", xfer_type != IN ? "" : "\t",
          adj_used, adj_avail);
      else
        sprintf(display, "Gb:\tunlimited");
      break;

    default:
      quotatab_log("warning: unknown QuotaDisplayUnits");
      break;
  }

  return display;
}

static char *quota_display_site_files(pool *p, unsigned int files_used,
    unsigned int files_avail, quota_xfer_t xfer_type) {
  char *display = (char *) pcalloc(p, 80);

  if (files_avail != 0)
    sprintf(display, "files:\t%u/%u", files_used, files_avail);
  else
    sprintf(display, "files:\tunlimited");

  return display;
}

/* Quota logging routines */
static int quotatab_closelog(void) {
  /* sanity check */
  if (quota_logfd != -1) {
    close(quota_logfd);
    quota_logfd = -1;
    quota_logname = NULL;
  }

  return 0;
}

int quotatab_log(const char *fmt, ...) {
  char buf[PR_TUNABLE_BUFFER_SIZE] = {'\0'};
  time_t timestamp = time(NULL);
  struct tm *t = NULL;
  va_list msg;

  /* sanity check */
  if (!quota_logname)
    return 0;

  t = pr_localtime(NULL, &timestamp);

  /* prepend the timestamp */
  strftime(buf, sizeof(buf), "%b %d %H:%M:%S ", t);
  buf[sizeof(buf) - 1] = '\0';

  /* prepend a small header */
  snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf),
           MOD_QUOTATAB_VERSION "[%u]: ", (unsigned int) getpid());

  buf[sizeof(buf) - 1] = '\0';

  /* affix the message */
  va_start(msg, fmt);
  vsnprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), fmt, msg);
  va_end(msg);

  buf[strlen(buf)] = '\n';
  buf[sizeof(buf) - 1] = '\0';

  if (write(quota_logfd, buf, strlen(buf)) < 0)
    return -1;

  return 0;
}

int quotatab_openlog(void) {
  int res = 0;

  /* Sanity checks. */
  if (quota_logname)
    return 0;

  if ((quota_logname = (char *) get_param_ptr(main_server->conf,
      "QuotaLog", FALSE)) == NULL)
    return 0;

  /* check for "none" */
  if (strcasecmp(quota_logname, "none") == 0) {
    quota_logname = NULL;
    return 0;
  }

  pr_signals_block();
  PRIVS_ROOT
  res = pr_log_openfile(quota_logname, &quota_logfd, 0640);
  PRIVS_RELINQUISH
  pr_signals_unblock();

  return res;
}

static int quotatab_close(quota_tabtype_t tab_type) {
  int res = 0;

  if (tab_type == TYPE_TALLY) {
    res = tally_tab->tab_close(tally_tab);
    tally_tab = NULL;

  } else if (tab_type == TYPE_LIMIT) {
    res = limit_tab->tab_close(limit_tab); 
    limit_tab = NULL;
  }

  return res;
}

static int quotatab_verify(quota_tabtype_t tab_type) {

  /* Request the table source object to verify itself */
  if (tab_type == TYPE_TALLY) {
    if (tally_tab->tab_verify(tally_tab))
      return TRUE;
    else
      quotatab_log("error: unable to use QuotaTallyTable: bad table header");

  } else if (tab_type == TYPE_LIMIT) {
    if (limit_tab->tab_verify(limit_tab))
      return TRUE;
    else
      quotatab_log("error: unable to use QuotaLimitTable: bad table header");
  }

  return FALSE;
}

static int quotatab_create(void) {

  /* Creation of an entry only ever occurs in the tally table -- the limit
   * table is considered read-only.
   */

  /* Obtain a writer lock for the entry in question. */
  if (quotatab_wlock(TYPE_TALLY) < 0)
    return FALSE;

  if (tally_tab->tab_create(tally_tab) < 0) {
    quotatab_unlock(TYPE_TALLY);
    return FALSE;
  }

  /* Release the lock */
  if (quotatab_unlock(TYPE_TALLY) < 0)
    return FALSE;

  return TRUE;
}

static quota_regtab_t *quotatab_get_backend(const char *backend,
    unsigned int srcs) {
  quota_regtab_t *regtab;

  for (regtab = quotatab_backends; regtab; regtab = regtab->next) {
    if ((regtab->regtab_srcs & srcs) &&
        strcmp(regtab->regtab_name, backend) == 0)
      return regtab;
  }

  errno = ENOENT;
  return NULL;
}

/* Returns TRUE if the given path is to be ignored, FALSE otherwise. */
static int quotatab_ignore_path(const char *path) {
  if (!path)
    return FALSE;

#if defined(HAVE_REGEX_H) && defined(HAVE_REGCOMP)
  if (quota_exclude_re == NULL)
    return FALSE;

  if (regexec(quota_exclude_re, path, 0, NULL, 0) == 0)
    return TRUE;

  return FALSE;
#else
  return FALSE;
#endif
}

static int quotatab_scan_dir(pool *p, const char *path, uid_t uid,
    gid_t gid, int flags, double *nbytes, unsigned int *nfiles) {
  struct stat st;
  DIR *dirh;
  struct dirent *dent;

  if (!nbytes ||
      !nfiles) {
    errno = EINVAL;
    return -1;
  }

  if (quotatab_ignore_path(path)) {
    quotatab_log("path '%s' matches QuotaExcludeFilter '%s', ignoring",
      path, quota_exclude_filter);
    return 0;
  }

  if (pr_fsio_lstat(path, &st) < 0) {
    return -1;
  }

  if (!S_ISDIR(st.st_mode)) {
    errno = EINVAL;
    return -1;
  }

  dirh = pr_fsio_opendir(path);
  if (dirh == NULL) {
    return -1;
  }

  if (uid != (uid_t) -1 &&
      st.st_uid == uid) {
    *nbytes += st.st_size;
    *nfiles += 1;

  } else if (gid != (gid_t) -1 &&
             st.st_gid == gid) {
    *nbytes += st.st_size;
    *nfiles += 1;
  }

  while ((dent = pr_fsio_readdir(dirh)) != NULL) {
    char *file;

    pr_signals_handle();

    if (strcmp(dent->d_name, ".") == 0 ||
        strcmp(dent->d_name, "..") == 0) {
      continue;
    }

    file = pdircat(p, path, dent->d_name, NULL);

    memset(&st, 0, sizeof(st));
    if (pr_fsio_lstat(file, &st) < 0) {
      quotatab_log("unable to lstat '%s': %s", file, strerror(errno));
      continue;
    }

    if (S_ISREG(st.st_mode) ||
        S_ISLNK(st.st_mode)) {

      if (uid != (uid_t) -1 &&
          st.st_uid == uid) {
        *nbytes += st.st_size;
        *nfiles += 1;
  
      } else if (gid != (gid_t) -1 &&
                 st.st_gid == gid) {
        *nbytes += st.st_size;
        *nfiles += 1;
      }

    } else if (S_ISDIR(st.st_mode)) {
      pool *sub_pool;

      sub_pool = make_sub_pool(p);

      if (quotatab_scan_dir(sub_pool, file, uid, gid, flags, nbytes,
          nfiles) < 0) {
        quotatab_log("error scanning '%s': %s", file, strerror(errno));
      }

      destroy_pool(sub_pool);

    } else {
      if (flags & QUOTA_SCAN_FL_VERBOSE) {
        quotatab_log("file '%s' is not a file, symlink, or directory; skipping",
          file);
      }
    }
  }

  pr_fsio_closedir(dirh); 
  return 0;
}

static int quotatab_open(quota_tabtype_t tab_type) {

  if (tab_type == TYPE_TALLY) {
    register config_rec *c = NULL;
    register quota_regtab_t *regtab = NULL;

    c = find_config(main_server->conf, CONF_PARAM, "QuotaTallyTable", FALSE);
    if (!c) {
      quotatab_log("notice: no QuotaTallyTable configured");
      return -1;
    }

    regtab = quotatab_get_backend(c->argv[0], QUOTATAB_TALLY_SRC);
    if (regtab) {
      tally_tab = regtab->regtab_open(quotatab_pool, TYPE_TALLY, c->argv[1]);
      if (!tally_tab)
        return -1;

    } else {
      quotatab_log("error: unsupported tally table type: '%s'", c->argv[0]);
      return -1;
    }

  } else if (tab_type == TYPE_LIMIT) {
    register config_rec *c = NULL;
    register quota_regtab_t *regtab = NULL;

    c = find_config(main_server->conf, CONF_PARAM, "QuotaLimitTable", FALSE);
    if (!c) {
      quotatab_log("notice: no QuotaLimitTable configured");
      return -1;
    }

    /* Look up the table source open routine by name, and invoke it */
    regtab = quotatab_get_backend(c->argv[0], QUOTATAB_LIMIT_SRC);
    if (regtab) {
      limit_tab = regtab->regtab_open(quotatab_pool, TYPE_LIMIT, c->argv[1]);
      if (!limit_tab)
        return -1;

    } else {
      quotatab_log("error: unsupported limit table type: '%s'", c->argv[0]);
      return -1;
    }
  }

  return 0;
}

/* Reads via this function are only ever done on tally tables.  Limit tables
 * are read via the quotatab_lookup function.
 */
int quotatab_read(void) {
  int bread = 0;

  /* Make sure the tally table can support reads. */
  if (!tally_tab || !tally_tab->tab_read) {
    errno = EPERM;
    return -1;
  }

  /* Obtain a reader lock for the entry in question. */
  if (quotatab_rlock(TYPE_TALLY) < 0) {
    quotatab_log("error: unable to obtain read lock: %s", strerror(errno));
    return -1;
  }

  /* Read from the current point in the stream enough information to populate
   * the quota structs.
   */
  if ((bread = tally_tab->tab_read(tally_tab)) < 0) {
    quotatab_unlock(TYPE_TALLY);
    return -1;
  }

  /* Release the lock */
  if (quotatab_unlock(TYPE_TALLY) < 0) {
    quotatab_log("error: unable to release read lock: %s", strerror(errno));
    return -1;
  }

  return bread;
}

/* This function is used by mod_quotatab backends, to register their
 * individual backend table function pointers with the main mod_quotatab
 * module.
 */
int quotatab_register_backend(const char *backend,
    quota_table_t *(*srcopen)(pool *, quota_tabtype_t, const char *),
    unsigned int srcs) {
  quota_regtab_t *regtab;

  if (!backend || !srcopen) {
    errno = EINVAL;
    return -1;
  }

  if (!quotatab_backend_pool) {
    quotatab_backend_pool = make_sub_pool(permanent_pool);
    pr_pool_tag(quotatab_backend_pool, MOD_QUOTATAB_VERSION ": Backend Pool");
  }

  /* Check to see if this backend has already been registered. */
  regtab = quotatab_get_backend(backend, srcs);
  if (regtab) {
    errno = EEXIST;
    return -1;
  }

  regtab = pcalloc(quotatab_backend_pool, sizeof(quota_regtab_t));
  regtab->regtab_name = pstrdup(quotatab_backend_pool, backend);
  regtab->regtab_open = srcopen;
  regtab->regtab_srcs = srcs;

  /* Add this object to the list. */
  regtab->next = quotatab_backends;
  quotatab_backends = regtab;
  quotatab_nbackends++;

  return 0;
}

static int quotatab_rlock(quota_tabtype_t tab_type) {
  if (have_quota_lock)
    return 0;

  if (tab_type == TYPE_TALLY) {
    int res = tally_tab->tab_rlock(tally_tab);
    if (res == 0)
      have_quota_lock = TRUE;

    return res;
  }

  else if (tab_type == TYPE_LIMIT) {
    int res = limit_tab->tab_rlock(limit_tab);
    if (res == 0)
      have_quota_lock = TRUE;

    return res;
  }

  /* default */
  errno = EINVAL;
  return -1;
}

/* Used by mod_quotatab backends to unregister their backend function pointers
 * from the main mod_quotatab module.
 */
int quotatab_unregister_backend(const char *backend, unsigned int srcs) {
  quota_regtab_t *regtab;

  if (!backend) {
    errno = EINVAL;
    return -1;
  }

  /* Check to see if this backend has been registered. */
  regtab = quotatab_get_backend(backend, srcs);
  if (!regtab) {
    errno = ENOENT;
    return -1;
  }

#if !defined(PR_SHARED_MODULE)
  /* If there is only one registered backend, it cannot be removed.
   */
  if (quotatab_nbackends == 1) {
    errno = EPERM;
    return -1;
  }
#endif

  /* Remove this backend from the linked list. */
  if (regtab->prev)
    regtab->prev->next = regtab->next;
  else
    /* This backend is the start of the quotatab_backends list (prev is NULL),
     * so we need to update the list head pointer as well.
     */
    quotatab_backends = regtab->next;

  if (regtab->next)
    regtab->next->prev = regtab->prev;

  regtab->prev = regtab->next = NULL;

  quotatab_nbackends--;

  /* NOTE: a counter should be kept of the number of unregistrations,
   * as the memory for a registration is not freed on unregistration.
   */

  return 0;
}

/* Note: this function will only find the first occurrence of the given
 *  name and type in the table.  This means that if there is a malformed
 *  quota table, with duplicate name/type pairs, the duplicates will be
 *  ignored.
 */
unsigned char quotatab_lookup(quota_tabtype_t tab_type, const char *name,
    quota_type_t quota_type) {

  if (tab_type == TYPE_TALLY) {

    /* Make sure the requested table can do lookups. */
    if (!tally_tab ||
        !tally_tab->tab_lookup) {
      errno = EPERM;
      return FALSE;
    }

    return tally_tab->tab_lookup(tally_tab, name, quota_type);
  
  } else if (tab_type == TYPE_LIMIT) {

    /* Make sure the requested table can do lookups. */
    if (!limit_tab ||
        !limit_tab->tab_lookup) {
      errno = EPERM;
      return FALSE;
    }

    return limit_tab->tab_lookup(limit_tab, name, quota_type);
  }

  errno = ENOENT;
  return FALSE;
}

static int quotatab_unlock(quota_tabtype_t tab_type) {
  if (!have_quota_lock)
    return 0;

  if (tab_type == TYPE_TALLY) {
    int res = tally_tab->tab_unlock(tally_tab);
    if (res == 0)
      have_quota_lock = FALSE;

    return res;
  }

  else if (tab_type == TYPE_LIMIT) {
    int res = limit_tab->tab_unlock(limit_tab);
    if (res == 0)
      have_quota_lock = FALSE;

    return res;
  }

  /* default */
  errno = EINVAL;
  return -1;
}

static int quotatab_wlock(quota_tabtype_t tab_type) {
  if (have_quota_lock)
    return 0;

  if (tab_type == TYPE_TALLY) {
    int res = tally_tab->tab_wlock(tally_tab);
    if (res == 0)
      have_quota_lock = TRUE;

    return res;
  }

  else if (tab_type == TYPE_LIMIT) {
    int res = limit_tab->tab_wlock(limit_tab);
    if (res == 0)
      have_quota_lock = TRUE;

    return res;
  }

  /* default */
  errno = EINVAL;
  return -1;
}

int quotatab_write(double bytes_in_inc, double bytes_out_inc,
    double bytes_xfer_inc, int files_in_inc, int files_out_inc,
    int files_xfer_inc) {

  /* Make sure the tally table can support writes. */
  if (!tally_tab || !tally_tab->tab_write) {
    errno = EPERM;
    return -1;
  }

  /* Obtain a writer lock for the entry in question */
  if (quotatab_wlock(TYPE_TALLY) < 0) {
    quotatab_log("error: unable to obtain write lock: %s", strerror(errno));
    return -1;
  }

  /* Make sure the deltas are cleared. */
  memset(&quotatab_deltas, '\0', sizeof(quotatab_deltas));

  /* Read in the tally (to catch any possible updates by other processes). */
  QUOTATAB_TALLY_READ

  /* Only update the tally if the value is not "unlimited". */
  if (quotatab_limit.bytes_in_avail > 0.0) {
    quotatab_tally.bytes_in_used += bytes_in_inc;

    /* Prevent underflows. */
    if (quotatab_tally.bytes_in_used < 0.0)
      quotatab_tally.bytes_in_used = 0.0;

    quotatab_deltas.bytes_in_delta = bytes_in_inc;
  }

  /* Only update the tally if the value is not "unlimited". */
  if (quotatab_limit.bytes_out_avail > 0.0) {
    quotatab_tally.bytes_out_used += bytes_out_inc;

    /* Prevent underflows. */
    if (quotatab_tally.bytes_out_used < 0.0)
      quotatab_tally.bytes_out_used = 0.0;

    quotatab_deltas.bytes_out_delta = bytes_out_inc;
  }

  /* Only update the tally if the value is not "unlimited". */
  if (quotatab_limit.bytes_xfer_avail > 0.0) {
    quotatab_tally.bytes_xfer_used += bytes_xfer_inc;

    /* Prevent underflows. */
    if (quotatab_tally.bytes_xfer_used < 0.0)
      quotatab_tally.bytes_xfer_used = 0.0;

    quotatab_deltas.bytes_xfer_delta = bytes_xfer_inc;
  }

  /* Only update the tally if the value is not "unlimited". */
  if (quotatab_limit.files_in_avail != 0) {

    /* Prevent underflows. As this is an unsigned data type, the 
     * underflow check is not as straightforward as checking for a value
     * less than zero.
     */
    if (!(quotatab_tally.files_in_used == 0 && files_in_inc < 0))
      quotatab_tally.files_in_used += files_in_inc;

    quotatab_deltas.files_in_delta = files_in_inc;
  }

  /* Only update the tally if the value is not "unlimited". */
  if (quotatab_limit.files_out_avail != 0) {

    /* Prevent underflows. As this is an unsigned data type, the
     * underflow check is not as straightforward as checking for a value
     * less than zero.
     */
    if (!(quotatab_tally.files_out_used == 0 && files_out_inc < 0))
      quotatab_tally.files_out_used += files_out_inc;

    quotatab_deltas.files_out_delta = files_out_inc;
  }

  /* Only update the tally if the value is not "unlimited". */
  if (quotatab_limit.files_xfer_avail != 0) {

    /* Prevent underflows. As this is an unsigned data type, the
     * underflow check is not as straightforward as checking for a value
     * less than zero.
     */
    if (!(quotatab_tally.files_xfer_used == 0 && files_xfer_inc < 0))
      quotatab_tally.files_xfer_used += files_xfer_inc;

    quotatab_deltas.files_xfer_delta = files_xfer_inc;
  }

  /* No need to write out to the stream if per-session quotas are in effect. */
  if (quotatab_limit.quota_per_session) {
    memset(&quotatab_deltas, '\0', sizeof(quotatab_deltas));
    quotatab_unlock(TYPE_TALLY);
    return 0;
  }

  if (tally_tab->tab_write(tally_tab) < 0) {
    quotatab_log("error: unable to update tally entry: %s", strerror(errno));
    quotatab_unlock(TYPE_TALLY);
    memset(&quotatab_deltas, '\0', sizeof(quotatab_deltas));
    return -1;
  }

  /* Release the lock */
  if (quotatab_unlock(TYPE_TALLY) < 0) {
    quotatab_log("error: unable to release write lock: %s", strerror(errno));
    memset(&quotatab_deltas, '\0', sizeof(quotatab_deltas));
    return -1;
  }

  memset(&quotatab_deltas, '\0', sizeof(quotatab_deltas));
  return 0;
}

/* FSIO handlers
 */

static int quotatab_fsio_write(pr_fh_t *fh, int fd, const char *buf,
    size_t bufsz) {
  int res;

  res = write(fd, buf, bufsz);
  if (res < 0)
    return res;

  /* Check to see if we've exceeded our upload limit.  mod_xfer will
   * have called pr_data_xfer(), which will have updated
   * session.xfer.total_bytes, before calling pr_fsio_write(), so
   * we do not have to worry about updated/changing session.xfer.total_bytes
   * ourselves.
   *
   * Note that there is a race condition here: it is possible for the same
   * user to be writing to the same file in chunks from multiple
   * simultaneous connections.
   */

  if (quotatab_limit.bytes_in_avail > 0.0 &&
      quotatab_tally.bytes_in_used + session.xfer.total_bytes > quotatab_limit.bytes_in_avail) {
#if defined(EDQUOT)
    quotatab_log("quotatab write(): limit exceeded, returning EDQUOT");
    errno = EDQUOT;
#elif defined(EFBIG)
    quotatab_log("quotatab write(): limit exceeded, returning EFBIG");
    errno = EFBIG;
#else
    quotatab_log("quotatab write(): limit exceeded, returning EIO");
    errno = EIO;
#endif

    return -1;
  }

  if (quotatab_limit.bytes_xfer_avail > 0.0 &&
      quotatab_tally.bytes_xfer_used + session.xfer.total_bytes > quotatab_limit.bytes_xfer_avail) {
#if defined(EDQUOT)
    quotatab_log("quotatab write(): transfer limit exceeded, returning EDQUOT");
    errno = EDQUOT;
#elif defined(EFBIG)
    quotatab_log("quotatab write(): transfer limit exceeded, returning EFBIG");
    errno = EFBIG;
#else
    quotatab_log("quotatab write(): transfer limit exceeded, returning EIO");
    errno = EIO;
#endif

    return -1;
  }

  return res;
}

/* Configuration handlers
 */

/* usage: QuotaDirectoryTally <on|off> */
MODRET set_quotadirtally(cmd_rec *cmd) {
  int bool = -1;
  config_rec *c = NULL;

  CHECK_ARGS(cmd, 1);
  CHECK_CONF(cmd, CONF_ROOT|CONF_VIRTUAL|CONF_GLOBAL);

  bool = get_boolean(cmd, 1);
  if (bool == -1)
    CONF_ERROR(cmd, "expected boolean argument");

  c = add_config_param(cmd->argv[0], 1, NULL);
  c->argv[0] = pcalloc(c->pool, sizeof(unsigned char));
  *((unsigned char *) c->argv[0]) = (unsigned char) bool;

  return PR_HANDLED(cmd);
}

/* usage: QuotaDisplayUnits <b|Kb|Mb|Gb> */
MODRET set_quotadisplayunits(cmd_rec *cmd) {
  quota_units_t units = BYTE;
  config_rec *c = NULL;

  CHECK_ARGS(cmd, 1);
  CHECK_CONF(cmd, CONF_ROOT|CONF_VIRTUAL|CONF_GLOBAL);

  if (strcasecmp(cmd->argv[1], "b") == 0) {
    /* No need to assign to the units variable, as it defaults to
     * displaying bytes
     */
    ;

  } else if (strcasecmp(cmd->argv[1], "Kb") == 0) {
    units = KILO;

  } else if (strcasecmp(cmd->argv[1], "Mb") == 0) {
    units = MEGA;

  } else if (strcasecmp(cmd->argv[1], "Gb") == 0) {
    units = GIGA;

  } else
    CONF_ERROR(cmd, pstrcat(cmd->tmp_pool, ": unknown display units: ",
      cmd->argv[1], NULL));

  c = add_config_param(cmd->argv[0], 1, NULL);
  c->argv[0] = palloc(c->pool, sizeof(quota_units_t));
  *((quota_units_t *) c->argv[0]) = units;

  return PR_HANDLED(cmd);
}

/* usage: QuotaEngine <on|off> */
MODRET set_quotaengine(cmd_rec *cmd) {
  int bool = -1;
  config_rec *c = NULL;

  CHECK_ARGS(cmd, 1);
  CHECK_CONF(cmd, CONF_ROOT|CONF_VIRTUAL|CONF_GLOBAL);

  bool = get_boolean(cmd, 1);
  if (bool == -1)
    CONF_ERROR(cmd, "expected boolean argument");

  c = add_config_param(cmd->argv[0], 1, NULL);
  c->argv[0] = pcalloc(c->pool, sizeof(unsigned char));
  *((unsigned char *) c->argv[0]) = (unsigned char) bool;

  return PR_HANDLED(cmd);
}

/* usage: QuotaExcludeFilter regex|"none" */
MODRET set_quotaexcludefilter(cmd_rec *cmd) {
#if defined(HAVE_REGEX_H) && defined(HAVE_REGCOMP)
  regex_t *re = NULL;
  config_rec *c;
  int res;

  CHECK_ARGS(cmd, 1);
  CHECK_CONF(cmd, CONF_ROOT|CONF_VIRTUAL|CONF_GLOBAL);

  if (strcasecmp(cmd->argv[1], "none") == 0) {
    add_config_param(cmd->argv[0], 0);
    return PR_HANDLED(cmd);
  }

  re = pr_regexp_alloc();

  res = regcomp(re, cmd->argv[1], REG_EXTENDED|REG_NOSUB);
  if (res != 0) {
    char errstr[256] = {'\0'};

    regerror(res, re, errstr, sizeof(errstr));
    pr_regexp_free(re);

    CONF_ERROR(cmd, pstrcat(cmd->tmp_pool, "'", cmd->argv[1], "' failed regex "
      "compilation: ", errstr, NULL));
  }

  c = add_config_param(cmd->argv[0], 2, NULL, NULL);
  c->argv[1] = pstrdup(c->pool, cmd->argv[1]);
  c->argv[2] = (void *) re;
  return PR_HANDLED(cmd);

#else
  CONF_ERROR(cmd, "The QuotaExcludeFilter directive cannot be used on this "
    "system, as you do not have POSIX compliant regex support");
#endif
}

/* usage: QuotaLock file */
MODRET set_quotalock(cmd_rec *cmd) {
  CHECK_ARGS(cmd, 1);
  CHECK_CONF(cmd, CONF_ROOT|CONF_VIRTUAL|CONF_GLOBAL);

  /* Check for non-absolute paths */
  if (*cmd->argv[1] != '/')
    CONF_ERROR(cmd, "absolute path required");

  add_config_param_str(cmd->argv[0], 1, cmd->argv[1]);

  return PR_HANDLED(cmd);
}

/* usage: QuotaLog path|"none" */
MODRET set_quotalog(cmd_rec *cmd) {
  CHECK_ARGS(cmd, 1);
  CHECK_CONF(cmd, CONF_ROOT|CONF_VIRTUAL|CONF_GLOBAL);

  add_config_param_str(cmd->argv[0], 1, cmd->argv[1]);

  return PR_HANDLED(cmd);
}

/* usage: QuotaOptions opt1 opt2 ... */
MODRET set_quotaoptions(cmd_rec *cmd) {
  config_rec *c;
  register unsigned int i;
  unsigned long opts = 0UL;

  if (cmd->argc-1 == 0)
    CONF_ERROR(cmd, "wrong number of parameters");

  CHECK_CONF(cmd, CONF_ROOT|CONF_VIRTUAL|CONF_GLOBAL);

  c = add_config_param(cmd->argv[0], 1, NULL);

  for (i = 1; i < cmd->argc; i++) {
    if (strcmp(cmd->argv[i], "ScanOnLogin") == 0) {
      opts |= QUOTA_OPT_SCAN_ON_LOGIN;

    } else {
      CONF_ERROR(cmd, pstrcat(cmd->tmp_pool, ": unknown QuotaOption: '",
        cmd->argv[i], "'", NULL));
    }
  }

  c->argv[0] = pcalloc(c->pool, sizeof(unsigned long));
  *((unsigned long *) c->argv[0]) = opts;

  return PR_HANDLED(cmd);
}

/* usage: QuotaShowQuotas <on|off> */
MODRET set_quotashowquotas(cmd_rec *cmd) {
  int bool = -1;
  config_rec *c = NULL;

  CHECK_ARGS(cmd, 1);
  CHECK_CONF(cmd, CONF_ROOT|CONF_VIRTUAL|CONF_GLOBAL);

  bool = get_boolean(cmd, 1);
  if (bool == -1)
    CONF_ERROR(cmd, "expected boolean argument");

  c = add_config_param(cmd->argv[0], 1, NULL);
  c->argv[0] = pcalloc(c->pool, sizeof(unsigned char));
  *((unsigned char *) c->argv[0]) = (unsigned char) bool;

  return PR_HANDLED(cmd);
}

/* usage: Quota{Limit,Tally}Table <source-type:source-info> */
MODRET set_quotatable(cmd_rec *cmd) {
  char *tmp = NULL;
  unsigned int tabflag = 0;
#if !defined(PR_SHARED_MODULE)
  register quota_regtab_t *regtab = NULL;
#endif /* PR_SHARED_MODULE */

  CHECK_ARGS(cmd, 1);
  CHECK_CONF(cmd, CONF_ROOT|CONF_VIRTUAL|CONF_GLOBAL);

  /* Separate the parameter into the separate pieces.  The parameter is 
   * given as one string to enhance its similarity to URL syntax.
   */
  if ((tmp = strchr(cmd->argv[1], ':')) == NULL)
    CONF_ERROR(cmd, "badly formatted parameter");

  *tmp++ = '\0';

  /* Verify that the requested source type has been registered, and supports
   * the table type (limit or tally).
   */
  if (strcasecmp(cmd->argv[0], "QuotaLimitTable") == 0)
    tabflag = QUOTATAB_LIMIT_SRC;

  else if (strcasecmp(cmd->argv[0], "QuotaTallyTable") == 0)
    tabflag = QUOTATAB_TALLY_SRC;

#if !defined(PR_SHARED_MODULE)
  regtab = quotatab_get_backend(cmd->argv[1], tabflag);
  if (!regtab)
    CONF_ERROR(cmd, pstrcat(cmd->tmp_pool, "unsupported table source type: '",
      cmd->argv[1], "'", NULL));
#endif /* PR_SHARED_MODULE */

  add_config_param_str(cmd->argv[0], 2, cmd->argv[1], tmp);
  return PR_HANDLED(cmd);
}

/* Variable handlers
 */

static const char *quota_get_bytes_str(void *data, size_t datasz) {
  const char *res = NULL;
  double adj = 0.0, bytes = *((double *) data);

  switch (byte_units) {
    case BYTE:
      /* no calculation needed */

      if (bytes > 0.0) {
        char buf[PR_TUNABLE_BUFFER_SIZE];
        memset(buf, '\0', sizeof(buf));
        snprintf(buf, sizeof(buf), "%.2f", bytes);
        res = pstrdup(session.pool, buf);

      } else
        res = pstrdup(session.pool, "(unlimited)");

      break;

    case KILO:
      /* Divide by 1024.0 */
      adj = (bytes / 1024.0);

      if (adj > 0.0) {
        char buf[PR_TUNABLE_BUFFER_SIZE];
        memset(buf, '\0', sizeof(buf));
        snprintf(buf, sizeof(buf), "%.2f KB", adj);
        res = pstrdup(session.pool, buf);

      } else
        res = pstrdup(session.pool, "(unlimited)");

      break;

    case MEGA:
      /* Divide by 1024.0 * 1024.0 */
      adj = (bytes / (1024.0 * 1024.0));

      if (adj > 0.0) {
        char buf[PR_TUNABLE_BUFFER_SIZE];
        memset(buf, '\0', sizeof(buf));
        snprintf(buf, sizeof(buf), "%.2f MB", adj);
        res = pstrdup(session.pool, buf);

      } else
        res = pstrdup(session.pool, "(unlimited)");

      break;

    case GIGA:
      /* Divide by 1024.0 * 1024.0 * 1024.0 */
      adj = (bytes / (1024.0 * 1024.0 * 1024.0));

      if (adj > 0.0) {
        char buf[PR_TUNABLE_BUFFER_SIZE];
        memset(buf, '\0', sizeof(buf));
        snprintf(buf, sizeof(buf), "%.2f GB", adj);
        res = pstrdup(session.pool, buf);

      } else
        res = pstrdup(session.pool, "(unlimited)");

      break;

    default:
      quotatab_log("warning: unknown QuotaDisplayUnits");
      break;
  }

  return res;
}

static const char *quota_get_files_str(void *data, size_t datasz) {
  const char *res;
  unsigned int files = *((unsigned int *) data);

  if (files != 0) {
    char buf[PR_TUNABLE_BUFFER_SIZE];
    memset(buf, '\0', sizeof(buf));
    snprintf(buf, sizeof(buf), "%u", files);
    res = pstrdup(session.pool, buf);

  } else
    res = pstrdup(session.pool, "(unlimited)");

  return res;
}

/* Command handlers
 */

MODRET quotatab_pre_appe(cmd_rec *cmd) {
  struct stat sbuf;

  /* Sanity check */
  if (!use_quotas)
    return PR_DECLINED(cmd);

  /* Refresh the tally */
  QUOTATAB_TALLY_READ

  /* Briefly cache the size (in bytes) of the file being appended to, so that
   * if successful, the byte counts can be adjusted correctly.
   */
  pr_fs_clear_cache();
  if (pr_fsio_lstat(cmd->arg, &sbuf) < 0)
    quotatab_disk_bytes = 0;
  else
    quotatab_disk_bytes = sbuf.st_size;

  return PR_DECLINED(cmd);
}

MODRET quotatab_post_appe(cmd_rec *cmd) {
  struct stat sbuf;
  off_t append_bytes = session.xfer.total_bytes;

  /* sanity check */
  if (!use_quotas)
    return PR_DECLINED(cmd);

  if (quotatab_ignore_path(cmd->arg)) {
    quotatab_log("%s: path '%s' matched QuotaExcludeFilter '%s', ignoring",
      cmd->argv[0], cmd->arg, quota_exclude_filter);
    return PR_DECLINED(cmd);
  }

  /* Check on the size of the appended-to file again, and use the difference
   * in file size as the increment.  Make sure that no caching effects 
   * mess with the stat.
   */
  pr_fs_clear_cache();
  if (pr_fsio_lstat(cmd->arg, &sbuf) >= 0)
    append_bytes = sbuf.st_size - quotatab_disk_bytes;

  else {
    if (errno == ENOENT)
      append_bytes = 0;

    else
      quotatab_log("%s: error checking '%s': %s", cmd->argv[0], cmd->arg,
        strerror(errno));
  }

  /* Write out an updated quota entry. */
  QUOTATAB_TALLY_WRITE(append_bytes, 0, session.xfer.total_bytes, 0, 0, 0)

  /* Check the bytes quotas to see if any have been reached.  Report this
   * to the user if so.
   */
  if (quotatab_limit.bytes_in_avail > 0.0 &&
      quotatab_tally.bytes_in_used >= quotatab_limit.bytes_in_avail) {
    quotatab_log("%s: quota reached: used %s", cmd->argv[0],
      DISPLAY_BYTES_IN(cmd));
    pr_response_add(R_DUP, "%s: notice: quota reached: used %s", cmd->argv[0],
      DISPLAY_BYTES_IN(cmd));

  } else if (quotatab_limit.bytes_xfer_avail > 0.0 &&
      quotatab_tally.bytes_xfer_used >= quotatab_limit.bytes_xfer_avail) {
    quotatab_log("%s: quota reached: used %s", cmd->argv[0],
      DISPLAY_BYTES_XFER(cmd));
    pr_response_add(R_DUP, "%s: notice: quota reached: used %s", cmd->argv[0],
      DISPLAY_BYTES_XFER(cmd));
  }

  return PR_DECLINED(cmd);
}

MODRET quotatab_post_appe_err(cmd_rec *cmd) {
  struct stat sbuf;
  off_t append_bytes = session.xfer.total_bytes;

  /* sanity check */
  if (!use_quotas)
    return PR_DECLINED(cmd);

  if (quotatab_ignore_path(cmd->arg)) {
    quotatab_log("%s: path '%s' matched QuotaExcludeFilter '%s', ignoring",
      cmd->argv[0], cmd->arg, quota_exclude_filter);
    return PR_DECLINED(cmd);
  }

  /* Check on the size of the appended-to file again, and use the difference
   * in file size as the increment.  Make sure that no caching effects 
   * mess with the stat.
   */
  pr_fs_clear_cache();
  if (pr_fsio_lstat(cmd->arg, &sbuf) >= 0)
    append_bytes = sbuf.st_size - quotatab_disk_bytes;

  else {
    if (errno == ENOENT)
      append_bytes = 0;

    else
      quotatab_log("%s: error checking '%s': %s", cmd->argv[0], cmd->arg,
        strerror(errno));
  }

  /* Write out an updated quota entry */
  QUOTATAB_TALLY_WRITE(append_bytes, 0, session.xfer.total_bytes, 0, 0, 0)

  /* Check the bytes quotas to see if any have been reached.  Report this
   * to the user if so.
   */
  if (quotatab_limit.bytes_in_avail > 0.0 &&
      quotatab_tally.bytes_in_used >= quotatab_limit.bytes_in_avail) {
    quotatab_log("%s: quota reached: used %s", cmd->argv[0],
      DISPLAY_BYTES_IN(cmd));
    pr_response_add_err(R_DUP, "%s: notice: quota reached: used %s",
      cmd->argv[0], DISPLAY_BYTES_IN(cmd));

  } else if (quotatab_limit.bytes_xfer_avail > 0.0 &&
      quotatab_tally.bytes_xfer_used >= quotatab_limit.bytes_xfer_avail) {
    quotatab_log("%s: quota reached: used %s", cmd->argv[0],
      DISPLAY_BYTES_XFER(cmd));
    pr_response_add_err(R_DUP, "%s: notice: quota reached: used %s",
      cmd->argv[0], DISPLAY_BYTES_XFER(cmd));
  }

  return PR_DECLINED(cmd);
}

MODRET quotatab_pre_dele(cmd_rec *cmd) {
  struct stat sbuf;

  /* sanity check */
  if (!use_quotas)
    return PR_DECLINED(cmd);

  /* Briefly cache the size (in bytes) of the file to be deleted, so that
   * if successful, the byte counts can be adjusted correctly.
   */
  pr_fs_clear_cache();
  if (pr_fsio_lstat(cmd->arg, &sbuf) < 0)
    quotatab_disk_bytes = 0;

  else
    quotatab_disk_bytes = sbuf.st_size;

  return PR_DECLINED(cmd);
}

MODRET quotatab_post_dele(cmd_rec *cmd) {

  /* sanity check */
  if (!use_quotas)
    return PR_DECLINED(cmd); 

  if (quotatab_ignore_path(cmd->arg)) {
    quotatab_log("%s: path '%s' matched QuotaExcludeFilter '%s', ignoring",
      cmd->argv[0], cmd->arg, quota_exclude_filter);
    return PR_DECLINED(cmd);
  }

  /* Write out an updated quota entry. */
  QUOTATAB_TALLY_WRITE(-quotatab_disk_bytes, 0, 0, -1, 0, 0)

  /* NOTE: if use_dirs is TRUE, also take into consideration the decreased
   * disk usage caused by any decrease in the size of the containing directory.
   */

  /* Clear the cached bytes. */
  quotatab_disk_bytes = 0;

  return PR_DECLINED(cmd);
}

MODRET quotatab_pre_mkd(cmd_rec *cmd) {

  /* Sanity check. */
  if (!use_dirs)
    return PR_DECLINED(cmd);

  /* Use quotatab_pre_stor() for most of the work. */
  return quotatab_pre_stor(cmd);
}

MODRET quotatab_post_mkd(cmd_rec *cmd) {

  /* Sanity check. */
  if (!use_dirs)
    return PR_DECLINED(cmd);

  /* Use quotatab_post_stor() for most of the work. */
  return quotatab_post_stor(cmd);
}

MODRET quotatab_post_mkd_err(cmd_rec *cmd) {

  /* Sanity check. */
  if (!use_dirs)
    return PR_DECLINED(cmd);

  /* Use quotatab_post_stor_err() for most of the work. */
  return quotatab_post_stor_err(cmd);
}

MODRET quotatab_post_pass(cmd_rec *cmd) {
  unsigned char have_limit_entry = FALSE;
  have_quota_entry = FALSE;

  /* Be done now if there is no quota table */
  if (!use_quotas ||
      !have_quota_limit_table ||
      !have_quota_tally_table) {
    use_quotas = FALSE;
    quotatab_log("turning QuotaEngine off");
    return PR_DECLINED(cmd);
  }

  /* Check for a limit and a tally entry for this user. */
  if (quotatab_lookup(TYPE_LIMIT, session.user, USER_QUOTA)) {
    quotatab_log("found limit entry for user '%s'", session.user);
    have_limit_entry = TRUE;

    if (quotatab_lookup(TYPE_TALLY, session.user, USER_QUOTA)) {
      quotatab_log("found tally entry for user '%s'", session.user);
      have_quota_entry = TRUE;
    }

    if ((quotatab_opts & QUOTA_OPT_SCAN_ON_LOGIN) &&
        (quotatab_limit.bytes_in_avail > 0 ||
         quotatab_limit.files_in_avail > 0)) {
      double byte_count = 0;
      unsigned int file_count = 0;
      time_t then;

      quotatab_log("ScanOnLogin enabled, scanning current directory '%s' "
        "for files owned by user '%s'", pr_fs_getcwd(), session.user);

      time(&then);
      if (quotatab_scan_dir(cmd->tmp_pool, pr_fs_getcwd(),
          session.uid, -1, 0, &byte_count, &file_count) < 0) {
        quotatab_log("unable to scan '%s': %s", pr_fs_getcwd(),
          strerror(errno));

      } else {
        double bytes_diff = (double)
          (byte_count - quotatab_tally.bytes_in_used);
        int files_diff = file_count - quotatab_tally.files_in_used;

        quotatab_log("found %0.2lf bytes in %u files for user '%s' "
          "in %lu secs", byte_count, file_count, session.user,
          time(NULL) - then);

        quotatab_log("updating tally (%0.2lf bytes, %d files difference)",
          bytes_diff, files_diff);

        /* Write out an updated quota entry */
        QUOTATAB_TALLY_WRITE(bytes_diff, 0, 0, files_diff, 0, 0);
      }
    }
  }

  /* Check for a limit and a tally entry for these groups. */
  if (!have_limit_entry) {
    char *group_name = session.group;
    gid_t group_id = session.gid;

    if (quotatab_lookup(TYPE_LIMIT, group_name, GROUP_QUOTA)) {
      quotatab_log("found limit entry for group '%s'", group_name);
      have_limit_entry = TRUE;

    } else {
      if (session.groups) {
        register int i = 0;

        char **group_names = session.groups->elts;
        gid_t *group_ids = session.gids->elts;

        /* Scan the list of supplemental group memberships for this user. */
        for (i = 0; i < session.groups->nelts-1; i++) {
          group_name = group_names[i];
          group_id = group_ids[i];

          if (quotatab_lookup(TYPE_LIMIT, group_name, GROUP_QUOTA)) {
            quotatab_log("found limit entry for group '%s'", group_name);
            have_limit_entry = TRUE;
            break;
          }
        }
      }
    }

    if (have_limit_entry) {
      if (quotatab_lookup(TYPE_TALLY, group_name, GROUP_QUOTA)) {
        quotatab_log("found tally entry for group '%s'", group_name);
        have_quota_entry = TRUE;
      }

      if ((quotatab_opts & QUOTA_OPT_SCAN_ON_LOGIN) &&
          (quotatab_limit.bytes_in_avail > 0 ||
           quotatab_limit.files_in_avail > 0)) {
        double byte_count = 0;
        unsigned int file_count = 0;
        time_t then;

        quotatab_log("ScanOnLogin enabled, scanning current directory '%s' "
          "for files owned by group '%s'", pr_fs_getcwd(), group_name);

        time(&then);
        if (quotatab_scan_dir(cmd->tmp_pool, pr_fs_getcwd(), -1,
            group_id, 0, &byte_count, &file_count) < 0) {
          quotatab_log("unable to scan '%s': %s", pr_fs_getcwd(),
            strerror(errno));

        } else {
          double bytes_diff = byte_count - quotatab_tally.bytes_in_used;
          int files_diff = file_count - quotatab_tally.files_in_used;

          quotatab_log("found %0.2lf bytes in %u files for group '%s'",
            byte_count, file_count, group_name, time(NULL) - then);

          quotatab_log("updating tally (%0.2lf bytes, %d files difference)",
            bytes_diff, files_diff);

          /* Write out an updated quota entry */
          QUOTATAB_TALLY_WRITE(bytes_diff, 0, 0, files_diff, 0, 0);
        }
      }
    }
  }

  /* Check for a limit and a tally entry for this class. */
  if (!have_limit_entry &&
      session.class) {
    if (quotatab_lookup(TYPE_LIMIT, session.class->cls_name, CLASS_QUOTA)) {
      quotatab_log("found limit entry for class '%s'", session.class->cls_name);
      have_limit_entry = TRUE;

      if (quotatab_lookup(TYPE_TALLY, session.class->cls_name, CLASS_QUOTA)) {
        quotatab_log("found tally entry for class '%s'",
          session.class->cls_name);
        have_quota_entry = TRUE;
      }
    }
  }

  /* Check for a limit and a tally entry for everyone. */
  if (!have_limit_entry) {
    if (quotatab_lookup(TYPE_LIMIT, NULL, ALL_QUOTA)) {
      quotatab_log("found limit entry for all");
      have_limit_entry = TRUE;

      if (quotatab_lookup(TYPE_TALLY, NULL, ALL_QUOTA)) {
        quotatab_log("found tally entry for all");
        have_quota_entry = TRUE;
      }
    }
  }

  /* Close the limit table -- no need to keep it open anymore */
  if (quotatab_close(TYPE_LIMIT) < 0)
    quotatab_log("error closing QuotaLimitTable: %s", strerror(errno));

  /* Only create a new tally entry if there is a corresponding limit
   * entry.
   */

  if (have_limit_entry &&
      !have_quota_entry) {
    memset(quotatab_tally.name, '\0', sizeof(quotatab_tally.name));
    snprintf(quotatab_tally.name, sizeof(quotatab_tally.name), "%s",
      quotatab_limit.name);
    quotatab_tally.name[sizeof(quotatab_tally.name)-1] = '\0';

    quotatab_tally.quota_type = quotatab_limit.quota_type;

    /* Initial tally values. */
    quotatab_tally.bytes_in_used = 0.0F;
    quotatab_tally.bytes_out_used = 0.0F;
    quotatab_tally.bytes_xfer_used = 0.0F;
    quotatab_tally.files_in_used = 0U;
    quotatab_tally.files_out_used = 0U;
    quotatab_tally.files_xfer_used = 0U;

    quotatab_log("creating new tally entry to match limit entry");

    if (quotatab_create()) {
      quotatab_log("new tally entry successfully created");
      have_quota_entry = TRUE;

    } else {
      quotatab_log("error: unable to create tally entry: %s",
        strerror(errno));
    }
  }

  if (have_quota_entry) {

    /* Assume that quotatab_lookup() does the reading in of the necessary
     * data from the quota source table as well.
     */

    /* If per-session quotas are in effect, zero the tally record now */
    if (quotatab_limit.quota_per_session) {
      quotatab_tally.bytes_in_used = 0.0F;
      quotatab_tally.bytes_out_used = 0.0F;
      quotatab_tally.bytes_xfer_used = 0.0F;
      quotatab_tally.files_in_used = 0U;
      quotatab_tally.files_out_used = 0U;
      quotatab_tally.files_xfer_used = 0U;

      quotatab_log("per session setting in effect: updates will not be "
        "tracked in the QuotaTallyTable");
    }

    /* If the limit for this user is a hard limit, install our own FS handler,
     * one that provides a custom write() function.  We will use this to
     * return an error when writing a file causes a limit to be reached.
     */
    if (quotatab_limit.quota_limit_type == HARD_LIMIT) {
      pr_fs_t *fs = pr_register_fs(main_server->pool, "quotatab", "/");
      if (fs) {
        quotatab_log("quotatab fs registered");

        fs->write = quotatab_fsio_write;

      } else
        quotatab_log("error registering quotatab fs: %s", strerror(errno));
    }

  } else {

    /* No quota entry for this user.  Make sure the quotas are not used. */
    quotatab_log("no quota entry found, turning QuotaEngine off");
    use_quotas = FALSE;
  }

  /* Register some Variable entries, for Display files. */
  if (pr_var_set(quotatab_pool, "%{mod_quotatab.limit.bytes_in}",
      "Maximum number of uploadable bytes", PR_VAR_TYPE_FUNC,
      quota_get_bytes_str, &quotatab_limit.bytes_in_avail,
      sizeof(double *)) < 0)
    quotatab_log("error setting %%{mod_quotatab.limit.bytes_in} variable: %s",
      strerror(errno));

  if (pr_var_set(quotatab_pool, "%{mod_quotatab.limit.bytes_out}",
      "Maximum number of downloadable bytes", PR_VAR_TYPE_FUNC,
      quota_get_bytes_str, &quotatab_limit.bytes_out_avail,
      sizeof(double *)) < 0)
    quotatab_log("error setting %%{mod_quotatab.limit.bytes_out} variable: %s",
      strerror(errno));

  if (pr_var_set(quotatab_pool, "%{mod_quotatab.limit.bytes_xfer}",
      "Maximum number of transferble bytes", PR_VAR_TYPE_FUNC,
      quota_get_bytes_str, &quotatab_limit.bytes_xfer_avail,
      sizeof(double *)) < 0)
    quotatab_log("error setting %%{mod_quotatab.limit.bytes_xfer} variable: %s",
      strerror(errno));

  if (pr_var_set(quotatab_pool, "%{mod_quotatab.limit.files_in}",
      "Maximum number of uploadable files", PR_VAR_TYPE_FUNC,
      quota_get_files_str, &quotatab_limit.files_in_avail,
      sizeof(unsigned int *)) < 0)
    quotatab_log("error setting %%{mod_quotatab.limit.files_in} variable: %s",
      strerror(errno));

  if (pr_var_set(quotatab_pool, "%{mod_quotatab.limit.files_out}",
      "Maximum number of downloadable files", PR_VAR_TYPE_FUNC,
      quota_get_files_str, &quotatab_limit.files_out_avail,
      sizeof(unsigned int *)) < 0)
    quotatab_log("error setting %%{mod_quotatab.limit.files_out} variable: %s",
      strerror(errno));

  if (pr_var_set(quotatab_pool, "%{mod_quotatab.limit.files_xfer}",
      "Maximum number of transferable files", PR_VAR_TYPE_FUNC,
      quota_get_files_str, &quotatab_limit.files_xfer_avail,
      sizeof(unsigned int *)) < 0)
    quotatab_log("error setting %%{mod_quotatab.limit.files_xfer} variable: %s",
      strerror(errno));

  if (pr_var_set(quotatab_pool, "%{mod_quotatab.tally.bytes_in}",
      "Current number of uploaded bytes", PR_VAR_TYPE_FUNC,
      quota_get_bytes_str, &quotatab_tally.bytes_in_used,
      sizeof(double *)) < 0)
    quotatab_log("error setting %%{mod_quotatab.tally.bytes_in} variable: %s",
      strerror(errno));

  if (pr_var_set(quotatab_pool, "%{mod_quotatab.tally.bytes_out}",
      "Current number of downloaded bytes", PR_VAR_TYPE_FUNC,
      quota_get_bytes_str, &quotatab_tally.bytes_out_used,
      sizeof(double *)) < 0)
    quotatab_log("error setting %%{mod_quotatab.limit.bytes_out} variable: %s",
      strerror(errno));

  if (pr_var_set(quotatab_pool, "%{mod_quotatab.tally.bytes_xfer}",
      "Current number of transferred bytes", PR_VAR_TYPE_FUNC,
      quota_get_bytes_str, &quotatab_tally.bytes_xfer_used,
      sizeof(double *)) < 0)
    quotatab_log("error setting %%{mod_quotatab.tally.bytes_xfer} variable: %s",
      strerror(errno));

  if (pr_var_set(quotatab_pool, "%{mod_quotatab.tally.files_in}",
      "Current number of uploaded files", PR_VAR_TYPE_FUNC,
      quota_get_files_str, &quotatab_tally.files_in_used,
      sizeof(unsigned int *)) < 0)
    quotatab_log("error setting %%{mod_quotatab.tally.files_in} variable: %s",
      strerror(errno));

  if (pr_var_set(quotatab_pool, "%{mod_quotatab.tally.files_out}",
      "Current number of downloaded files", PR_VAR_TYPE_FUNC,
      quota_get_files_str, &quotatab_tally.files_out_used,
      sizeof(unsigned int *)) < 0)
    quotatab_log("error setting %%{mod_quotatab.tally.files_out} variable: %s",
      strerror(errno));

  if (pr_var_set(quotatab_pool, "%{mod_quotatab.tally.files_xfer}",
      "Current number of transferred files", PR_VAR_TYPE_FUNC,
      quota_get_files_str, &quotatab_tally.files_xfer_used,
      sizeof(unsigned int *)) < 0)
    quotatab_log("error setting %%{mod_quotatab.tally.files_xfer} variable: %s",
      strerror(errno));

  return PR_DECLINED(cmd);
}

MODRET quotatab_pre_retr(cmd_rec *cmd) {

  /* Sanity check */
  if (!use_quotas)
    return PR_DECLINED(cmd);

  /* Refresh the tally */
  QUOTATAB_TALLY_READ

  /* Check quotas to see if bytes download or total quota has been reached.
   * Block command if so.
   */
  if (quotatab_limit.bytes_out_avail > 0.0 &&
      quotatab_tally.bytes_out_used >= quotatab_limit.bytes_out_avail) {

    /* Report the exceeding of the threshhold. */
    quotatab_log("%s denied: quota exceeded: used %s", cmd->argv[0],
      DISPLAY_BYTES_OUT(cmd));
    pr_response_add_err(R_451, "%s denied: quota exceeded: used %s",
      cmd->argv[0], DISPLAY_BYTES_OUT(cmd));
    have_err_response = TRUE;
    return PR_ERROR(cmd);

  } else if (quotatab_limit.bytes_xfer_avail > 0.0 &&
      quotatab_tally.bytes_xfer_used >= quotatab_limit.bytes_xfer_avail) {

    /* Report the exceeding of the threshhold. */
    quotatab_log("%s denied: quota exceeded: used %s", cmd->argv[0],
      DISPLAY_BYTES_XFER(cmd));
    pr_response_add_err(R_451, "%s denied: quota exceeded: used %s",
      cmd->argv[0], DISPLAY_BYTES_XFER(cmd));
    have_err_response = TRUE;
    return PR_ERROR(cmd);
  }

  /* Check quotas to see if files download or total quota has been reached.
   * Block command if so.
   */
  if (quotatab_limit.files_out_avail != 0 &&
      quotatab_tally.files_out_used >= quotatab_limit.files_out_avail) {

    /* Report the exceeding of the threshhold. */
    quotatab_log("%s denied: quota exceeded: used %s", cmd->argv[0],
      DISPLAY_FILES_OUT(cmd));
    pr_response_add_err(R_451, "%s denied: quota exceeded: used %s",
      cmd->argv[0], DISPLAY_FILES_OUT(cmd));
    have_err_response = TRUE;
    return PR_ERROR(cmd);

  } else if (quotatab_limit.files_xfer_avail != 0 &&
      quotatab_tally.files_xfer_used >= quotatab_limit.files_xfer_avail) {

    /* Report the exceeding of the threshhold. */
    quotatab_log("%s: denied: quota exceeded: used %s", cmd->argv[0],
      DISPLAY_FILES_XFER(cmd));
    pr_response_add(R_451, "%s denied: quota exceeded: used %s", cmd->argv[0],
      DISPLAY_FILES_XFER(cmd));
    have_err_response = TRUE;
    return PR_ERROR(cmd);
  }

  return PR_DECLINED(cmd);
}

MODRET quotatab_post_retr(cmd_rec *cmd) {

  /* Sanity check */
  if (!use_quotas)
    return PR_DECLINED(cmd);

  if (quotatab_ignore_path(cmd->arg)) {
    quotatab_log("%s: path '%s' matched QuotaExcludeFilter '%s', ignoring",
      cmd->argv[0], cmd->arg, quota_exclude_filter);
    return PR_DECLINED(cmd);
  }

  /* Write out an updated tally */
  QUOTATAB_TALLY_WRITE(0, session.xfer.total_bytes, session.xfer.total_bytes,
    0, 1, 1)

  /* Check quotas to see if bytes download or total quota has been reached.
   * Report this to user if so.
   */
  if (quotatab_limit.bytes_out_avail > 0.0 &&
      quotatab_tally.bytes_out_used >= quotatab_limit.bytes_out_avail) {

    /* Report the reaching of the threshhold. */
    quotatab_log("%s: quota reached: used %s", cmd->argv[0],
      DISPLAY_BYTES_OUT(cmd));
    pr_response_add(R_DUP, "%s: notice: quota reached: used %s", cmd->argv[0],
      DISPLAY_BYTES_OUT(cmd));

  } else if (quotatab_limit.bytes_xfer_avail > 0.0 &&
      quotatab_tally.bytes_xfer_used >= quotatab_limit.bytes_xfer_avail) {

    /* Report the reaching of the threshhold. */
    quotatab_log("%s: quota reached: used %s", cmd->argv[0],
      DISPLAY_BYTES_XFER(cmd));
    pr_response_add(R_DUP, "%s: notice: quota reached: used %s", cmd->argv[0],
      DISPLAY_BYTES_XFER(cmd));
  }

  /* Check quotas to see if files download or total quota has been reached.
   * Report this to user if so.
   */
  if (quotatab_limit.files_out_avail != 0 && 
      quotatab_tally.files_out_used >= quotatab_limit.files_out_avail) {

    /* Report the reaching of the threshhold. */
    quotatab_log("%s: quota reached: used %s", cmd->argv[0],
      DISPLAY_FILES_OUT(cmd));
    pr_response_add(R_DUP, "%s: notice: quota reached: used %s", cmd->argv[0],
      DISPLAY_FILES_OUT(cmd));

  } else if (quotatab_limit.files_xfer_avail != 0 &&
    quotatab_tally.files_xfer_used >= quotatab_limit.files_xfer_avail) {

    /* Report the reaching of the threshhold. */
    quotatab_log("%s: quota reached: used %s", cmd->argv[0],
      DISPLAY_FILES_XFER(cmd));
    pr_response_add(R_DUP, "%s: notice: quota reached: used %s", cmd->argv[0],
      DISPLAY_FILES_XFER(cmd));
  }

  return PR_DECLINED(cmd);
}

MODRET quotatab_post_retr_err(cmd_rec *cmd) {

  /* Sanity check */
  if (!use_quotas)
    return PR_DECLINED(cmd);

  if (quotatab_ignore_path(cmd->arg)) {
    quotatab_log("%s: path '%s' matched QuotaExcludeFilter '%s', ignoring",
      cmd->argv[0], cmd->arg, quota_exclude_filter);
    return PR_DECLINED(cmd);
  }

  /* Write out an updated tally */
  QUOTATAB_TALLY_WRITE(0, session.xfer.total_bytes, session.xfer.total_bytes,
    0, 0, 0)

  /* Check quotas to see if bytes download or total quota has been reached.
   * Report this to user if so (if not already reported).
   */
  if (quotatab_limit.bytes_out_avail > 0.0 &&
      quotatab_tally.bytes_out_used >= quotatab_limit.bytes_out_avail) {

    if (!have_err_response) {

      /* Report the reaching of the threshhold. */
      quotatab_log("%s: quota reached: used %s", cmd->argv[0],
        DISPLAY_BYTES_OUT(cmd));
      pr_response_add_err(R_DUP, "%s: notice: quota reached: used %s",
        cmd->argv[0], DISPLAY_BYTES_OUT(cmd));
    }

  } else if (quotatab_limit.bytes_xfer_avail > 0.0 &&
      quotatab_tally.bytes_xfer_used >= quotatab_limit.bytes_xfer_avail) {

    if (!have_err_response) {

      /* Report the reaching of the threshhold. */
      quotatab_log("%s: quota reached: used %s", cmd->argv[0],
        DISPLAY_BYTES_XFER(cmd));
      pr_response_add_err(R_DUP, "%s: notice: quota reached: used %s",
        cmd->argv[0], DISPLAY_BYTES_XFER(cmd));
    }
  }

  /* Check quotas to see if files download or total quota has been reached.
   * Report this to user if so (if not already reported).
   */
  if (quotatab_limit.files_out_avail != 0 &&
      quotatab_tally.files_out_used >= quotatab_limit.files_out_avail) {

    if (!have_err_response) {

      /* Report the reaching of the treshhold. */
      quotatab_log("%s: quota reached: used %s", cmd->argv[0],
        DISPLAY_FILES_OUT(cmd));
      pr_response_add_err(R_DUP, "%s: notice: quota reached: used %s",
        cmd->argv[0], DISPLAY_FILES_OUT(cmd));
    }

  } else if (quotatab_limit.files_xfer_avail != 0 &&
    quotatab_tally.files_xfer_used >= quotatab_limit.files_xfer_avail) {

    if (!have_err_response) {

      /* Report the reaching of the treshhold. */
      quotatab_log("%s: quota reached: used %s", cmd->argv[0],
        DISPLAY_FILES_XFER(cmd));
      pr_response_add_err(R_DUP, "%s: notice: quota reached: used %s",
        cmd->argv[0], DISPLAY_FILES_XFER(cmd));
    }
  }

  have_err_response = FALSE;
  return PR_DECLINED(cmd);
}

MODRET quotatab_pre_rmd(cmd_rec *cmd) {
  struct stat sbuf;

  /* Sanity check. */
  if (!use_quotas || !use_dirs)
    return PR_DECLINED(cmd);

  /* Briefly cache the size (in bytes) of the directory to be deleted, so that
   * if successful, the byte counts can be adjusted correctly.
   */
  pr_fs_clear_cache();
  if (pr_fsio_lstat(cmd->arg, &sbuf) < 0)
    quotatab_disk_bytes = 0;
  else
    quotatab_disk_bytes = sbuf.st_size;

  return PR_DECLINED(cmd);
}

MODRET quotatab_post_rmd(cmd_rec *cmd) {

  /* Sanity check. */
  if (!use_quotas || !use_dirs)
    return PR_DECLINED(cmd);

  if (quotatab_ignore_path(cmd->arg)) {
    quotatab_log("%s: path '%s' matched QuotaExcludeFilter '%s', ignoring",
      cmd->argv[0], cmd->arg, quota_exclude_filter);
    return PR_DECLINED(cmd);
  }

  /* Write out an updated quota entry. */
  QUOTATAB_TALLY_WRITE(-quotatab_disk_bytes, 0, 0, -1, 0, -1)

  /* Clear the cached bytes. */
  quotatab_disk_bytes = 0;

  return PR_DECLINED(cmd);
}

MODRET quotatab_pre_rnto(cmd_rec *cmd) {
  struct stat sbuf;

  /* Sanity check */
  if (!use_quotas)
    return PR_DECLINED(cmd);

  /* Briefly cache the size (in bytes) of the file being overwritten, so that
   * if successful, the byte counts can be adjusted correctly.
   */
  pr_fs_clear_cache();
  if (pr_fsio_lstat(cmd->arg, &sbuf) < 0)
    quotatab_disk_bytes = 0;
  else
    quotatab_disk_bytes = sbuf.st_size;

  return PR_DECLINED(cmd);
}

MODRET quotatab_post_rnto(cmd_rec *cmd) {

  /* Sanity check */
  if (!use_quotas)
    return PR_DECLINED(cmd);

  if (quotatab_ignore_path(cmd->arg)) {
    quotatab_log("%s: path '%s' matched QuotaExcludeFilter '%s', ignoring",
      cmd->argv[0], cmd->arg, quota_exclude_filter);
    return PR_DECLINED(cmd);
  }

  /* Write out an updated quota entry. */
  QUOTATAB_TALLY_WRITE(-quotatab_disk_bytes, 0, -quotatab_disk_bytes,
    -1, 0, -1)

  /* Clear the cached bytes. */
  quotatab_disk_bytes = 0;

  return PR_DECLINED(cmd);
}

MODRET quotatab_pre_stor(cmd_rec *cmd) {
  struct stat sbuf;
 
  /* Sanity check */
  if (!use_quotas)
    return PR_DECLINED(cmd);

  /* Refresh the tally */
  QUOTATAB_TALLY_READ

  /* Check quotas to see if bytes upload or total quota has been reached.
   * Block command if so.
   */
  if (quotatab_limit.bytes_in_avail > 0.0 &&
      quotatab_tally.bytes_in_used >= quotatab_limit.bytes_in_avail) {

    /* Report the exceeding of the threshhold. */
    quotatab_log("%s denied: quota exceeded: used %s", cmd->argv[0],
      DISPLAY_BYTES_IN(cmd));
    pr_response_add_err(R_552, "%s denied: quota exceeded: used %s",
      cmd->argv[0], DISPLAY_BYTES_IN(cmd));
    have_err_response = TRUE;
    return PR_ERROR(cmd);

  } else if (quotatab_limit.bytes_xfer_avail > 0.0 &&
      quotatab_tally.bytes_xfer_used >= quotatab_limit.bytes_xfer_avail) {

    /* Report the exceeding of the threshhold. */
    quotatab_log("%s denied: quota exceeded: used %s", cmd->argv[0],
      DISPLAY_BYTES_XFER(cmd));
    pr_response_add_err(R_552, "%s denied: quota exceeded: used %s",
      cmd->argv[0], DISPLAY_BYTES_XFER(cmd));
    have_err_response = TRUE;
    return PR_ERROR(cmd);
  }

  /* Check quotas to see if files upload or total quota has been reached.
   * Block the command if so.
   */
  if (quotatab_limit.files_in_avail != 0 &&
      quotatab_tally.files_in_used >= quotatab_limit.files_in_avail) {

    /* Repor the exceeding of the threshhold. */
    quotatab_log("%s denied: quota exceeded: used %s", cmd->argv[0],
      DISPLAY_FILES_IN(cmd));
    pr_response_add_err(R_552, "%s denied: quota exceeded: used %s",
      cmd->argv[0], DISPLAY_FILES_IN(cmd));
    have_err_response = TRUE;
    return PR_ERROR(cmd);

  } else if (quotatab_limit.files_xfer_avail != 0 &&
      quotatab_tally.files_xfer_used >= quotatab_limit.files_xfer_avail) {

    /* Report the exceeding of the threshhold. */
    quotatab_log("%s denied: quota exceeded: used %s", cmd->argv[0],
      DISPLAY_FILES_XFER(cmd));
    pr_response_add_err(R_552, "%s denied: quota exceeded: used %s",
      cmd->argv[0], DISPLAY_FILES_XFER(cmd));
    have_err_response = TRUE;
    return PR_ERROR(cmd);
  }

  /* Briefly cache the size (in bytes) of the file being appended to, so that
   * if successful, the byte counts can be adjusted correctly.  If the
   * stat fails, it means that a new file is being uploaded, so set the
   * disk_bytes to be zero. 
   */
  pr_fs_clear_cache();
  if (pr_fsio_lstat(cmd->arg, &sbuf) < 0)
    quotatab_disk_bytes = 0;
  else
    quotatab_disk_bytes = sbuf.st_size;

  return PR_DECLINED(cmd);
}

MODRET quotatab_post_stor(cmd_rec *cmd) {
  struct stat sbuf;
  off_t store_bytes = session.xfer.total_bytes;

  /* Sanity check */
  if (!use_quotas)
    return PR_DECLINED(cmd);

  if (quotatab_ignore_path(cmd->arg)) {
    quotatab_log("%s: path '%s' matched QuotaExcludeFilter '%s', ignoring",
      cmd->argv[0], cmd->arg, quota_exclude_filter);
    return PR_DECLINED(cmd);
  }

  /* Check on the size of the stored file again, and use the difference
   * in file size as the increment.  Make sure that no caching effects
   * mess with the stat.
   */
  pr_fs_clear_cache();
  if (pr_fsio_lstat(cmd->arg, &sbuf) >= 0) 
    store_bytes = sbuf.st_size - quotatab_disk_bytes;

  else {
    if (errno == ENOENT)
      store_bytes = 0;

    else
      quotatab_log("%s: error checking '%s': %s", cmd->argv[0], cmd->arg,
        strerror(errno));
  }

  /* NOTE: if use_dirs is TRUE, also take into consideration the increased
   * disk usage caused by any increase in the size of the containing directory.
   */

  /* Write out an updated quota entry */
  QUOTATAB_TALLY_WRITE(store_bytes, 0, session.xfer.total_bytes,
    quotatab_disk_bytes ? 0 : 1, 0, 1)

  /* Check quotas to see if bytes upload or total quota has been reached.
   * Report this to user if so.  If it is a hard bytes limit, delete the
   * uploaded file.
   */
  if (quotatab_limit.bytes_in_avail > 0.0 &&
      quotatab_tally.bytes_in_used >= quotatab_limit.bytes_in_avail) {

    /* Report the reaching of the threshhold. */
    quotatab_log("%s: quota reached: used %s", cmd->argv[0],
      DISPLAY_BYTES_IN(cmd));
    pr_response_add(R_DUP, "%s: notice: quota reached: used %s", cmd->argv[0],
      DISPLAY_BYTES_IN(cmd));

    if (quotatab_tally.bytes_in_used > quotatab_limit.bytes_in_avail &&
        quotatab_limit.quota_limit_type == HARD_LIMIT) {
      if (pr_fsio_unlink(cmd->arg) < 0) {
        quotatab_log("notice: unable to unlink '%s': %s", cmd->arg,
          strerror(errno));

      } else {
        QUOTATAB_TALLY_WRITE(-store_bytes, 0, -session.xfer.total_bytes,
          -1, 0, -1);
        
        /* Report the removal of the file. */
        quotatab_log("%s: quota reached: '%s' removed", cmd->argv[0], cmd->arg);
        pr_response_add(R_DUP, "%s: notice: quota reached: '%s' removed",
          cmd->argv[0], cmd->arg);
      }
    }

  } else if (quotatab_limit.bytes_xfer_avail > 0.0 &&
      quotatab_tally.bytes_xfer_used >= quotatab_limit.bytes_xfer_avail) {

    /* Report the reaching of the threshhold. */ 
    quotatab_log("%s: quota reached: used %s", cmd->argv[0],
      DISPLAY_BYTES_XFER(cmd));
    pr_response_add(R_DUP, "%s: notice: quota reached: used %s", cmd->argv[0],
      DISPLAY_BYTES_XFER(cmd));

    if (quotatab_tally.bytes_xfer_used > quotatab_limit.bytes_xfer_avail &&
        quotatab_limit.quota_limit_type == HARD_LIMIT) {
      if (pr_fsio_unlink(cmd->arg) < 0) {
        quotatab_log("notice: unable to unlink '%s': %s", cmd->arg,
          strerror(errno));

      } else {
        QUOTATAB_TALLY_WRITE(-store_bytes, 0, -session.xfer.total_bytes,
          -1, 0, -1);

        /* Report the removal of the file. */
        quotatab_log("%s: quota reached: '%s' removed", cmd->argv[0], cmd->arg);
        pr_response_add(R_DUP, "%s: notice: quota reached: '%s' removed",
          cmd->argv[0], cmd->arg);
      }
    }
  }

  /* Check quotas to see if files upload or total quota has been reached.
   * Report this to user if so.
   */
  if (quotatab_limit.files_in_avail != 0 &&
      quotatab_tally.files_in_used >= quotatab_limit.files_in_avail) {

    /* Report the reaching of the treshhold. */
    quotatab_log("%s: quota reached: used %s", cmd->argv[0],
      DISPLAY_FILES_IN(cmd));
    pr_response_add(R_DUP, "%s: notice: quota reached: used %s", cmd->argv[0],
      DISPLAY_FILES_IN(cmd));

  } else if (quotatab_limit.files_xfer_avail != 0 &&
      quotatab_tally.files_xfer_used >= quotatab_limit.files_xfer_avail) {

    /* Report the reaching of the threshhold. */
    quotatab_log("%s: quota reached: used %s", cmd->argv[0],
      DISPLAY_FILES_XFER(cmd));
    pr_response_add(R_DUP, "%s: notice: quota reached: used %s", cmd->argv[0],
      DISPLAY_FILES_XFER(cmd));
  }

  return PR_DECLINED(cmd);
}

MODRET quotatab_post_stor_err(cmd_rec *cmd) {
  struct stat sbuf;
  off_t store_bytes = session.xfer.total_bytes;

  /* Sanity check */
  if (!use_quotas)
    return PR_DECLINED(cmd);

  if (quotatab_ignore_path(cmd->arg)) {
    quotatab_log("%s: path '%s' matched QuotaExcludeFilter '%s', ignoring",
      cmd->argv[0], cmd->arg, quota_exclude_filter);
    return PR_DECLINED(cmd);
  }

  /* Check on the size of the stored file again, and use the difference
   * in file size as the increment.  Make sure that no caching effects 
   * mess with the stat.
   */
  pr_fs_clear_cache();
  if (pr_fsio_lstat(cmd->arg, &sbuf) >= 0) 
    store_bytes = sbuf.st_size - quotatab_disk_bytes;

  else {
    if (errno == ENOENT)
      store_bytes = 0;

    else
      quotatab_log("%s: error checking '%s': %s", cmd->argv[0], cmd->arg,
        strerror(errno));
  }

  /* Write out an updated quota entry */
  QUOTATAB_TALLY_WRITE(store_bytes, 0, session.xfer.total_bytes, 0, 0, 0)

  /* Check quotas to see if bytes upload or total quota has been reached.
   * Report this to user if so (if not already reported).  If it is a hard
   * bytes limit, delete the uploaded file.
   */
  if (quotatab_limit.bytes_in_avail > 0.0 &&
      quotatab_tally.bytes_in_used >= quotatab_limit.bytes_in_avail) {

    if (!have_err_response) {

      /* Report the reaching of the threshhold. */
      quotatab_log("%s: quota reached: used %s", cmd->argv[0],
        DISPLAY_BYTES_IN(cmd));
      pr_response_add_err(R_DUP, "%s: notice: quota reached: used %s",
        cmd->argv[0], DISPLAY_BYTES_IN(cmd));
    }

    if (quotatab_tally.bytes_in_used > quotatab_limit.bytes_in_avail) {
      if (quotatab_limit.quota_limit_type == HARD_LIMIT) {
        if (pr_fsio_unlink(cmd->arg) < 0) {
          quotatab_log("notice: unable to unlink '%s': %s", cmd->arg,
            strerror(errno));

        } else {

          /* Report the removal of the file. */
          quotatab_log("%s: quota reached: '%s' removed", cmd->argv[0],
            cmd->arg);
          pr_response_add_err(R_DUP, "%s: notice: quota reached: '%s' removed",
            cmd->argv[0], cmd->arg);
        }
      }

      QUOTATAB_TALLY_WRITE(-store_bytes, 0, -session.xfer.total_bytes, 0, 0, 0);
    }

  } else if (quotatab_limit.bytes_xfer_avail > 0.0 &&
      quotatab_tally.bytes_xfer_used >= quotatab_limit.bytes_xfer_avail) {

    if (!have_err_response) {

      /* Report the reaching of the threshhold. */
      quotatab_log("%s: quota reached: used %s", cmd->argv[0],
        DISPLAY_BYTES_XFER(cmd));
      pr_response_add_err(R_DUP, "%s: notice: quota reached: used %s",
        cmd->argv[0], DISPLAY_BYTES_XFER(cmd));
    }

    if (quotatab_tally.bytes_xfer_used > quotatab_limit.bytes_xfer_avail) {
      if (quotatab_limit.quota_limit_type == HARD_LIMIT) {
        if (pr_fsio_unlink(cmd->arg) < 0) {
          quotatab_log("notice: unable to unlink '%s': %s", cmd->arg,
            strerror(errno));

        } else {

          /* Report the removal of the file. */
          quotatab_log("%s: quota reached: '%s' removed", cmd->argv[0],
            cmd->arg);
          pr_response_add_err(R_DUP, "%s: notice: quota reached: '%s' removed",
            cmd->argv[0], cmd->arg);
        }
      }

      QUOTATAB_TALLY_WRITE(-store_bytes, 0, -session.xfer.total_bytes, 0, 0, 0);
    }
  }

  /* Check quotas to see if files upload or total quota has been reached.
   * Report this to user if so.
   */
  if (quotatab_limit.files_in_avail != 0 &&
      quotatab_tally.files_in_used >= quotatab_limit.files_in_avail) {

    /* Report the reaching of the threshhold. */
    quotatab_log("%s: quota reached: used %s", cmd->argv[0],
      DISPLAY_FILES_IN(cmd));
    pr_response_add_err(R_DUP, "%s: notice: quota reached: used %s",
      cmd->argv[0], DISPLAY_FILES_IN(cmd));

  } else if (quotatab_limit.files_xfer_avail != 0 &&
      quotatab_tally.files_xfer_used >= quotatab_limit.files_xfer_avail) {

    /* Report the reaching of the threshhold. */
    quotatab_log("%s: quota reached: used %s", cmd->argv[0],
      DISPLAY_FILES_XFER(cmd));
    pr_response_add_err(R_DUP, "%s: notice: quota reached: used %s",
      cmd->argv[0], DISPLAY_FILES_XFER(cmd));
  }

  return PR_DECLINED(cmd);
}

MODRET quotatab_site(cmd_rec *cmd) {

  /* Make sure it's a valid SITE QUOTA command */
  if (cmd->argc < 2)
    return PR_DECLINED(cmd);

  if (strcasecmp(cmd->argv[1], "QUOTA") == 0) {
    unsigned char *authenticated = get_param_ptr(cmd->server->conf,
      "authenticated", FALSE);

    /* The user is required to be authenticated/logged in, first */
    if (!authenticated || *authenticated == FALSE) {
      pr_response_send(R_530, "Please login with USER and PASS");
      return PR_ERROR(cmd);
    }

    /* Is showing of the user's quota barred by configuration? */
    if (!allow_site_quota) {
      pr_response_add_err(R_500, "'SITE QUOTA' not understood.");
      return PR_ERROR(cmd);
    }

    /* Check for <Limit> restrictions. */
    if (!dir_check(cmd->tmp_pool, "SITE_QUOTA", "NONE", session.cwd, NULL)) {
      pr_response_add_err(R_550, "%s: %s", cmd->arg, strerror(EPERM));
      return PR_ERROR(cmd);
    }

    /* Log that the user requested their quota. */
    quotatab_log("SITE QUOTA requested by user %s", session.user);

    /* If quotas are not in use, no need to do anything. */
    if (!use_quotas || !have_quota_entry) {
      pr_response_add(R_202, "No quotas in effect");
      return PR_HANDLED(cmd);
    }

    /* Refresh the tally. */
    QUOTATAB_TALLY_READ

    pr_response_add(R_200, "The current quota for this session are "
      "[current/limit]:");

    pr_response_add(R_DUP, "Name: %s", quotatab_limit.name);
    pr_response_add(R_DUP, "Quota Type: %s",
      (quotatab_limit.quota_type == USER_QUOTA ? "User" :
       quotatab_limit.quota_type == GROUP_QUOTA ? "Group" :
       quotatab_limit.quota_type == CLASS_QUOTA ? "Class" :
       quotatab_limit.quota_type == ALL_QUOTA ? "All" :
       "(unknown)"));

    pr_response_add(R_DUP, "Per Session: %s",
      quotatab_limit.quota_per_session ? "True" : "False");

    pr_response_add(R_DUP, "Limit Type: %s",
      (quotatab_limit.quota_limit_type == HARD_LIMIT ? "Hard" :
       quotatab_limit.quota_limit_type == SOFT_LIMIT ? "Soft" :
       "(unknown)"));

    pr_response_add(R_DUP, "  Uploaded %s",
      quota_display_site_bytes(cmd->tmp_pool, quotatab_tally.bytes_in_used,
      quotatab_limit.bytes_in_avail, IN));
    pr_response_add(R_DUP, "  Downloaded %s",
      quota_display_site_bytes(cmd->tmp_pool, quotatab_tally.bytes_out_used,
      quotatab_limit.bytes_out_avail, OUT));
    pr_response_add(R_DUP, "  Transferred %s",
      quota_display_site_bytes(cmd->tmp_pool, quotatab_tally.bytes_xfer_used,
      quotatab_limit.bytes_xfer_avail, XFER));

    pr_response_add(R_DUP, "  Uploaded %s",
      quota_display_site_files(cmd->tmp_pool, quotatab_tally.files_in_used,
      quotatab_limit.files_in_avail, IN));
    pr_response_add(R_DUP, "  Downloaded %s",
      quota_display_site_files(cmd->tmp_pool, quotatab_tally.files_out_used,
      quotatab_limit.files_out_avail, OUT));
    pr_response_add(R_DUP, "  Transferred %s",
      quota_display_site_files(cmd->tmp_pool, quotatab_tally.files_xfer_used,
      quotatab_limit.files_xfer_avail, XFER));

    /* Add one final line to preserve the spacing. */
    pr_response_add(R_DUP,
      "Please contact %s if these entries are inaccurate",
      cmd->server->ServerAdmin ? cmd->server->ServerAdmin : "ftp-admin");

    return PR_HANDLED(cmd);
  }

  if (strcasecmp(cmd->argv[1], "HELP") == 0) {

    /* Add a description of SITE QUOTA to the output. */
    pr_response_add(R_214, "QUOTA");
  }

  return PR_DECLINED(cmd);
}

/* Event handlers
 */

static void quotatab_exit_ev(const void *event_data, void *user_data) {

  if (use_quotas && have_quota_tally_table)
    if (quotatab_close(TYPE_TALLY) < 0)
      quotatab_log("error: unable to close QuotaTallyTable: %s",
        strerror(errno));

  quotatab_closelog();
  return;
}

#if defined(PR_SHARED_MODULE)
static void quotatab_mod_unload_ev(const void *event_data, void *user_data) {
  if (strcmp("mod_quotatab.c", (const char *) event_data) == 0) {
    pr_event_unregister(&quotatab_module, NULL, NULL);
    pr_unregister_fs("quotatab");
    if (quotatab_pool) {
      destroy_pool(quotatab_pool);
      quotatab_pool = NULL;
    }
  }
}
#endif

static void quotatab_restart_ev(const void *event_data, void *user_data) {

  /* Reset the module's memory pool. */
  destroy_pool(quotatab_pool);
  quotatab_pool = make_sub_pool(permanent_pool);
  pr_pool_tag(quotatab_pool, MOD_QUOTATAB_VERSION);

  return;
}

/* Initialization routines
 */

static int quotatab_init(void) {

  /* Initialize the module's memory pool. */
  if (!quotatab_pool) {
    quotatab_pool = make_sub_pool(permanent_pool);
    pr_pool_tag(quotatab_pool, MOD_QUOTATAB_VERSION);
  }

#if defined(PR_SHARED_MODULE)
  pr_event_register(&quotatab_module, "core.module-unload",
    quotatab_mod_unload_ev, NULL);
#endif
  pr_event_register(&quotatab_module, "core.restart", quotatab_restart_ev,
    NULL); 

  return 0;
}

static int quotatab_sess_init(void) {
  config_rec *c;
  unsigned char *quotatab_engine = NULL, *quotatab_showquotas = NULL,
    *quotatab_usedirs = NULL;
  quota_units_t *units = NULL;

  /* Check to see if quotas are enabled for this server. */
  quotatab_engine = get_param_ptr(main_server->conf, "QuotaEngine", FALSE);
  if (quotatab_engine != NULL &&
      *quotatab_engine == TRUE) {
    use_quotas = TRUE;

  } else {
    use_quotas = FALSE;
    return 0;
  }

  /* Check to see if SITE QUOTA enabled for this server. */
  quotatab_showquotas = get_param_ptr(main_server->conf, "QuotaShowQuotas",
    FALSE);
  if (quotatab_showquotas != NULL &&
      *quotatab_showquotas == FALSE) {
    allow_site_quota = FALSE;

 } else {
    allow_site_quota = TRUE;
  }

  quotatab_openlog();

  /* Open the quota limit and tally tables.  This is being done while the
   * proces still has root privs, so the tables _can_ be opened.  In the case
   * of file quotastreams, the file descriptor is cached, so that this process,
   * once root privs are dropped, is still able to read from and write to the
   * streams.  Other confstream mechanisms may need to cache something
   * similarly.
   */
  PRIVS_ROOT
  if (quotatab_open(TYPE_LIMIT) < 0) {
    PRIVS_RELINQUISH
    quotatab_log("error: unable to open QuotaLimitTable: %s", strerror(errno));
    have_quota_limit_table = FALSE;

  } else {
    PRIVS_RELINQUISH

    /* Verify that it's a valid limit table */
    if (quotatab_verify(TYPE_LIMIT))
      have_quota_limit_table = TRUE;
    else
      use_quotas = FALSE;
  }

  PRIVS_ROOT
  if (quotatab_open(TYPE_TALLY) < 0) {
    PRIVS_RELINQUISH
    quotatab_log("error: unable to open QuotaTallyTable: %s", strerror(errno));
    have_quota_tally_table = FALSE;

  } else {
    PRIVS_RELINQUISH

    /* Verify that it's a valid tally table */
    if (quotatab_verify(TYPE_TALLY))
      have_quota_tally_table = TRUE;
    else
      use_quotas = FALSE;
  }

  /* Make sure the tables will be closed when the child exits. */
  pr_event_register(&quotatab_module, "core.exit", quotatab_exit_ev, NULL);

  /* Check for the units to display for byte quotas. */
  units = get_param_ptr(main_server->conf, "QuotaDisplayUnits", FALSE);
  byte_units = units ? *units : BYTE;

  /* Check to see if directories are to be used for tallies for this server. */
  quotatab_usedirs = get_param_ptr(main_server->conf, "QuotaDirectoryTally",
    FALSE);
  if (quotatab_usedirs != NULL &&
      *quotatab_usedirs == TRUE) {
    use_dirs = TRUE;

  } else {
    use_dirs = FALSE;
  }

  c = find_config(main_server->conf, CONF_PARAM, "QuotaExcludeFilter", FALSE);
  if (c && c->argc == 3) {
     quota_exclude_filter = c->argv[1];
     quota_exclude_re = c->argv[2];
  }

  c = find_config(main_server->conf, CONF_PARAM, "QuotaOptions", FALSE);
  if (c) {
    quotatab_opts = *((unsigned long *) c->argv[0]);
  }

  return 0;
}

/* Module API tables
 */

static conftable quotatab_conftab[] = {
  { "QuotaDirectoryTally",	set_quotadirtally,	NULL },
  { "QuotaDisplayUnits",	set_quotadisplayunits,	NULL },
  { "QuotaEngine",		set_quotaengine,	NULL },
  { "QuotaExcludeFilter",	set_quotaexcludefilter,	NULL },
  { "QuotaLimitTable",		set_quotatable,		NULL },
  { "QuotaLock",		set_quotalock,		NULL },
  { "QuotaLog",			set_quotalog,		NULL },
  { "QuotaOptions",		set_quotaoptions,	NULL },
  { "QuotaShowQuotas",		set_quotashowquotas,	NULL },
  { "QuotaTallyTable",		set_quotatable,		NULL },
  { NULL }
};

static cmdtable quotatab_cmdtab[] = {
  { PRE_CMD,		C_APPE, G_NONE, quotatab_pre_appe,	FALSE,	FALSE },
  { POST_CMD,		C_APPE,	G_NONE,	quotatab_post_appe,	FALSE,	FALSE },
  { POST_CMD_ERR,	C_APPE,	G_NONE,	quotatab_post_appe_err,	FALSE,	FALSE },
  { PRE_CMD,		C_DELE,	G_NONE,	quotatab_pre_dele,	FALSE,	FALSE },
  { POST_CMD,		C_DELE,	G_NONE,	quotatab_post_dele,	FALSE,	FALSE },
  { PRE_CMD,		C_MKD,	G_NONE,	quotatab_pre_mkd,	FALSE,	FALSE },
  { POST_CMD,		C_MKD,	G_NONE,	quotatab_post_mkd,	FALSE,	FALSE },
  { POST_CMD_ERR,	C_MKD,	G_NONE,	quotatab_post_mkd_err,	FALSE,	FALSE },
  { POST_CMD,		C_PASS,	G_NONE,	quotatab_post_pass,	FALSE,	FALSE },
  { PRE_CMD,		C_RETR, G_NONE, quotatab_pre_retr,	FALSE,	FALSE },
  { POST_CMD,		C_RETR,	G_NONE,	quotatab_post_retr,	FALSE,	FALSE },
  { POST_CMD_ERR,	C_RETR,	G_NONE,	quotatab_post_retr_err,	FALSE,	FALSE },
  { PRE_CMD,		C_RMD,	G_NONE,	quotatab_pre_rmd,	FALSE,	FALSE },
  { POST_CMD,		C_RMD,	G_NONE,	quotatab_post_rmd,	FALSE,	FALSE },
  { PRE_CMD,		C_RNTO,	G_NONE,	quotatab_pre_rnto,	FALSE,	FALSE },
  { POST_CMD,		C_RNTO,	G_NONE,	quotatab_post_rnto,	FALSE,	FALSE },
  { CMD,		C_SITE,	G_NONE,	quotatab_site,		FALSE,	FALSE, CL_MISC },
  { PRE_CMD,		C_STOR,	G_NONE, quotatab_pre_stor,	FALSE,	FALSE },
  { POST_CMD,		C_STOR,	G_NONE,	quotatab_post_stor,	FALSE,	FALSE },
  { POST_CMD_ERR,	C_STOR,	G_NONE,	quotatab_post_stor_err,	FALSE,	FALSE },
  { PRE_CMD,		C_STOU,	G_NONE,	quotatab_pre_stor,	FALSE,	FALSE },
  { POST_CMD,		C_STOU,	G_NONE,	quotatab_post_stor,	FALSE,	FALSE },
  { POST_CMD_ERR,	C_STOU,	G_NONE,	quotatab_post_stor_err,	FALSE,	FALSE },
  { PRE_CMD,		C_XMKD,	G_NONE,	quotatab_pre_mkd,	FALSE,	FALSE },
  { POST_CMD,		C_XMKD,	G_NONE,	quotatab_post_mkd,	FALSE,	FALSE },
  { POST_CMD_ERR,	C_XMKD,	G_NONE,	quotatab_post_mkd_err,	FALSE,	FALSE },
  { PRE_CMD,		C_XRMD,	G_NONE,	quotatab_pre_rmd,	FALSE,	FALSE },
  { POST_CMD,		C_XRMD,	G_NONE,	quotatab_post_rmd,	FALSE,	FALSE },
  { 0, NULL }
};

module quotatab_module = {
  NULL, NULL,

  /* Module API version 2.0 */
  0x20,

  /* Module name */
  "quotatab",

  /* Module configuration handler table */
  quotatab_conftab,

  /* Module command handler table */
  quotatab_cmdtab,

  /* Module authentication handler table */
  NULL,

  /* Module initialization function */
  quotatab_init,

  /* Session initialization function */
  quotatab_sess_init,

  /* Module version */
  MOD_QUOTATAB_VERSION
};
