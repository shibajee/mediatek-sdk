#include "utils.h"
#include <stdlib.h>
#include <arpa/inet.h>

#include "user_conf.h"
#include "busybox_conf.h"


#if (defined CONFIG_RALINK_WATCHDOG || defined CONFIG_RALINK_WATCHDOG_MODULE) && defined CONFIG_USER_WATCHDOG
static void watchdog_restart(void)
{
	do_system("killall -9 watchdog");
	do_system("rmmod ralink_wdt.o");
	if (strcmp(nvram_get(RT2860_NVRAM, "WatchDogEnable"), "1") == 0) {
		do_system("insmod -q ralink_wdt.o");
		do_system("wdg.sh");
		do_system("watchdog");
	}
}
#endif

void set_sys_adm(char *input)
{
	char *admuser, *admpass;
	char *old_user;
	char tmp[128];

	admuser = admpass = NULL;
	old_user = (char *) nvram_bufget(RT2860_NVRAM, "Login");
	admuser = strdup(web_get("admuser", input, 0));
	admpass = strdup(web_get("admpass", input, 0));

	if (!strlen(admuser)) {
		DBG_MSG("account empty, leave it unchanged");
		goto leave;
	}
	if (!strlen(admpass)) {
		DBG_MSG("password empty, leave it unchanged");
		goto leave;
	}
#if (defined CONFIG_RALINK_WATCHDOG || defined CONFIG_RALINK_WATCHDOG_MODULE) && defined CONFIG_USER_WATCHDOG
	nvram_bufset(RT2860_NVRAM, "WatchDogEnable", web_get("admwatchdog", input, 1));
#endif
	/* modify /etc/passwd to new user name and passwd */
	do_system("sed -e 's/^%s:/%s:/' /etc/passwd > /etc/newpw", old_user, admuser);
	do_system("cp /etc/newpw /etc/passwd");
	do_system("rm -f /etc/newpw");
	do_system("chpasswd.sh %s %s", admuser, admpass);
	/* TODO: re-generate lighttpd.conf & restart lighttpd */

	nvram_bufset(RT2860_NVRAM, "Login", admuser);
	nvram_bufset(RT2860_NVRAM, "Password", admpass);
	nvram_commit(RT2860_NVRAM);

	sprintf(tmp, "echo \"%s:%s\" > /etc/lighttpd.user", admuser, admpass);
	system(tmp);
#if (defined CONFIG_RALINK_WATCHDOG || defined CONFIG_RALINK_WATCHDOG_MODULE) && defined CONFIG_USER_WATCHDOG
	watchdog_restart();
#endif

leave:
	free_all(2, admuser, admpass);
	sprintf(tmp, "http://%s/index.html", nvram_bufget(RT2860_NVRAM, "lan_ipaddr"));
	web_redirect_wholepage(tmp);
	sleep(5);
	do_system("kill -9 `cat /var/run/lighttpd.pid`");
	do_system("lighttpd -f /etc_ro/lighttpd/lighttpd.conf -m /etc_ro/lighttpd/lib");

	return;
}

#if defined CONFIG_DATE
void ntp_sync_host(char *time)
{
	if(!time || (!strlen(time)))
		return;
	if(strchr(time, ';'))
		return;
	do_system("date -s %s", time);
	do_system("config-udhcpd.sh -r 1");
}

void set_ntp(char *input)
{
	char *host_time;
	char *tz, *ntpServer, *ntpSync;

	host_time = web_get("hostTime", input, 0);
	if (strlen(host_time) > 0) {
		ntp_sync_host(host_time);
		web_redirect(getenv("HTTP_REFERER"));
		return;
	}
	tz = ntpServer = ntpSync = NULL;
	web_debug_header();
	tz = strdup(web_get("time_zone", input, 1));
	ntpServer = strdup(web_get("NTPServerIP", input, 1));
	ntpSync = strdup(web_get("NTPSync", input, 1));

#if 1
	if(!tz || !ntpServer || !ntpSync)
		goto leave;

	if(!strlen(tz))
		goto leave;

	if(check_semicolon(tz))
		goto leave;

	if(!strlen(ntpServer)){
		// user choose to make  NTP server disable
		nvram_bufset(RT2860_NVRAM, "NTPServerIP", "");
		nvram_bufset(RT2860_NVRAM, "NTPSync", "");
	}else{
		if(check_semicolon(ntpServer))
			goto leave;
		if(!strlen(ntpSync))
			goto leave;
		if(strtol(ntpSync, NULL, 10) > 300)
			goto leave;
		nvram_bufset(RT2860_NVRAM, "NTPServerIP", ntpServer);
		nvram_bufset(RT2860_NVRAM, "NTPSync", ntpSync);
	}
#else
		nvram_bufset(RT2860_NVRAM, "NTPServerIP", ntpServer);
		nvram_bufset(RT2860_NVRAM, "NTPSync", ntpSync);
#endif
	nvram_bufset(RT2860_NVRAM, "TZ", tz);
	nvram_commit(RT2860_NVRAM);

	do_system("ntp.sh");

leave:
	free_all(3,tz, ntpServer, ntpSync);

	return;
}
#endif

#if defined CONFIG_USER_GREENAP && defined CONFIG_CROND && defined CONFIG_CRONTAB
void set_greenap(char *input)
{
	char *shour1, *sminute1, *ehour1, *eminute1;
	char *shour2, *sminute2, *ehour2, *eminute2;
	char *shour3, *sminute3, *ehour3, *eminute3;
	char *shour4, *sminute4, *ehour4, *eminute4;
	char start[6], end[6];

	shour1 = sminute1 = ehour1 = eminute1 = NULL;
	shour2 = sminute2 = ehour2 = eminute2 = NULL;
	shour3 = sminute3 = ehour3 = eminute3 = NULL;
	shour4 = sminute4 = ehour4 = eminute4 = NULL;
	web_debug_header();
	shour1 = strdup(web_get("GAPSHour1", input, 1));
	sminute1 = strdup(web_get("GAPSMinute1", input, 1));
	ehour1 = strdup(web_get("GAPEHour1", input, 1));
	eminute1 = strdup(web_get("GAPEMinute1", input, 1));
	sprintf(start, "%s %s", sminute1, shour1);
	sprintf(end, "%s %s", eminute1, ehour1);
	nvram_bufset(RT2860_NVRAM, "GreenAPStart1", start);
	nvram_bufset(RT2860_NVRAM, "GreenAPEnd1", end);
	WebTF(input, "GAPAction1", RT2860_NVRAM, "GreenAPAction1", 1);
	shour2 = strdup(web_get("GAPSHour2", input, 1));
	sminute2 = strdup(web_get("GAPSMinute2", input, 1));
	ehour2 = strdup(web_get("GAPEHour2", input, 1));
	eminute2 = strdup(web_get("GAPEMinute2", input, 1));
	sprintf(start, "%s %s", sminute2, shour2);
	sprintf(end, "%s %s", eminute2, ehour2);
	nvram_bufset(RT2860_NVRAM, "GreenAPStart2", start);
	nvram_bufset(RT2860_NVRAM, "GreenAPEnd2", end);
	WebTF(input, "GAPAction2", RT2860_NVRAM, "GreenAPAction2", 1);
	shour3 = strdup(web_get("GAPSHour3", input, 1));
	sminute3 = strdup(web_get("GAPSMinute3", input, 1));
	ehour3 = strdup(web_get("GAPEHour3", input, 1));
	eminute3 = strdup(web_get("GAPEMinute3", input, 1));
	sprintf(start, "%s %s", sminute3, shour3);
	sprintf(end, "%s %s", eminute3, ehour3);
	nvram_bufset(RT2860_NVRAM, "GreenAPStart3", start);
	nvram_bufset(RT2860_NVRAM, "GreenAPEnd3", end);
	WebTF(input, "GAPAction3", RT2860_NVRAM, "GreenAPAction3" ,1);
	shour4 = strdup(web_get("GAPSHour4", input, 1));
	sminute4 = strdup(web_get("GAPSMinute4", input, 1));
	ehour4 = strdup(web_get("GAPEHour4", input, 1));
	eminute4 = strdup(web_get("GAPEMinute4", input, 1));
	sprintf(start, "%s %s", sminute4, shour4);
	sprintf(end, "%s %s", eminute4, ehour4);
	nvram_bufset(RT2860_NVRAM, "GreenAPStart4", start);
	nvram_bufset(RT2860_NVRAM, "GreenAPEnd4", end);
	WebTF(input, "GAPAction4", RT2860_NVRAM, "GreenAPAction4", 1);
	nvram_commit(RT2860_NVRAM);

	do_system("greenap.sh init");

	free_all(16, shour1, sminute1, ehour1, eminute1,
		     shour2, sminute2, ehour2, eminute2,
		     shour3, sminute3, ehour3, eminute3,
		     shour4, sminute4, ehour4, eminute4);

	return;
}
#endif

#ifdef CONFIG_USER_INADYN
void set_ddns(char *input)
{
	char *ddns_provider, *ddns, *ddns_acc, *ddns_pass;
	char empty_char = '\0';

	ddns_provider = ddns = ddns_acc = ddns_pass = NULL;
	web_debug_header();
	ddns_provider = strdup(web_get("DDNSProvider", input, 1));
	ddns = strdup(web_get("DDNS", input, 1));
	ddns_acc = strdup(web_get("Account", input, 1));
	ddns_pass = strdup(web_get("Password", input, 1));

	if(!ddns_provider || !ddns || !ddns_acc || !ddns_pass)
		goto leave;

	if(!strcmp("none", ddns_provider )) {
		ddns = ddns_acc = ddns_pass = &empty_char;
	} else {
		if(!strlen(ddns) || !strlen(ddns_acc) || !strlen(ddns_pass))
			goto leave;
	}

	if(check_semicolon(ddns) || check_semicolon(ddns_acc) || check_semicolon(ddns_pass))
		goto leave;

	nvram_bufset(RT2860_NVRAM, "DDNSProvider", ddns_provider);
	nvram_bufset(RT2860_NVRAM, "DDNS", ddns);
	nvram_bufset(RT2860_NVRAM, "DDNSAccount", ddns_acc);
	nvram_bufset(RT2860_NVRAM, "DDNSPassword", ddns_pass);
	nvram_commit(RT2860_NVRAM);

	do_system("ddns.sh");

leave:
	free_all(4, ddns_provider, ddns, ddns_acc, ddns_pass);

	return;
}
#endif

void load_default(void)
{
	do_system("ralink_init clear 2860");
#if defined CONFIG_LAN_WAN_SUPPORT || defined CONFIG_MAC_TO_MAC_MODE
	do_system("ralink_init renew 2860 /etc_ro/Wireless/RT2860AP/RT2860_default_vlan");
#elif defined CONFIG_ICPLUS_PHY
	do_system("ralink_init renew 2860 /etc_ro/Wireless/RT2860AP/RT2860_default_oneport");
#else
	do_system("ralink_init renew 2860 /etc_ro/Wireless/RT2860AP/RT2860_default_novlan");
#endif
#if defined RTDEV_SUPPORT
	do_system("ralink_init clear rtdev");
	do_system("ralink_init renew rtdev /etc_ro/Wireless/iNIC/RT2860AP.dat");
#endif
#if defined CONFIG_MTK_VOIP
	system("ralink_init clear voip");
	system("ralink_init renew voip /etc_ro/voip_default_settings");
#endif
	do_system("reboot");
}

#if defined CONFIG_LOGREAD && defined CONFIG_KLOGD
void clear_log(char *input)
{
	do_system("killall -q klogd");
	do_system("killall -q syslogd");
	sleep(1);
	do_system("syslogd -C8");
	do_system("klogd");
	web_redirect(getenv("HTTP_REFERER"));
}
#endif

void set_sys_cmd(char *input)
{
	char *cmd = web_get("command", input, 0);

	if(!cmd)
		return;

	if(!strlen(cmd))
		do_system("cat /dev/null > %s", SYSTEM_COMMAND_LOG);
	else
		do_system("%s 1>%s 2>&1", cmd, SYSTEM_COMMAND_LOG);
	do_system("echo \"%s\" > /var/last_system_command", cmd);
	web_redirect(getenv("HTTP_REFERER"));

	return;
}

void set_last_cmd(char *input)
{
	do_system("sh < /var/last_system_command 1>%s 2>&1", SYSTEM_COMMAND_LOG);
	web_redirect(getenv("HTTP_REFERER"));

	return;
}

void set_opmode(char *input)
{
	char *value, *mode;
	const char *old_mode = nvram_bufget(RT2860_NVRAM, "OperationMode");
	const char *old_nat = nvram_bufget(RT2860_NVRAM, "natEnabled");
	int need_commit = 0;
#if defined CONFIG_RT2860V2_STA_DPB || defined CONFIG_RT2860V2_STA_ETH_CONVERT
	char  *econv, *econv_mode;
#endif
#if defined CONFIG_RA_NAT_HW
	const char  *old_hwnat = nvram_bufget(RT2860_NVRAM, "hwnatEnabled");
#endif

	web_debug_header();
	mode = strdup(web_get("opMode", input, 1));
	value = web_get("tcp_timeout", input, 1);
	if (strcmp(value, nvram_get(RT2860_NVRAM, "TcpTimeout"))) {
		nvram_set(RT2860_NVRAM, "TcpTimeout", value);
		need_commit = 1;
	}
	value = web_get("udp_timeout", input, 1);
	if (strcmp(value, nvram_get(RT2860_NVRAM, "UdpTimeout"))) {
		nvram_set(RT2860_NVRAM, "UdpTimeout", value);
		need_commit = 1;
	}

	if (!strncmp(old_mode, "0", 2)) {
	}
	else if (!strncmp(old_mode, "1", 2) || !strncmp(old_mode, "3", 2)) {
		if (!strncmp(mode, "0", 2)) {
#if defined CONFIG_RAETH_ROUTER || defined CONFIG_MAC_TO_MAC_MODE || defined CONFIG_RT_3052_ESW || defined CONFIG_ICPLUS_PHY
#else
			/*
			 * mode: gateway (or ap-client) -> bridge
			 * config: wan_ip(wired) overwrites lan_ip(bridge)
			 */
			nvram_bufset(RT2860_NVRAM, "lan_ipaddr", nvram_bufget(RT2860_NVRAM, "wan_ipaddr"));
			need_commit = 1;
#endif
		}
		if (!strncmp(mode, "2", 2)) {
#if defined CONFIG_RAETH_ROUTER || defined CONFIG_MAC_TO_MAC_MODE || defined CONFIG_RT_3052_ESW || defined CONFIG_ICPLUS_PHY
#else
			/*
			 * mode: gateway (or ap-client) -> ethernet-converter
			 * config: wan_ip(wired) overwrites lan_ip(wired)
			 *         lan_ip(wireless) overwrites wan_ip(wireless)
			 */
			nvram_bufset(RT2860_NVRAM, "lan_ipaddr", nvram_bufget(RT2860_NVRAM, "wan_ipaddr"));
			nvram_bufset(RT2860_NVRAM, "wan_ipaddr", nvram_bufget(RT2860_NVRAM, "lan_ipaddr"));
			need_commit = 1;
#endif
		}
	}
	else if (!strncmp(old_mode, "2", 2)) {
		if (!strncmp(mode, "0", 2)) {
			/*
			 * mode: wireless-isp -> bridge
			 * config: lan_ip(wired) overwrites lan_ip(bridge) -> the same
			 */
		}
		else if (!strncmp(mode, "1", 2) || !strncmp(mode, "3", 2)) {
#if defined CONFIG_RAETH_ROUTER || defined CONFIG_MAC_TO_MAC_MODE || defined CONFIG_RT_3052_ESW || defined CONFIG_ICPLUS_PHY
#else
			/*
			 * mode: ethernet-converter -> gateway (or ap-client)
			 * config: lan_ip(wired) overwrites wan_ip(wired)
			 *         wan_ip(wireless) overwrites lan_ip(wireless)
			 */
			nvram_bufset(RT2860_NVRAM, "lan_ipaddr", nvram_bufget(RT2860_NVRAM, "wan_ipaddr"));
			nvram_bufset(RT2860_NVRAM, "wan_ipaddr", nvram_bufget(RT2860_NVRAM, "lan_ipaddr"));
			need_commit = 1;
#endif
		}
	}

#if defined CONFIG_RT2860V2_STA_DPB || defined CONFIG_RT2860V2_STA_ETH_CONVERT
	if (!strncmp(mode, "0", 2)) {
		const char *old;

		econv = strdup(web_get("ethConv", input, 1));
		econv_mode = strdup(web_get("oEthConvMode", input, 1));
		old = nvram_bufget(RT2860_NVRAM, "ethConvert");
		if (strncmp(old, econv, 2)) {
			nvram_bufset(RT2860_NVRAM, "ethConvert", econv);
			need_commit = 1;
		}
		if (!strcmp(econv_mode, "0"))
			nvram_bufset(RT2860_NVRAM, "ethConvertMAC", "00:00:00:00:00:00");
		else if (!strcmp(econv_mode, "1"))
			nvram_bufset(RT2860_NVRAM, "ethConvertMAC", "FF:FF:FF:FF:FF:FF");
		else
			nvram_bufset(RT2860_NVRAM, "ethConvertMAC", web_get("oEthConvMac", input, 1));
		if (!strncmp(econv, "1", 2)) {
			//disable dhcp server in this mode
			old = nvram_bufget(RT2860_NVRAM, "dhcpEnabled");
			if (!strncmp(old, "1", 2)) {
				nvram_bufset(RT2860_NVRAM, "dhcpEnabled", "0");
				need_commit = 1;
			}
		}
	}
#endif
#if defined RT2860_APCLI_SUPPORT
	if (!strncmp(mode, "0", 2)) {
		nvram_bufset(RT2860_NVRAM, "apClient", web_get("apcliEnbl", input, 1));
		need_commit = 1;
	}
#endif

	//new OperationMode
	if (strncmp(mode, old_mode, 2)) {
		nvram_bufset(RT2860_NVRAM, "OperationMode", mode);

		//from or to ap client mode
		if (!strncmp(mode, "3", 2))
			nvram_bufset(RT2860_NVRAM, "ApCliEnable", "1");
		else if (!strncmp(old_mode, "3", 2))
			nvram_bufset(RT2860_NVRAM, "ApCliEnable", "0");
		need_commit = 1;
	}

	value = web_get("natEnbl", input, 1);
	if (strncmp(value, old_nat, 2)) {
		nvram_bufset(RT2860_NVRAM, "natEnabled", value);
		need_commit = 1;
	}
#ifdef CONFIG_RA_NAT_HW
	value = web_get("hwnatEnbl", input, 1);
	if (strncmp(value, old_hwnat, 2)) {
		nvram_bufset(RT2860_NVRAM, "hwnatEnabled", value);
		need_commit = 1;
	}
#endif

	/*
	 * For 100PHY  ( Ethernet Convertor with one port only)
	 * If this is one port only board(IC+ PHY) then redirect
	 * the user browser to our alias ip address.
	 */
	if(is_oneport_only()){
		//     old mode is Gateway, and new mode is BRIDGE/WirelessISP/Apcli
		if (    (!strcmp(old_mode, "1") && !strcmp(mode, "0"))  ||
				(!strcmp(old_mode, "1") && !strcmp(mode, "2"))  ||
				(!strcmp(old_mode, "1") && !strcmp(mode, "3"))  ){
			char redirect_url[512];
			const char *lan_ip = nvram_bufget(RT2860_NVRAM, "lan_ipaddr");

			if(! strlen(lan_ip))
				lan_ip = "10.10.10.254";
			snprintf(redirect_url, 512, "http://%s", lan_ip);
			web_redirect_wholepage(redirect_url);
			goto leave;
		}

		//     old mode is BRIDGE/WirelessISP/Apcli, and new mode is Gateway
		if (    (!strcmp(old_mode, "0") && !strcmp(mode, "1"))  ||
				(!strcmp(old_mode, "2") && !strcmp(mode, "1"))  ||
				(!strcmp(old_mode, "3") && !strcmp(mode, "1"))  ){
			web_redirect_wholepage("http://172.32.1.254");
			goto leave;
		}
	}

#if defined CONFIG_RTDEV_MII
	if (!strncmp(mode, "0", 2)) {
		const char *old_mii = nvram_bufget(RTDEV_NVRAM, "InicMiiEnable");

		value = web_get("miiMode", input, 1);
		if (strncmp(value, old_mii, 2)) {
			nvram_set(RTDEV_NVRAM, "InicMiiEnable", value);
			need_commit = 1; //force to run initInternet
		}
	}
	else {
		nvram_set(RTDEV_NVRAM, "InicMiiEnable", "0");
		need_commit = 1; //force to run initInternet
	}
#endif

leave:
	sleep(1);   // wait for websDone() to finish tcp http session(close socket)
	free(mode);
#if defined CONFIG_RT2860V2_STA_DPB || defined CONFIG_RT2860V2_STA_ETH_CONVERT
	free_all(2, econv, econv_mode);
#endif

	//restart internet if any changes
	if (need_commit) {
		nvram_commit(RT2860_NVRAM);
		update_flash_8021x(RT2860_NVRAM);
		do_system("init_system restart");
	}

	return;
}

void management_init(void)
{
	const char *user = nvram_bufget(RT2860_NVRAM, "Login");
	const char *pw = nvram_bufget(RT2860_NVRAM, "Password");
	char tmp[128];

	sprintf(tmp, "echo \"%s:%s\" > /etc/lighttpd.user", user, pw);
	system(tmp);
	do_system("ntp.sh");
#if defined CONFIG_USER_GREENAP && defined CONFIG_CROND && defined CONFIG_CRONTAB
	do_system("greenap.sh init");
#endif
	do_system("ddns.sh");
#if defined CONFIG_RALINK_WATCHDOG|CONFIG_RALINK_WATCHDOG_MODULE && defined CONFIG_USER_WATCHDOG
	watchdog_restart();
#endif
	do_system("killall -q klogd");
	do_system("killall -q syslogd");
	do_system("syslogd -C8");
	do_system("klogd");
}

int main(int argc, char *argv[]) 
{
	char *form, *inStr;
	long inLen;

	nvram_init(RT2860_NVRAM);
	if ((argc > 1) && (!strcmp(argv[1], "init"))) {
		management_init();
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
	if (!strcmp(form, "sysAdm")) {
		set_sys_adm(inStr);
#if defined CONFIG_DATE
	} else if (!strcmp(form, "ntp")) {
		set_ntp(inStr);
#endif
#if defined CONFIG_USER_GREENAP && defined CONFIG_CROND && defined CONFIG_CRONTAB
	} else if (!strcmp(form, "greenap")) {
		set_greenap(inStr);
#endif
#if defined CONFIG_USER_INADYN
	} else if (!strcmp(form, "ddns")) {
		set_ddns(inStr);
#endif
	} else if (!strcmp(form, "loaddefault")) {
		load_default();
#if defined CONFIG_LOGREAD && defined CONFIG_KLOGD
	} else if (!strcmp(form, "clearlog")) {
		clear_log(inStr);
#endif
	} else if (!strcmp(form, "sysCMD")) {
		set_sys_cmd(inStr);
	} else if (!strcmp(form, "repeatLastCMD")) {
		set_last_cmd(inStr);
	} else if (!strcmp(form, "OPMode")) {
		set_opmode(inStr);
	}
	free(inStr);
leave:
	nvram_close(RT2860_NVRAM);
	
	return 0;
}
