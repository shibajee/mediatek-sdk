#include "utils.h"
#include <stdlib.h>

static int set_default(void)
{
	FILE *fp;
	int i;

	//retry 15 times (15 seconds)
	for (i = 0; i < 15; i++) {
		fp = fopen("/var/run/nvramd.pid", "r");
		if (fp == NULL) {
			if (i == 0)
				fprintf(stderr, "lighttpd: waiting for nvram_daemon ");
			else
				fprintf(stdout, ". ");
		}
		else {
			fclose(fp);
#if defined (RT2860_MBSS_SUPPORT)
			int max_bss_num = 8;
			int bssidnum = strtol(nvram_get(RT2860_NVRAM, "BssidNum"), NULL, 10);
			char newBssidNum[3];

			nvram_init(RT2860_NVRAM);
#if defined (RT2860_NEW_MBSS_SUPPORT)
			max_bss_num = 16;
#endif
#ifdef CONFIG_RT2860V2_AP_MESH
			max_bss_num--;
#endif
#if defined (RT2860_APCLI_SUPPORT)
			max_bss_num--;
#endif
			if (bssidnum > max_bss_num)
				bssidnum = max_bss_num;
			sprintf(newBssidNum, "%d", bssidnum);
			nvram_set(RT2860_NVRAM, "BssidNum", newBssidNum);
#endif
			nvram_init(RT2860_NVRAM);
#if defined (RTDEV_SUPPORT) || defined (CONFIG_RT2561_AP) || defined (CONFIG_RT2561_AP_MODULE)
			nvram_init(RTDEV_NVRAM);
#endif
			nvram_close(RT2860_NVRAM);

			return 0;
		}
		sleep(1);
	}
	fprintf(stderr, "goahead: please execute nvram_daemon first!");
	return (-1);
}

static void init_internet(void)
{
	do_system("internet.sh");
	do_system("/etc_ro/lighttpd/www/cgi-bin/wireless.cgi init");
	do_system("/etc_ro/lighttpd/www/cgi-bin/firewall.cgi init");
	do_system("/etc_ro/lighttpd/www/cgi-bin/adm.cgi init");
	do_system("/etc_ro/lighttpd/www/cgi-bin/internet.cgi init");
#if defined CONFIG_RALINKAPP_SWQOS || defined CONFIG_RALINKAPP_HWQOS
	do_system("/etc_ro/lighttpd/www/cgi-bin/qos.cgi init");
#endif
#if defined CONFIG_USER_STORAGE
	do_system("/etc_ro/lighttpd/www/cgi-bin/nas.cgi init");
	do_system("/etc_ro/lighttpd/www/cgi-bin/usb.cgi init");
#endif
}
static void usage(void)
{
	printf("Usage:\n");
	printf("\tinitial: init_system start\n");
	printf("\trestart: init_system restart\n");
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		usage();
		return -1;
	}
	if (!strcmp(argv[1], "start")) {
		if (set_default() < 0)
			return -1;
/* 	TODO:
	//signal(SIGTSTP, dhcpc_handler);
#if (defined CONFIG_USB) || (defined CONFIG_MMC)
	signal(SIGTTIN, hotPluglerHandler);
#endif
	goaInitGpio();
	signal(SIGXFSZ, WPSSingleTriggerHandler);
*/
		init_internet();
		do_system("lighttpd -f /etc_ro/lighttpd/lighttpd.conf -m /etc_ro/lighttpd/lib");
	} else if (!strcmp(argv[1], "restart")) {
		init_internet();
	} else {
		usage();
	}

	return 0;
}
