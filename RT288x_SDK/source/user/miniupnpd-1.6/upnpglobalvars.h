/* $Id: //WIFI_SOC/MP/SDK_4_3_0_0/RT288x_SDK/source/user/miniupnpd-1.6/upnpglobalvars.h#1 $ */
/* MiniUPnP project
 * http://miniupnp.free.fr/ or http://miniupnp.tuxfamily.org/
 * (c) 2006-2012 Thomas Bernard
 * This software is subject to the conditions detailed
 * in the LICENCE file provided within the distribution */

#ifndef __UPNPGLOBALVARS_H__
#define __UPNPGLOBALVARS_H__

#include <time.h>
#include "upnppermissions.h"
#include "miniupnpdtypes.h"
#include "config.h"

/* name of the network interface used to acces internet */
extern const char * ext_if_name;

/* file to store all leases */
#ifdef ENABLE_LEASEFILE
extern const char * lease_file;
#endif

#if 1//def ENABLE_WSC_SERVICE
#define FILE_PATH_LEN	256
extern char pid_file_path[FILE_PATH_LEN];

/* moved from miniupnpd.c */

/* structure containing variables used during "main loop"
 * that are filled during the init */
struct runtime_vars {
	/* LAN IP addresses for SSDP traffic and HTTP */
	/* moved to global vars */
	/*int n_lan_addr;*/
	/*struct lan_addr_s lan_addr[MAX_LAN_ADDR];*/
	int port;	/* HTTP Port */
	int notify_interval;	/* seconds between SSDP announces */
	/* unused rules cleaning related variables : */
	int clean_ruleset_threshold;	/* threshold for removing unused rules */
	int clean_ruleset_interval;		/* (minimum) interval between checks */
};
#endif /* ENABLE_WSC_SERVICE */

/* forced ip address to use for this interface
 * when NULL, getifaddr() is used */
extern const char * use_ext_ip_addr;

/* parameters to return to upnp client when asked */
extern unsigned long downstream_bitrate;
extern unsigned long upstream_bitrate;

/* statup time */
extern time_t startup_time;

/* runtime boolean flags */
extern int runtime_flags;
#define LOGPACKETSMASK		0x0001
#define SYSUPTIMEMASK		0x0002
#ifdef ENABLE_NATPMP
#define ENABLENATPMPMASK	0x0004
#endif
#define CHECKCLIENTIPMASK	0x0008
#define SECUREMODEMASK		0x0010

#define ENABLEUPNPMASK		0x0020

#ifdef PF_ENABLE_FILTER_RULES
#define PFNOQUICKRULESMASK	0x0040
#endif

#ifdef ENABLE_WSC_SERVICE
#define ENABLEWPSMASK		0x8000

extern char wps_uuidvalue[];
extern char wps_serialnumber[];
#endif /* ENABLE_WSC_SERVICE */

#define SETFLAG(mask)	runtime_flags |= mask
#define GETFLAG(mask)	(runtime_flags & mask)
#define CLEARFLAG(mask)	runtime_flags &= ~mask

extern const char * pidfilename;

extern char uuidvalue[];

#define SERIALNUMBER_MAX_LEN (10)
extern char serialnumber[];

#define MODELNUMBER_MAX_LEN (48)
extern char modelnumber[];

#define PRESENTATIONURL_MAX_LEN (64)
extern char presentationurl[];

#define FRIENDLY_NAME_MAX_LEN (64)
extern char friendly_name[];

/* UPnP permission rules : */
extern struct upnpperm * upnppermlist;
extern unsigned int num_upnpperm;

#ifdef ENABLE_NATPMP
/* NAT-PMP */
#if 0
extern unsigned int nextnatpmptoclean_timestamp;
extern unsigned short nextnatpmptoclean_eport;
extern unsigned short nextnatpmptoclean_proto;
#endif
#endif

/* For automatic removal of expired rules (with LeaseDuration) */
extern unsigned int nextruletoclean_timestamp;

#ifdef USE_PF
extern const char * anchor_name;
/* queue and tag for PF rules */
extern const char * queue;
extern const char * tag;
#endif

#ifdef USE_NETFILTER
extern const char * miniupnpd_nat_chain;
extern const char * miniupnpd_forward_chain;
#endif

#ifdef ENABLE_NFQUEUE
extern int nfqueue;
extern int n_nfqix;
extern unsigned nfqix[];
#endif

/* lan addresses to listen to SSDP traffic */
extern struct lan_addr_list lan_addrs;

#ifdef ENABLE_IPV6
/* ipv6 address used for HTTP */
extern char ipv6_addr_for_http_with_brackets[64];
#endif

extern const char * minissdpdsocketpath;

/* BOOTID.UPNP.ORG and CONFIGID.UPNP.ORG */
extern unsigned int upnp_bootid;
extern unsigned int upnp_configid;

#ifdef ENABLE_6FC_SERVICE
extern int ipv6fc_firewall_enabled;
extern int ipv6fc_inbound_pinhole_allowed;
#endif

#endif

