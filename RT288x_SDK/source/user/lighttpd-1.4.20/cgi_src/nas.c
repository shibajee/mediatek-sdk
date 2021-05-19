#include "utils.h"
#include <stdlib.h>

#include "user_conf.h"

void set_ftp_cfg(char *input)
{
	web_debug_header();
	// fetch from web input
	WebTF(input, "ftp_enabled", RT2860_NVRAM, "FtpEnabled", 1);
	WebTF(input, "ftp_name", RT2860_NVRAM, "FtpName", 1);
	WebTF(input, "ftp_anonymous", RT2860_NVRAM, "FtpAnonymous", 1);
	WebTF(input, "ftp_port", RT2860_NVRAM, "FtpPort", 1);
	WebTF(input, "ftp_max_sessions", RT2860_NVRAM, "FtpMaxSessions", 1);
	WebTF(input, "ftp_adddir", RT2860_NVRAM, "FtpAddDir", 1);
	WebTF(input, "ftp_rename", RT2860_NVRAM, "FtpRename", 1);
	WebTF(input, "ftp_remove", RT2860_NVRAM, "FtpRemove", 1);
	WebTF(input, "ftp_read", RT2860_NVRAM, "FtpRead", 1);
	WebTF(input, "ftp_write", RT2860_NVRAM, "FtpWrite", 1);
	WebTF(input, "ftp_download", RT2860_NVRAM, "FtpDownload", 1);
	WebTF(input, "ftp_upload", RT2860_NVRAM, "FtpUpload", 1);
	// set to nvram
	nvram_commit(RT2860_NVRAM);

	// setup device
	do_system("storage.sh ftp");
}

static void set_smb()
{
	int i;
	const char *admin_id = nvram_bufget(RT2860_NVRAM, "Login");

	do_system("storage.sh samba");
	if (1 == strtol(nvram_bufget(RT2860_NVRAM, "SmbEnabled"), NULL, 10))
	{
		do_system("nmbd");
		do_system("smbd");
	}
}

void set_smb_cfg(char *input)
{
	web_debug_header();
	// fetch from web input
	WebTF(input, "smb_enabled", RT2860_NVRAM, "SmbEnabled", 1);
	WebTF(input, "smb_workgroup", RT2860_NVRAM, "HostName", 1);
	WebTF(input, "smb_netbios", RT2860_NVRAM, "SmbNetBIOS", 1);
	// set to nvram
	nvram_commit(RT2860_NVRAM);

	// setup device
	set_smb();
}

void add_dir(char *input)
{
	char *dir_name, *disk_part;

	dir_name = disk_part = NULL;
	dir_name = strdup(web_get("adddir_name", input, 0));
	disk_part = strdup(web_get("disk_part", input, 0));
	// DBG_MSG(dir_name);
	// DBG_MSG(disk_part);
	do_system("mkdir -p \"%s/%s\"", disk_part, dir_name);
	do_system("chmod 777 \"%s/%s\"", disk_part, dir_name);
	free_all(2, dir_name, disk_part);
	web_back_parentpage();
}

void remove_dir(char *input)
{
	do_system("rm -rf \"%s\"", web_get("dir_path", input, 0));
	web_redirect(getenv("HTTP_REFERER"));
}

void remove_disk(void)
{
	FILE *fp_mount = NULL;
	char part[30];

	if (NULL == (fp_mount = fopen("/proc/mounts", "r")))
	{
		DBG_MSG();
		return;
	}
	while(EOF != fscanf(fp_mount, "%s %*s %*s %*s %*s %*s\n", part))
	{
		if (NULL != strstr(part, "/dev/sd") || NULL != strstr(part, "/dev/mmc"))
			do_system("umount -l %s", part);
	}
	fclose(fp_mount);
	web_redirect(getenv("HTTP_REFERER"));
}

void nas_init(void)
{
	printf("\n##### Hotplug Storage init #####\n");
	sleep(1);           // wait for sub-storage initiation
#if defined CONFIG_USER_STORAGE && (defined CONFIG_USB || defined CONFIG_MMC)
#if defined CONFIG_USER_PROFTPD
	do_system("storage.sh ftp");
#endif
#if defined CONFIG_USER_SAMBA
	set_smb();
#endif
#endif
}

int main(int argc, char *argv[]) 
{
	char *form, *inStr;
	long inLen;

	nvram_init(RT2860_NVRAM);
	if ((argc > 1) && (!strcmp(argv[1], "init"))) {
		nas_init();
		goto leave;
	}
	inLen = strtol(getenv("CONTENT_LENGTH"), NULL, 10) + 1;
	if (inLen <= 1) {
		DBG_MSG("get no data!");
		goto leave;
	}
	inStr = malloc(inLen);
	memset(inStr, 0, inLen);
	fgets(inStr, inLen, stdin);
	//DBG_MSG("%s\n", inStr);
	form = web_get("page", inStr, 0);
	if (!strcmp(form, "ftp")) {
		set_ftp_cfg(inStr);
	} else if (!strcmp(form, "smb")) {
		set_smb_cfg(inStr);
	} else if (!strcmp(form, "adddir")) {
		add_dir(inStr);
	} else if (!strcmp(form, "rmdir")) {
		remove_dir(inStr);
	} else if (!strcmp(form, "rmdisk")) {
		remove_disk();
	}
	free(inStr);
leave:
	nvram_close(RT2860_NVRAM);
	
	return 0;
}
