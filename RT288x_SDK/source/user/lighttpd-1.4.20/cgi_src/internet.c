#include "utils.h"
#include <stdlib.h>
#include <net/route.h>

#include "user_conf.h"
#include "busybox_conf.h"

void set_wan(char *input)
{
	char *pppoe_opmode, *l2tp_mode, *l2tp_opmode, *pptp_mode, *pptp_opmode;

	pppoe_opmode = l2tp_mode = l2tp_opmode = pptp_mode = pptp_opmode = NULL;
	web_debug_header();
	WebTF(input, "connectionType", RT2860_NVRAM, "wanConnectionMode", 1);
	/* STATIC */
	WebTF(input, "staticIp", RT2860_NVRAM, "wan_ipaddr", 1);
	WebTF(input, "staticNetmask", RT2860_NVRAM, "wan_netmask", 1);
	WebTF(input, "staticGateway", RT2860_NVRAM, "wan_gateway", 1);
	WebTF(input, "staticPriDns", RT2860_NVRAM, "wan_primary_dns", 1);
	WebTF(input, "staticSecDns", RT2860_NVRAM, "wan_secondary_dns", 1);
	/* DHCP */
	WebTF(input, "hostname", RT2860_NVRAM, "wan_dhcp_hn", 1);
	/* PPPoE */
	WebTF(input, "pppoeUser", RT2860_NVRAM, "wan_pppoe_user", 1);
	WebTF(input, "pppoePass", RT2860_NVRAM, "wan_pppoe_pass", 1);
	pppoe_opmode = strdup(web_get("pppoeOPMode", input, 1));
	nvram_bufset(RT2860_NVRAM, "wan_pppoe_opmode", pppoe_opmode);
	if (!strncmp(pppoe_opmode, "OnDemand", 8))
		WebTF(input, "pppoeIdleTime", RT2860_NVRAM, "wan_pppoe_optime", 1);
	else
		nvram_bufset(RT2860_NVRAM, "wan_pppoe_optime", "");
	/* L2TP */
	WebTF(input, "l2tpServer", RT2860_NVRAM, "wan_l2tp_server", 1);
	WebTF(input, "l2tpUser", RT2860_NVRAM, "wan_l2tp_user", 1);
	WebTF(input, "l2tpPass", RT2860_NVRAM, "wan_l2tp_pass", 1);
	l2tp_mode = strdup(web_get("l2tpMode", input, 1));
	nvram_bufset(RT2860_NVRAM, "wan_l2tp_mode", l2tp_mode);
	if (!strncmp(l2tp_mode, "0", 1)) {
		WebTF(input, "l2tpIp", RT2860_NVRAM, "wan_l2tp_ip", 1);
		WebTF(input, "l2tpNetmask", RT2860_NVRAM, "wan_l2tp_netmask", 1);
		WebTF(input, "l2tpGateway", RT2860_NVRAM, "wan_l2tp_gateway", 1);
	}
	l2tp_opmode = strdup(web_get("l2tpOPMode", input, 1));
	nvram_bufset(RT2860_NVRAM, "wan_l2tp_optime", l2tp_opmode);
	if (!strncmp(l2tp_opmode, "KeepAlive", 9))
		WebTF(input, "l2tpOPMode", RT2860_NVRAM, "wan_l2tp_optime", 1);
#if 0
	else if (!strncmp(l2tp_opmode, "OnDemand", 8))
		WebTF(input, "l2tpIdleTime", RT2860_NVRAM, "wan_l2tp_optime", 1);
#endif
	else
		nvram_bufset(RT2860_NVRAM, "wan_l2tp_optime", "");
	/* PPTP */
	WebTF(input, "pptpServer", RT2860_NVRAM, "wan_pptp_server", 1);
	WebTF(input, "pptpUser", RT2860_NVRAM, "wan_pptp_user", 1);
	WebTF(input, "pptpPass", RT2860_NVRAM, "wan_pptp_pass", 1);
	pptp_mode = strdup(web_get("pptpMode", input, 1));
	nvram_bufset(RT2860_NVRAM, "wan_pptp_mode", pptp_mode);
	if (!strncmp(pptp_mode, "0", 1)) {
		WebTF(input, "pptpIp", RT2860_NVRAM, "wan_pptp_ip", 1);
		WebTF(input, "pptpNetmask", RT2860_NVRAM, "wan_pptp_netmask", 1);
		WebTF(input, "pptpGateway", RT2860_NVRAM, "wan_pptp_gateway", 1);
	}
	WebTF(input, "pptpOPMode", RT2860_NVRAM, "wan_pptp_opmode", 1);
	pptp_opmode = strdup(web_get("pptpOPMode", input, 1));
	nvram_bufset(RT2860_NVRAM, "wan_pptp_opmode", pptp_opmode);
	if (!strncmp(pptp_opmode, "KeepAlive", 9))
		WebTF(input, "pptpRedialPeriod", RT2860_NVRAM, "wan_pptp_optime", 1);
#if 0
	else if (!strncmp(pptp_opmode, "OnDemand", 8))
		WebTF(input, "pptpIdleTime", RT2860_NVRAM, "wan_pptp_optime", 1);
#endif
	else
		nvram_bufset(RT2860_NVRAM, "wan_pptp_optime", "");
#ifdef CONFIG_USER_3G
	/* 3G */
	WebTF(input, "Dev3G", RT2860_NVRAM, "wan_3g_dev", 1);
	WebTF(input, "APN3G", RT2860_NVRAM, "wan_3g_apn", 1);
	WebTF(input, "PIN3G", RT2860_NVRAM, "wan_3g_pin", 1);
	WebTF(input, "Dial3G", RT2860_NVRAM, "wan_3g_dial", 1);
	WebTF(input, "User3G", RT2860_NVRAM, "wan_3g_user", 1);
	WebTF(input, "Password3G", RT2860_NVRAM, "wan_3g_pass", 1);
	//WebTF(input, "OPMode3G", RT2860_NVRAM, "wan_3g_opmode", 1);
#endif
	/* MAC Clone */
	WebTF(input, "macCloneEnbl", RT2860_NVRAM, "macCloneEnabled", 1);
	WebTF(input, "macCloneMac", RT2860_NVRAM, "macCloneMac", 1);

	nvram_commit(RT2860_NVRAM);
	do_system("init_system restart");
}

void set_lan(char *input)
{
	char *ip, *lan2enabled, *lan2_ip;
	char *dhcp_tp, *dhcp_s, *dhcp_e, *dhcp_m;
	const char  *opmode, *wan_ip, *ctype;


	/*
	 *      * check static ip address:
	 *           * lan and wan ip should not be the same except in bridge mode
	 */
	ip = lan2enabled = lan2_ip = dhcp_tp = dhcp_s = dhcp_e = dhcp_m = NULL;
	web_debug_header();
	ip = strdup(web_get("lanIp", input, 1));
	WebTF(input, "lanNetmask", RT2860_NVRAM, "lan_netmask", 1);
	lan2enabled = strdup(web_get("lan2enabled", input, 1));
	lan2_ip = strdup(web_get("lan2Ip", input, 1));
	WebTF(input, "lan2Netmask", RT2860_NVRAM, "lan2_netmask", 1);
#if defined CONFIG_HOSTNAME
	WebTF(input, "hostname", RT2860_NVRAM, "HostName", 1);
#endif
	dhcp_tp = strdup(web_get("lanDhcpType", input, 1));
	WebTF(input, "stpEnbl", RT2860_NVRAM, "stpEnabled", 1);
	WebTF(input, "lltdEnbl", RT2860_NVRAM, "lltdEnabled" ,1);
	WebTF(input, "igmpEnbl", RT2860_NVRAM, "igmpEnabled", 1);
	WebTF(input, "upnpEnbl", RT2860_NVRAM, "upnpEnabled", 1);
	WebTF(input, "radvdEnbl", RT2860_NVRAM, "radvdEnabled", 1);
	WebTF(input, "pppoeREnbl", RT2860_NVRAM, "pppoeREnabled", 1);
	WebTF(input, "dnspEnbl", RT2860_NVRAM, "dnsPEnabled" ,1);
	dhcp_s = strdup(web_get("dhcpStart", input, 1));
	dhcp_e = strdup(web_get("dhcpEnd", input, 1));
	dhcp_m = strdup(web_get("dhcpMask", input, 1));

	nvram_bufset(RT2860_NVRAM, "lan_ipaddr", ip);
	nvram_bufset(RT2860_NVRAM, "Lan2Enabled", lan2enabled);
	nvram_bufset(RT2860_NVRAM, "lan2_ipaddr", lan2_ip);
	opmode = nvram_bufget(RT2860_NVRAM, "OperationMode");
	ctype = nvram_bufget(RT2860_NVRAM, "connectionType");
	if (strncmp(ctype, "STATIC", 7)) {
		wan_ip = nvram_bufget(RT2860_NVRAM, "wan_ipaddr");
		if (strcmp(opmode, "0") && !strncmp(ip, wan_ip, 15)) {
			DBG_MSG("IP address is identical to WAN");
			goto leave;
		}
		if (!strcmp(lan2enabled, "1")) {
			if (strcmp(opmode, "0") && !strncmp(lan2_ip, wan_ip, 15)) {
				DBG_MSG("LAN2 IP address is identical to WAN");
				goto leave;
			}
			else if (strcmp(opmode, "0") && !strncmp(lan2_ip, ip, 15)) {
				DBG_MSG("LAN2 IP address is identical to LAN1");
				goto leave;
			}
		}
	}
	// configure gateway and dns (WAN) at bridge mode
	if (!strncmp(opmode, "0", 2)) {
		WebTF(input, "lanGateway", RT2860_NVRAM, "wan_gateway", 1);
		WebTF(input, "lanPriDns", RT2860_NVRAM, "wan_primary_dns", 1);
		WebTF(input, "lanSecDns", RT2860_NVRAM, "wan_secondary_dns", 1);
	}
	if (!strncmp(dhcp_tp, "DISABLE", 8))
		nvram_bufset(RT2860_NVRAM, "dhcpEnabled", "0");
	else if (!strncmp(dhcp_tp, "SERVER", 7)) {
		if (-1 == inet_addr(dhcp_s)) {
			DBG_MSG("invalid DHCP Start IP");
			goto leave;
		}
		nvram_bufset(RT2860_NVRAM, "dhcpStart", dhcp_s);
		if (-1 == inet_addr(dhcp_e)) {
			DBG_MSG("invalid DHCP End IP");
			goto leave;
		}
		nvram_bufset(RT2860_NVRAM, "dhcpEnd", dhcp_e);
		if (-1 == inet_addr(dhcp_m)) {
			DBG_MSG("invalid DHCP Subnet Mask");
			goto leave;
		}
		nvram_bufset(RT2860_NVRAM, "dhcpMask", dhcp_m);
		nvram_bufset(RT2860_NVRAM, "dhcpEnabled", "1");
		WebTF(input, "dhcpPriDns", RT2860_NVRAM, "dhcpPriDns", 1);
		WebTF(input, "dhcpSecDns", RT2860_NVRAM, "dhcpSecDns", 1);
		WebTF(input, "dhcpGateway", RT2860_NVRAM, "dhcpGateway", 1);
		WebTF(input, "dhcpLease", RT2860_NVRAM, "dhcpLease", 1);
		WebTF(input, "dhcpStatic1", RT2860_NVRAM, "dhcpStatic1", 1);
		WebTF(input, "dhcpStatic2", RT2860_NVRAM, "dhcpStatic2", 1);
		WebTF(input, "dhcpStatic3", RT2860_NVRAM, "dhcpStatic3", 1);
	}
	nvram_commit(RT2860_NVRAM);
	do_system("init_system restart");
leave: 
	free_all(7, ip, lan2enabled, lan2_ip, dhcp_tp, dhcp_s, dhcp_e, dhcp_m);
	return;
}

void set_vpnpass(char *input)
{
	WebTF(input, "l2tpPT", RT2860_NVRAM, "l2tpPassThru", 1);
	WebTF(input, "ipsecPT", RT2860_NVRAM, "ipsecPassThru", 1);
	WebTF(input, "pptpPT", RT2860_NVRAM, "pptpPassThru", 1);
	nvram_commit(RT2860_NVRAM);
	do_system("vpn-passthru.sh");
}

static void zebra_restart(void)
{
	const char *opmode, *password, *RIPEnable;

	do_system("killall -q zebra");

	opmode = nvram_bufget(RT2860_NVRAM, "OperationMode");
	password = nvram_bufget(RT2860_NVRAM, "Password");
	RIPEnable = nvram_bufget(RT2860_NVRAM, "RIPEnable");
	if(!opmode||!strlen(opmode))
		return;
	if(!strcmp(opmode, "0"))    // bridge
		return;

	if(!RIPEnable || !strlen(RIPEnable) || !strcmp(RIPEnable,"0"))
		return;

	if(!password || !strlen(password))
		password = "rt2880";

	do_system("echo \"hostname linux.router1\" > /etc/zebra.conf");
	do_system("echo \"password %s\" >> /etc/zebra.conf", password);
	do_system("echo \"enable password rt2880\" >> /etc/zebra.conf");
	do_system("echo \"log syslog\" >> /etc/zebra.conf");
	do_system("zebra -d -f /etc/zebra.conf");
}

static void ripd_restart(void)
{
	char *lan_ip, *wan_ip, *lan_mask, *wan_mask;
	const char *opmode, *password, *RIPEnable;

	do_system("killall -q ripd");
	opmode = nvram_bufget(RT2860_NVRAM, "OperationMode");
	password = nvram_bufget(RT2860_NVRAM, "Password");
	RIPEnable = nvram_bufget(RT2860_NVRAM, "RIPEnable");

	if(!opmode||!strlen(opmode))
		return;
	if(!strcmp(opmode, "0"))    // bridge
		return;
	if(!RIPEnable || !strlen(RIPEnable) || !strcmp(RIPEnable,"0"))
		return;

	if(!password || !strlen(password))
		password = "rt2880";

	do_system("echo \"hostname linux.router1\" > /etc/ripd.conf ");
	do_system("echo \"password %s\" >> /etc/ripd.conf", password);
	do_system("echo \"router rip\" >> /etc/ripd.conf ");

	// deal with WAN
	wan_ip = get_ipaddr(get_wanif_name());
	if(strlen(wan_ip) != 0){
		wan_mask = get_netmask(get_wanif_name());
		if(strlen(wan_mask) != 0) {
			do_system("echo \"network %s/%d\" >> /etc/ripd.conf", wan_ip, netmask_aton(wan_mask));
			do_system("echo \"network %s\" >> /etc/ripd.conf", get_wanif_name());
		}else
			DBG_MSG("The WAN IP is still undeterminated...");
	}else
		DBG_MSG("The WAN IP is still undeterminated...");

	// deal with LAN
	lan_ip = get_ipaddr(get_lanif_name());
	if(strlen(lan_ip) != 0){
		lan_mask = get_netmask(get_lanif_name());
		if(strlen(lan_mask) != 0){
			do_system("echo \"network %s/%d\" >> /etc/ripd.conf", lan_ip, netmask_aton(lan_mask));
			do_system("echo \"network %s\" >> /etc/ripd.conf", get_lanif_name());
		}
	}
	do_system("echo \"version 2\" >> /etc/ripd.conf");
	do_system("echo \"log syslog\" >> /etc/ripd.conf");
	do_system("ripd -f /etc/ripd.conf -d");
}

void set_routing(char *input)
{
	char *rip;
	const char *RIPEnable;

	RIPEnable = nvram_bufget(RT2860_NVRAM, "RIPEnable");
	web_debug_header();
	rip = web_get("RIPSelect", input, 1);
	if(!rip || !strlen(rip))
		return;

	if(!RIPEnable || !strlen(RIPEnable))
		RIPEnable = "0";

	if(!strcmp(rip, "0") && !strcmp(RIPEnable, "0")){
		// nothing changed
	}else if(!strcmp(rip, "1") && !strcmp(RIPEnable, "1")){
		// nothing changed
	}else if(!strcmp(rip, "0") && !strcmp(RIPEnable, "1")){
		nvram_bufset(RT2860_NVRAM, "RIPEnable", rip);
		nvram_commit(RT2860_NVRAM);
		do_system("killall -q ripd");
		do_system("killall -q zebra");
	}else if(!strcmp(rip, "1") && !strcmp(RIPEnable, "0")){
		nvram_bufset(RT2860_NVRAM, "RIPEnable", rip);
		nvram_commit(RT2860_NVRAM);
		zebra_restart();
		ripd_restart();
	}
}

void set_add_routing(char *input)
{
	char *dest, *hostnet, *netmask, *gateway;
	char *interface, *custom_interface, *comment;
	char *true_interface;
	char cmd[CMDLEN] = {0}, result[256] = {0};
	FILE *fp;

	dest = hostnet = netmask = gateway = interface = custom_interface = comment = NULL;
	web_debug_header();
	dest = strdup(web_get("dest", input, 1));
	hostnet = strdup (web_get("hostnet", input, 1));
	netmask = strdup(web_get("netmask", input, 1));
	gateway = strdup(web_get("gateway", input, 1));
	interface = strdup(web_get("interface", input, 1));
	custom_interface = strdup(web_get("custom_interface", input, 1));
	comment = strdup(web_get("comment", input, 1));

	if(!strlen(dest))
		goto leave;

	strcat(cmd, "route add ");

	// host or net?
	if(!strcmp(hostnet, "net"))
		strcat(cmd, "-net ");
	else
		strcat(cmd, "-host ");

	// destination
	strcat(cmd, dest);
	strcat(cmd, " ");

	// netmask
	if(strlen(netmask))
		sprintf(cmd, "%s netmask %s", cmd, netmask);
	else
		strcpy(netmask, "255.255.255.255");

	//gateway
	if(strlen(gateway))
		sprintf(cmd, "%s gw %s", cmd, gateway);
	else
		strcpy(gateway, "0.0.0.0");

	//interface
	if(strlen(interface)){
		if (!strcmp(interface, "WAN")){
			true_interface = get_wanif_name();
		}else if (!strcmp(interface, "Custom")){
			if(!strlen(custom_interface))
				goto leave;
			true_interface = custom_interface;
		}else   // LAN & unknown
			true_interface = get_lanif_name();
	}else{
		strcpy(interface, "LAN");
		true_interface = get_lanif_name();
	}
	sprintf(cmd, "%s dev %s ", cmd, true_interface);

	strcat(cmd, "2>&1 ");

	printf("%s\n", cmd);
	fp = popen(cmd, "r");
	fgets(result, sizeof(result), fp);
	pclose(fp);


	if(!strlen(result)){
		// success, write down to the flash
		char tmp[1024];
		const char *rrs = nvram_bufget(RT2860_NVRAM, "RoutingRules");
		if(!rrs || !strlen(rrs)){
			memset(tmp, 0, sizeof(tmp));
		}else{
			strncpy(tmp, rrs, sizeof(tmp));
		}
		if(strlen(tmp))
			strcat(tmp, ";");
		sprintf(tmp, "%s%s,%s,%s,%s,%s,%s,%s", tmp, dest, netmask, gateway, interface, true_interface, custom_interface, comment);
		nvram_bufset(RT2860_NVRAM, "RoutingRules", tmp);
		nvram_commit(RT2860_NVRAM);
	}else{
		printf("<h1>Add routing failed:<br> %s<h1>", result);
	}
leave:
	free_all(7, dest, hostnet, netmask, gateway, interface, custom_interface, comment);

	return;
}

static void remove_routing_rule(char *dest, char *netmask, char *ifname)
{
	char cmd[CMDLEN];
	strcpy(cmd, "route del ");

	// host or net?
	if(!strcmp(netmask, "255.255.255.255") )
		strcat(cmd, "-host ");
	else
		strcat(cmd, "-net ");

	// destination
	strcat(cmd, dest);
	strcat(cmd, " ");

	// netmask
	if(strcmp(netmask, "255.255.255.255"))
		sprintf(cmd, "%s netmask %s", cmd, netmask);

	//interface
	sprintf(cmd, "%s dev %s ", cmd, ifname);
	system(cmd);
}

void set_delete_routing(char *input)
{
	int index, rule_count;
	char *value, dest[256], netmask[256], true_interface[256];
	char name_buf[16] = {0};
	char *rrs;
	char *new_rrs;
	int *deleArray, j=0;

	rrs = (char *) nvram_bufget(RT2860_NVRAM, "RoutingRules");
	if(!rrs || !strlen(rrs))
		return;

	rule_count = get_nums(rrs, ';');
	if(!rule_count)
		return;

	if(!(deleArray = malloc(sizeof(int) * rule_count) ) )
		return;

	if(! (new_rrs = strdup(rrs))){
		free(deleArray);
		return;
	}

	web_debug_header();
	for(index=0; index< rule_count; index++){
		snprintf(name_buf, sizeof(name_buf), "DR%d", index);
		value = web_get(name_buf, input, 1);
		if(value){
			deleArray[j++] = index;
			if(strlen(value) > 256)
				continue;
			sscanf(value, "%s%s%s", dest, netmask, true_interface);
			remove_routing_rule(dest, netmask, true_interface);
			printf("Delete entry: %s,%s,%s<br>\n", dest, netmask, true_interface);
		}
	}

	if(j>0){
		delete_nth_value(deleArray, j, new_rrs, ';');
		nvram_bufset(RT2860_NVRAM, "RoutingRules", new_rrs);
		nvram_commit(RT2860_NVRAM);
	}
	free(deleArray);
	free(new_rrs);
}

#if defined CONFIG_IPV6
void config_ipv6(int mode)
{
	const char *wan_v6addr = nvram_bufget(RT2860_NVRAM, "IPv6WANIPAddr");
#if defined CONFIG_IPV6_SIT_6RD || defined CONFIG_IPV6_TUNNEL
	const char *srv_v6addr = nvram_bufget(RT2860_NVRAM, "IPv6SrvAddr");
#endif
	int prefix_len = strtol(nvram_bufget(RT2860_NVRAM, "IPv6PrefixLen"), NULL, 10);
	int wan_prefix_len = strtol(nvram_bufget(RT2860_NVRAM, "IPv6WANPrefixLen"), NULL, 10);
	const char *gw_v6addr = nvram_bufget(RT2860_NVRAM, "IPv6GWAddr");
	char *wan_if = get_wanif_name();
	char *lan_if = get_lanif_name();
	char v6addr[40];
#if defined CONFIG_IPV6_SIT_6RD
	char wan_addr[16];
	char ipv6_ip_addr[20];
	unsigned short temp[8];
	int i, used, shift;
	char *tok = NULL;
#endif

	strcpy(v6addr, nvram_bufget(RT2860_NVRAM, "IPv6IPAddr"));
#if defined CONFIG_IPV6_SIT_6RD
	do_system("ip link set 6rdtun down");
#endif
	do_system("echo 0 > /proc/sys/net/ipv6/conf/all/forwarding");

	switch (mode) {
	case 1:
		do_system("ifconfig %s add  %s/%d", lan_if, v6addr, prefix_len);
		do_system("ifconfig %s add  %s/%d", wan_if, wan_v6addr, wan_prefix_len);
		do_system("route -A inet6 add default gw %s dev %s", gw_v6addr, wan_if);
		do_system("echo 1 > /proc/sys/net/ipv6/conf/all/forwarding");
		// do_system("ecmh");
		break;
#if defined CONFIG_IPV6_SIT_6RD
	case 2:
		if (get_ifip(get_wanif_name(), wan_addr) < 0) {
			DBG_MSG("Can't Query WAN IPv4 Address!");
			return;
		}
		memset(temp, 0, sizeof(temp));
		do_system("ip tunnel add 6rdtun mode sit local %s ttl 64", wan_addr);
		for (i=0, tok = strtok(v6addr, ":"); tok; i++, tok = strtok(NULL, ":"))
			temp[i] = strtol(tok, NULL, 16);
		if ((shift = 16 - (prefix_len % 16)) < 16) {
			temp[i-1] = (temp[i-1] >> shift) << shift;
		}
		sprintf(v6addr, "%x", temp[0]);
		for (used=1; used<i; used++)
			sprintf(v6addr, "%s:%x", v6addr, temp[used]);
		for (tok = strtok(wan_addr, "."); tok; i++, tok = strtok(NULL, ".")) {
			temp[i] = strtol(tok, NULL, 10)<<8;
			tok = strtok(NULL, ".");
			temp[i] += strtol(tok, NULL, 10);
		}
		if (shift < 16) {
			used = prefix_len / 16;
			while (used < i) {
				temp[used] = (temp[used] >> shift) << shift;
				temp[used] += temp[used+1]>>(16-shift);
				temp[used+1] <<= shift;
				used++;
			}
		} else {
			used = i;
		}
		sprintf(ipv6_ip_addr, "%x", temp[0]);
		for (i=1; i<used; i++) {
			sprintf(ipv6_ip_addr, "%s:%x", ipv6_ip_addr, temp[i]);
		}
		do_system("ip tunnel 6rd dev 6rdtun 6rd-prefix %s::/%d", v6addr, prefix_len);
		do_system("ip addr add %s::1/%d dev 6rdtun", ipv6_ip_addr, prefix_len);
		do_system("ip link set 6rdtun up");
		do_system("ip route add ::/0 via ::%s dev 6rdtun", srv_v6addr);
		do_system("ip addr add %s::1/64 dev %s", ipv6_ip_addr, lan_if);
		do_system("echo 1 > /proc/sys/net/ipv6/conf/all/forwarding");
		do_system("radvd.sh %s", ipv6_ip_addr);
		break;
#endif
#if defined CONFIG_IPV6_TUNNEL
	case 3:
		do_system("config-dslite.sh %s %s %s", srv_v6addr, v6addr, gw_v6addr);
		break;
#endif
	default:
		break;
	}
	return;
}
#endif

#if defined CONFIG_IPV6
void set_ipv6(char *input)
{
	char *v6_opmode;

	v6_opmode = NULL;
	web_debug_header();
	v6_opmode = strdup(web_get("ipv6_opmode", input, 1));
	nvram_bufset(RT2860_NVRAM, "IPv6OpMode", v6_opmode);
	if (!strcmp(v6_opmode, "1")) {
		WebTF(input, "ipv6_lan_ipaddr", RT2860_NVRAM, "IPv6IPAddr", 1);
		WebTF(input, "ipv6_lan_prefix_len", RT2860_NVRAM, "IPv6PrefixLen", 1);
		WebTF(input, "ipv6_wan_ipaddr", RT2860_NVRAM, "IPv6WANIPAddr", 1);
		WebTF(input, "ipv6_wan_prefix_len", RT2860_NVRAM, "IPv6WANPrefixLen", 1);
		WebTF(input, "ipv6_static_gw", RT2860_NVRAM, "IPv6GWAddr", 1);
#if defined CONFIG_IPV6_SIT_6RD
	} else if (!strcmp(v6_opmode, "2")) {
		WebTF(input, "ipv6_6rd_prefix", RT2860_NVRAM, "IPv6IPAddr", 1);
		WebTF(input, "ipv6_6rd_prefix_len", RT2860_NVRAM, "IPv6PrefixLen", 1);
		WebTF(input, "ipv6_6rd_border_ipaddr", RT2860_NVRAM, "IPv6SrvAddr", 1);
		nvram_bufset(RT2860_NVRAM, "radvdEnabled", "1");
#endif
#if defined CONFIG_IPV6_TUNNEL
	} else if (!strcmp(v6_opmode, "3")) {
		WebTF(input, "ipv6_ds_wan_ipaddr", RT2860_NVRAM, "IPv6IPAddr", 1);
		WebTF(input, "ipv6_ds_aftr_ipaddr", RT2860_NVRAM, "IPv6SrvAddr" ,1);
		WebTF(input, "ipv6_ds_gw_ipaddr", RT2860_NVRAM, "IPv6GWAddr", 1);
#endif
	} else {
		return;
	}
	nvram_commit(RT2860_NVRAM);
	free(v6_opmode);
	do_system("init_system restart");
}
#endif

void static_routing_init(void)
{
	int index=0;
	char one_rule[256];
	char *rrs;
	struct in_addr dest_s, gw_s, netmask_s;
	char dest[32], netmask[32], gw[32], interface[32], true_interface[32], custom_interface[32], comment[32];
	int flgs, ref, use, metric, nl=0;
	unsigned long int d,g,m;
	int isGatewayMode = (!strcmp("1", nvram_bufget(RT2860_NVRAM, "OperationMode"))) ? 1 : 0 ;

	// delete old user rules
	FILE *fp = fopen("/proc/net/route", "r");
	if(!fp)
		return;

	rrs = (char *) nvram_bufget(RT2860_NVRAM, "RoutingRules");
	if(!rrs|| !strlen(rrs))
		return;
	while (fgets(one_rule, sizeof(one_rule), fp) != NULL) {
		if (nl) {
			if (sscanf(one_rule, "%s%lx%lx%X%d%d%d%lx",
						interface, &d, &g, &flgs, &ref, &use, &metric, &m) != 8) {
				DBG_MSG("format error\n");
				fclose(fp);
				return;
			}
			dest_s.s_addr = d;
			gw_s.s_addr = g;
			netmask_s.s_addr = m;

			strncpy(dest, inet_ntoa(dest_s), sizeof(dest));
			strncpy(gw, inet_ntoa(gw_s), sizeof(gw));
			strncpy(netmask, inet_ntoa(netmask_s), sizeof(netmask));

			// check if internal routing rules
			if( (index=get_index_routingrule(rrs, dest, netmask, interface)) != -1){
				remove_routing_rule(dest, netmask, interface);
			}
		}
		nl++;
	}
	fclose(fp);

	index = 0;

	while( get_nth_value(index, rrs, ';', one_rule, 256) != -1 ){
		char cmd[1024];

		if((get_nth_value(0, one_rule, ',', dest, sizeof(dest)) == -1)){
			index++;
			continue;
		}
		if((get_nth_value(1, one_rule, ',', netmask, sizeof(netmask)) == -1)){
			index++;
			continue;
		}
		if((get_nth_value(2, one_rule, ',', gw, sizeof(gw)) == -1)){
			index++;
			continue;
		}
		if((get_nth_value(3, one_rule, ',', interface, sizeof(interface)) == -1)){
			index++;
			continue;
		}
		if((get_nth_value(4, one_rule, ',', true_interface, sizeof(true_interface)) == -1)){
			index++;
			continue;
		}
		if((get_nth_value(5, one_rule, ',', custom_interface, sizeof(custom_interface)) == -1)){
			index++;
			continue;
		}
		if((get_nth_value(6, one_rule, ',', comment, sizeof(comment)) == -1)){
			index++;
			continue;
		}

		strcpy(cmd, "route add ");

		// host or net?
		if(!strcmp(netmask, "255.255.255.255") )
			strcat(cmd, "-host ");
		else
			strcat(cmd, "-net ");

		// destination
		strcat(cmd, dest);
		strcat(cmd, " ");

		// netmask
		if(strcmp(netmask, "255.255.255.255") )
			sprintf(cmd, "%s netmask %s", cmd, netmask);

		// gateway
		if(strlen(gw) && strcmp(gw, "0.0.0.0"))
			sprintf(cmd, "%s gw %s", cmd, gw);

#if 0
		//interface
		if (!strcmp(interface, "WAN")){
			true_interface = getWanIfName();
		}else if (!gstrcmp(interface, "Custom")){
			true_interface = custom_interface;
		}else   // LAN & unknown
			true_interface = getLanIfName();
#endif
		sprintf(cmd, "%s dev %s ", cmd, true_interface);

		strcat(cmd, "2>&1 ");

		if(strcmp(interface, "WAN") || (!strcmp(interface, "WAN") && isGatewayMode)  ){
			do_system(cmd);
		}else{
			DBG_MSG("Skip WAN routing rule in the non-Gateway mode: %s\n", cmd);
		}

		index++;
	}
	return;
}

int main(int argc, char *argv[]) 
{
	char *form, *inStr;
	long inLen;

	nvram_init(RT2860_NVRAM);
	if ((argc > 1) && (!strcmp(argv[1], "init"))) {
		static_routing_init();
		zebra_restart();
		ripd_restart();
#if defined CONFIG_IPV6
		config_ipv6(strtol(nvram_bufget(RT2860_NVRAM, "IPv6OpMode"), NULL, 10));
#endif
		goto leave;
	} else if ((argc > 1) && (!strcmp(argv[1], "ripd"))) {
		ripd_restart();
		goto leave;
	} else if ((argc > 1) && (!strcmp(argv[1], "ipv6"))) {
#if defined CONFIG_IPV6
		config_ipv6(strtol(nvram_bufget(RT2860_NVRAM, "IPv6OpMode"), NULL, 10));
#endif
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
	if (!strcmp(form, "wan")) {
		set_wan(inStr);
	} else if (!strcmp(form, "lan")) {
		set_lan(inStr);
	} else if (!strcmp(form, "vpnpass")) {
		set_vpnpass(inStr);
	} else if (!strcmp(form, "routing")) {
		set_routing(inStr);
	} else if (!strcmp(form, "addrouting")) {
		set_add_routing(inStr);
	} else if (!strcmp(form, "delrouting")) {
		set_delete_routing(inStr);
#if defined CONFIG_IPV6
	} else if (!strcmp(form, "ipv6")) {
		set_ipv6(inStr);
#endif
	}
	free(inStr);
leave:
	printf("webs: Listening for HTTP requests at address %s", nvram_get(RT2860_NVRAM, "lan_ipaddr"));	
	nvram_close(RT2860_NVRAM);

	return 0;
}
