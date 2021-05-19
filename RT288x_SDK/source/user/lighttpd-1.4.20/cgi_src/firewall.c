#include "utils.h"
#include <stdlib.h>
#include <arpa/inet.h>

#include "user_conf.h"
#include "firewall.h"

static char cmd[CMDLEN];

static char *SuperDMZ_MakeSinglePortForwardAvoidStr(void)
{
	int i=0;
	char rec[256];

	int publicPort_int, proto;
	char publicPort[8], protocol[8];

	char *enable, *rule;

	static char result[1024];
	memset(result, 0, sizeof(result));

	enable = (char *)nvram_bufget(RT2860_NVRAM, "SinglePortForwardEnable");
	if(!enable)
		return NULL;
	if(atoi(enable)){
		rule = (char *)nvram_bufget(RT2860_NVRAM, "SinglePortForwardRules");
		if(!rule)
			return NULL;
	}else
		return NULL;

	while( (get_nth_value(i++, rule, ';', rec, sizeof(rec)) != -1) ){
		// get public port
		if((get_nth_value(1, rec, ',', publicPort, sizeof(publicPort)) == -1)){
			continue;
		}
		if( (publicPort_int = atoi(publicPort)) == 0 || publicPort_int > 65535)
			continue;

		// get protocol
		if((get_nth_value(3, rec, ',', protocol, sizeof(protocol)) == -1))
			continue;
		proto = atoi(protocol);

		if(proto == PROTO_TCP){
			strncat(result, "-t ", sizeof(result));
			strncat(result, publicPort, sizeof(result));
			strncat(result, " ", sizeof(result));
		}else if(proto == PROTO_UDP){
			strncat(result, "-u ", sizeof(result));
			strncat(result, publicPort, sizeof(result));
			strncat(result, " ", sizeof(result));
		}else if(proto == PROTO_TCP_UDP){
			strncat(result, "-t ", sizeof(result));
			strncat(result, publicPort, sizeof(result));
			strncat(result, " -u ", sizeof(result));
			strncat(result, publicPort, sizeof(result));
			strncat(result, " ", sizeof(result));
		}else
			continue;
	}
	return result;
}

static char *SuperDMZ_MakePortForwardAvoidStr(void)
{
	int i=0;
	char rec[256];

	static char result[1024];

	int prf_int, prt_int, proto;
	char prf[8], prt[8], protocol[8];
	char *enable, *rule;

	memset(result, 0, sizeof(result));

	enable = (char *)nvram_bufget(RT2860_NVRAM, "PortForwardEnable");
	if(!enable){
		return NULL;
	}
	if(atoi(enable)){
		rule = (char *)nvram_bufget(RT2860_NVRAM, "PortForwardRules");
		if(!rule){
			return NULL;
		}
	}else
		return NULL;

	while( (get_nth_value(i++, rule, ';', rec, sizeof(rec)) != -1) ){
		// get protocol
		if((get_nth_value(3, rec, ',', protocol, sizeof(protocol)) == -1))
			continue;
		proto = atoi(protocol);

		// get port range "from"
		if((get_nth_value(1, rec, ',', prf, sizeof(prf)) == -1)){
			continue;
		}
		if( (prf_int = atoi(prf)) == 0 || prf_int > 65535)
			continue;

		// get port range "to"
		if((get_nth_value(2, rec, ',', prt, sizeof(prt)) == -1)){
			continue;
		}
		if( (prt_int = atoi(prt)) > 65535)
			continue;

#define MAKE_COMMAND								\
		strncat(result, prf, sizeof(result));		\
		if(prt_int){								\
			strncat(result, ":", sizeof(result));	\
			strncat(result, prt, sizeof(result));	\
		}strncat(result, " ", sizeof(result));

		if(proto == PROTO_TCP){
			strncat(result, "-t ", sizeof(result));
			MAKE_COMMAND;
		}else if(proto == PROTO_UDP){
			strncat(result, "-u ", sizeof(result));
			MAKE_COMMAND;
		}else if(proto == PROTO_TCP_UDP){
			strncat(result, "-t ", sizeof(result));
			MAKE_COMMAND;
			strncat(result, "-u ", sizeof(result));
			MAKE_COMMAND;
		}else
			continue;
	}
	return result;
}

inline int getRuleNums(char *rules)
{
	return get_nums(rules, ';');
}
static void iptablesMaliciousFilterRun(void)
{
	char *wpf = (char *)nvram_bufget(RT2860_NVRAM, "WANPingFilter");
	char *bps = (char *)nvram_bufget(RT2860_NVRAM, "BlockPortScan");
	char *bsf = (char *)nvram_bufget(RT2860_NVRAM, "BlockSynFlood");
	if(!wpf || !bps || !bsf)
		return;

	if(!atoi(wpf)){
		;// do nothing
	}else{
		do_system("iptables -A %s -i %s -p icmp -j DROP", MALICIOUS_INPUT_FILTER_CHAIN, get_wanif_name());
	}

	if(!atoi(bps)){
		;//do nothing
	}else{

		/*
		 *  Port scan rules
		 */
		// nmap- Xmas
		do_system("iptables -A %s -p tcp --tcp-flags ALL FIN,URG,PSH -j DROP", MALICIOUS_FILTER_CHAIN);
		// nmap- PSH
		do_system("iptables -A %s -p tcp --tcp-flags ALL SYN,RST,ACK,FIN,URG -j DROP", MALICIOUS_FILTER_CHAIN);
		// Null
		do_system("iptables -A %s -p tcp --tcp-flags ALL NONE -j DROP", MALICIOUS_FILTER_CHAIN);
		// SYN/RST
		do_system("iptables -A %s -p tcp --tcp-flags SYN,RST SYN,RST -j DROP", MALICIOUS_FILTER_CHAIN);
		// SYN/FIN
		do_system("iptables -A %s -p tcp --tcp-flags SYN,FIN SYN,FIN ", MALICIOUS_FILTER_CHAIN);

		do_system("iptables -A %s -p tcp --tcp-flags ALL FIN,URG,PSH -j DROP", MALICIOUS_INPUT_FILTER_CHAIN);
		do_system("iptables -A %s -p tcp --tcp-flags ALL SYN,RST,ACK,FIN,URG -j DROP", MALICIOUS_INPUT_FILTER_CHAIN);
		do_system("iptables -A %s -p tcp --tcp-flags ALL NONE -j DROP", MALICIOUS_INPUT_FILTER_CHAIN);
		do_system("iptables -A %s -p tcp --tcp-flags SYN,RST SYN,RST -j DROP", MALICIOUS_INPUT_FILTER_CHAIN);
		do_system("iptables -A %s -p tcp --tcp-flags SYN,FIN SYN,FIN ", MALICIOUS_INPUT_FILTER_CHAIN);
	}

	if(!atoi(bsf)){
		;// do nothing
	}else{
		/*
		 * SYN flooding fules
		 */
		 //do_system("iptables -A %s -m limit --limit 1/s --limit-burst 10 -j RETURN", SYNFLOOD_FILTER_CHAIN);
		 //do_system("iptables -A %s -j DROP", SYNFLOOD_FILTER_CHAIN);
		 //do_system("iptables -A %s -m limit --limit 1/s --limit-burst 10 -j RETURN", SYNFLOOD_INPUT_FILTER_CHAIN);
		 //do_system("iptables -A %s -j DROP", SYNFLOOD_INPUT_FILTER_CHAIN);
		 do_system("echo 1 > /proc/sys/net/ipv4/tcp_syncookies 2>&1 1>/dev/null");
	}
	return;
}
static void iptablesRemoteManagementRun(void)
{
	char *rmE = (char *)nvram_bufget(RT2860_NVRAM, "RemoteManagement");
	char *opmode = (char *)nvram_bufget(RT2860_NVRAM, "OperationMode");
	char *spifw = (char *)nvram_bufget(RT2860_NVRAM, "SPIFWEnabled");
	if(!opmode)
		return;

	// "Gateway mode" only
	if(strcmp(opmode , "1"))
		return;

	if(rmE && atoi(rmE) == 1)
		return;

	if (atoi(spifw) == 0){
		;//do_system("iptables -A %s -i %s -j ACCEPT", get_wanif_name());
	}
	else{
		do_system("iptables -A %s -i %s -m state --state RELATED,ESTABLISHED -j ACCEPT", MALICIOUS_INPUT_FILTER_CHAIN, get_wanif_name());
	}
	if(get_one_port()){

		// make the web server to be reachable.
		if (atoi(spifw) == 0){
			do_system("iptables -A %s -i %s -d 172.32.1.254 -p tcp --dport 80 -j ACCEPT", MALICIOUS_INPUT_FILTER_CHAIN, get_wanif_name());
		}
		else{
			do_system("iptables -A %s -i %s -m state -d 172.32.1.254 -p tcp --dport 80 --state NEW,INVALID -j ACCEPT", MALICIOUS_INPUT_FILTER_CHAIN, get_wanif_name());
		}
	}
	if (atoi(spifw) == 0){
		do_system("iptables -A %s -i %s -p tcp --dport 80 -j DROP", MALICIOUS_INPUT_FILTER_CHAIN , get_wanif_name());
	}
	else{
		do_system("iptables -A %s -i %s -m state -p tcp --dport 80 --state NEW,INVALID -j DROP", MALICIOUS_INPUT_FILTER_CHAIN, get_wanif_name());
	}
	return;
}

static void makeIPPortFilterRule(char *buf, int len, char *mac_address,
char *sip_1, char *sip_2, int sprf_int, int sprt_int, 
char *dip_1, char *dip_2, int dprf_int, int dprt_int, int proto, int action)
{
		int rc = 0;
		char *pos = buf;
    char *spifw = (char *)nvram_bufget(RT2860_NVRAM, "SPIFWEnabled");

		switch(action){
		case ACTION_DROP:
		    if (atoi(spifw) == 0)
			rc = snprintf(pos, len-rc, 
				"iptables -A %s ", IPPORT_FILTER_CHAIN);
		    else
			rc = snprintf(pos, len-rc, 
				"iptables -A %s -m state --state NEW,INVALID ", IPPORT_FILTER_CHAIN);
			break;
		case ACTION_ACCEPT:
			rc = snprintf(pos, len-rc, 
				"iptables -A %s ", IPPORT_FILTER_CHAIN);
			break;
		}
		pos = pos + rc;

		// write mac address
		if(mac_address && strlen(mac_address)){
			rc = snprintf(pos, len-rc, "-m mac --mac-source %s ", mac_address);
			pos = pos+rc;
		}

		// write source ip
		rc = snprintf(pos, len-rc, "-s %s ", sip_1);
		pos = pos+rc;
		
		// write dest ip
		rc = snprintf(pos, len-rc, "-d %s ", dip_1);
		pos = pos+rc;

		// write protocol type
		if(proto == PROTO_NONE){
			rc = snprintf(pos, len-rc, " ");
			pos = pos + rc;
		}else if(proto == PROTO_ICMP){
			rc = snprintf(pos, len-rc, "-p icmp ");
			pos = pos + rc;
		}else{
			if(proto == PROTO_TCP)
				rc = snprintf(pos, len-rc, "-p tcp ");
			else if (proto == PROTO_UDP)
				rc = snprintf(pos, len-rc, "-p udp ");
			pos = pos + rc;

			// write source port
			if(sprf_int){
				if(sprt_int)
					rc = snprintf(pos, len-rc, "--sport %d:%d ", sprf_int, sprt_int);
				else
					rc = snprintf(pos, len-rc, "--sport %d ", sprf_int);
				pos = pos+rc;
			}

			// write dest port
			if(dprf_int){
				if(dprt_int)
					rc = snprintf(pos, len-rc, "--dport %d:%d ", dprf_int, dprt_int);
				else
					rc = snprintf(pos, len-rc, "--dport %d ", dprf_int);
				pos = pos+rc;
			}
		}

		switch(action){
		case ACTION_DROP:			// 1 == ENABLE--DROP mode
			rc = snprintf(pos, len-rc, "-j DROP");
			break;
		case ACTION_ACCEPT:			// 2 == ENABLE--ACCEPT mode
			rc = snprintf(pos, len-rc, "-j ACCEPT");
			break;
		}
}

static void iptablesIPPortFilterRun(void)
{
	int i=0;
	char rec[256];
	int sprf_int, sprt_int, proto, action;
	int dprf_int, dprt_int;
	char sprf[8], sprt[8], protocol[8];
	char dprf[8], dprt[8];
	char mac_address[32];
	char sip_1[32], sip_2[32], action_str[4];
	char dip_1[32], dip_2[32];
  char *firewall_enable, *default_policy, *rule;
  char *spifw = (char *)nvram_bufget(RT2860_NVRAM, "SPIFWEnabled");
  int mode;
	
    firewall_enable = (char *)nvram_bufget(RT2860_NVRAM, "IPPortFilterEnable");
    if(!firewall_enable){
        DBG_MSG("Warning: can't find \"IPPortFilterEnable\" in flash.");
        return;
    }
    mode = atoi(firewall_enable);
    if(!mode)
		return;

	rule = (char *)nvram_bufget(RT2860_NVRAM, "IPPortFilterRules");
	if(!rule){
		DBG_MSG("Warning: can't find \"IPPortFilterRules\" in flash.");
		return;
	}

	default_policy = (char *)nvram_bufget(RT2860_NVRAM, "DefaultFirewallPolicy");
	// add the default policy to the end of FORWARD chain
	if(!default_policy)
		default_policy = "0";

	if(atoi(default_policy) == 1){
		//the default policy is drop
		if (atoi(spifw) == 0){
			;//do_system("iptables -t filter -A %s -j ACCEPT", IPPORT_FILTER_CHAIN);
		}
		else{
			do_system("iptables -t filter -A %s -m state --state RELATED,ESTABLISHED -j ACCEPT", IPPORT_FILTER_CHAIN);
		}
	}

	while( (get_nth_value(i++, rule, ';', rec, sizeof(rec)) != -1) ){
        // get sip 1

    if((get_nth_value(0, rec, ',', sip_1, sizeof(sip_1)) == -1)){
			continue;
		}
		if(!is_ipnetmask_valid(sip_1)){
			continue;
		}

		// we dont support ip range yet.
        // get sip 2
        //if((get_nth_value(1, rec, ',', sip_2, sizeof(sip_2)) == -1))
        //	continue;
		//if(!is_ip_valid(sip_2))
		//	continue;

		// get source port range "from"
		if((get_nth_value(2, rec, ',', sprf, sizeof(sprf)) == -1)){
			continue;
		}

		if( (sprf_int = atoi(sprf)) > 65535)
			continue;
		// get dest port range "to"
		if((get_nth_value(3, rec, ',', sprt, sizeof(sprt)) == -1)){
			continue;
		}

		if( (sprt_int = atoi(sprt)) > 65535)
			continue;

		// Destination Part
        // get dip 1
		if((get_nth_value(4, rec, ',', dip_1, sizeof(dip_1)) == -1)){
			continue;
		}

		if(!is_ipnetmask_valid(dip_1)){
			continue;
		}

		// we dont support ip range yet
        // get sip 2
        //if((get_nth_value(5, rec, ',', dip_2, sizeof(dip_2)) == -1))
        //    continue;
        //if(!is_ip_valid(dip_2))
        //    continue;

		// get source port range "from"
		if((get_nth_value(6, rec, ',', dprf, sizeof(dprf)) == -1)){
			continue;
		}

		if( (dprf_int = atoi(dprf)) > 65535)
			continue;

		// get dest port range "to"
		if((get_nth_value(7, rec, ',', dprt, sizeof(dprt)) == -1)){
			continue;
		}

		if( (dprt_int = atoi(dprt)) > 65535)
			continue;


		// get protocol
		if((get_nth_value(8, rec, ',', protocol, sizeof(protocol)) == -1))
			continue;
		proto = atoi(protocol);

		// get action
        if((get_nth_value(9, rec, ',', action_str, sizeof(action_str)) == -1)){
            continue;
        }
        action = atoi(action_str);

        // get_nth_value(10) is "comment".

        // get mac address
        if((get_nth_value(11, rec, ',', mac_address, sizeof(mac_address)) == -1))
            continue;
		if(strlen(mac_address)){
	        if(!is_mac_valid(mac_address))
	        	continue;
		}

        //TODO:
		// supposed to do validation here but  we didn't do it because code size.
/*
# iptables example
# iptables -t nat -A POSTROUTING -o eth0  -s 10.10.10.0/24 -j MASQUERADE
# iptables -A FORWARD -m physdev --physdev-in ra0 --physdev-out eth2 -m state --state ESTABLISHED,RELATED -j ACCEPT
# iptables -A FORWARD -m physdev --physdev-in eth0 --physdev-out eth2 -j DROP
# iptables -A FORWARD -i eth0 -o eth2 -j DROP
# iptables -A FORWARD -p tcp --dport 139 -j DROP
# iptables -A FORWARD -i eth0 -o eth2 -m state --state NEW,INVALID -p tcp --dport 80 -j DROP
*/
		makeIPPortFilterRule(cmd, sizeof(cmd), mac_address, sip_1, sip_2, sprf_int, sprt_int, dip_1, dip_2, dprf_int, dprt_int, proto, action);
		do_system(cmd);
	}


	switch(atoi(default_policy)){
	case 0:
		do_system("iptables -t filter -A %s -j ACCEPT", IPPORT_FILTER_CHAIN);
		break;
	case 1:
		do_system("iptables -t filter -A %s -j DROP", IPPORT_FILTER_CHAIN);
		break;
	}

}
static void iptablesPortTriggerFlush(void)
{
	do_system("iptables -t filter -F %s 1>/dev/null 2>&1", PORT_TRIGGER_CHAIN);
	do_system("iptables -t nat -F %s 1>/dev/null 2>&1", PORT_TRIGGER_PREROUTING_CHAIN);
}
static void iptablesIPPortFilterFlush(void)
{
	do_system("iptables -F %s 1>/dev/null 2>&1", IPPORT_FILTER_CHAIN);
}

static void iptablesBasicSetting(void)
{
	char *firewall_enable;

	firewall_enable = (char *)nvram_bufget(RT2860_NVRAM, "IPPortFilterEnable");

	// flush  ipport   filter   chain
    iptablesIPPortFilterFlush();

	if(!firewall_enable || !atoi(firewall_enable))
	return;
	
    iptablesIPPortFilterRun();

	return;
}
static void ipportFilter(char *input)
{
	char rule[8192];
	char *mac_address;
	char *sip_1, *sip_2, *sprf, *sprt, *protocol, *action_str, *comment;
	char *dip_1, *dip_2, *dprf, *dprt;
	char *IPPortFilterRules;
	
	int sprf_int, sprt_int, dprf_int, dprt_int, proto, action;

	mac_address = sip_1 = sip_2 = sprf = sprt = protocol = NULL;
	action_str = comment = dip_1 = dip_2 = dprf = dprt = NULL;
	web_debug_header();
	mac_address = strdup(web_get("mac_address", input, 1));
	sip_1 = strdup(web_get("sip_address", input, 1));
	if(sip_1==NULL) strcpy(sip_1, "any");
	sip_2 = strdup(web_get("sip_address2", input, 1));
	sprf = strdup(web_get("sFromPort", input, 1));
	sprt = strdup(web_get("sToPort", input, 1));
	dip_1 = strdup(web_get("dip_address", input, 1));
	if(dip_1==NULL) strcpy(dip_1, "any");
	dip_2 = strdup(web_get("dip_address2", input, 1));
	dprf = strdup(web_get("dFromPort", input, 1));
	dprt = strdup(web_get("sToPort", input, 1));
	protocol = strdup(web_get("protocol", input, 1));
	action_str = strdup(web_get("action", input, 1));
	comment = strdup(web_get("comment", input, 1));

/*	
	sip_1 = websGetVar(wp, T("sip_address"), T("any"));
	sip_2 = websGetVar(wp, T("sip_address2"), T(""));
	sprf = websGetVar(wp, T("sFromPort"), T("0"));
	sprt = websGetVar(wp, T("sToPort"), T(""));

	dip_1 = websGetVar(wp, T("dip_address"), T("any"));
	dip_2 = websGetVar(wp, T("dip_address2"), T(""));
	dprf = websGetVar(wp, T("dFromPort"), T("0"));
	dprt = websGetVar(wp, T("dToPort"), T(""));

	protocol = websGetVar(wp, T("protocol"), T(""));
	action_str = websGetVar(wp, T("action"), T(""));
	comment = websGetVar(wp, T("comment"), T(""));
*/
	if(!mac_address || !sip_1 || !dip_1 || !sprf || !dprf)
		goto leave;

	if(!strlen(mac_address) && !strlen(sip_1) && !strlen(dip_1) && !strlen(sprf) && !strlen(dprf))
		goto leave;

	// we dont trust user input.....
	if(strlen(mac_address)){
		if(!is_mac_valid(mac_address))
			goto leave;
	}

	if(strlen(sip_1)){
		if(!is_ipnetmask_valid(sip_1))
			goto leave;
	}else
		strcpy(sip_1, "any/0");

	if(strlen(dip_1)){
		if(!is_ipnetmask_valid(dip_1))
			goto leave;
	}else
    	strcpy(dip_1, "any/0");

	strcpy(sip_2, "0"); 
	strcpy(dip_2, "0");

	if(! strcmp(protocol, "TCP"))
		proto = PROTO_TCP;
	else if( !strcmp(protocol, "UDP"))
		proto = PROTO_UDP;
	else if( !strcmp(protocol, "None"))
		proto = PROTO_NONE;
	else if( !strcmp(protocol, "ICMP"))
		proto = PROTO_ICMP;
	else
		goto leave;

	if(!strlen(sprf) || proto == PROTO_NONE || proto == PROTO_ICMP){
		sprf_int = 0;
	}else{
		sprf_int = atoi(sprf);
		if(sprf_int == 0 || sprf_int > 65535)
			goto leave;
	}

	if(!strlen(sprt) || proto == PROTO_NONE || proto == PROTO_ICMP){
		sprt_int = 0;
	}else{
		sprt_int = atoi(sprt);
		if(sprt_int ==0 || sprt_int > 65535)
			goto leave;
	}

	if(!strlen(dprf) || proto == PROTO_NONE || proto == PROTO_ICMP){
		dprf_int = 0;
	}else{
		dprf_int = atoi(dprf);
		if(dprf_int ==0 || dprf_int > 65535)
			goto leave;
	}

	if(!strlen(dprt) || proto == PROTO_NONE || proto == PROTO_ICMP){
		dprt_int = 0;
	}else{
		dprt_int = atoi(dprt);
		if(dprt_int ==0 || dprt_int > 65535)
			goto leave;
	}

	if(! (strcmp(action_str, "Accept")))
		action = ACTION_ACCEPT;
	else if(! (strcmp(action_str, "Drop")))
		action = ACTION_DROP;
	else
		goto leave;

	if(strlen(comment) > 32)
		goto leave;
	// i know you will try to break our box... ;) 
	if(strchr(comment, ';') || strchr(comment, ','))
		goto leave;

	if(   ( IPPortFilterRules = (char *)nvram_bufget(RT2860_NVRAM, "IPPortFilterRules")) && strlen(IPPortFilterRules)){
		snprintf(rule, sizeof(rule), "%s;%s,%s,%d,%d,%s,%s,%d,%d,%d,%d,%s,%s", IPPortFilterRules, sip_1, sip_2, sprf_int, sprt_int, dip_1, dip_2, dprf_int, dprt_int, proto, action, comment, mac_address);
	}else{
		snprintf(rule, sizeof(rule), "%s,%s,%d,%d,%s,%s,%d,%d,%d,%d,%s,%s", sip_1, sip_2, sprf_int, sprt_int, dip_1, dip_2, dprf_int, dprt_int, proto, action, comment, mac_address);
	}

	nvram_set(RT2860_NVRAM, "IPPortFilterRules", rule);
	nvram_commit(RT2860_NVRAM);

	iptablesIPPortFilterFlush();
	iptablesIPPortFilterRun();

leave:
	free_all(12, mac_address, sip_1, sip_2, sprf, sprt, protocol,
		     action_str, comment, dip_1, dip_2, dprf, dprt);
    return;
	
}

static void BasicSettings(char *input)
{
	char *default_policy, *firewall_enable;

	web_debug_header();
	firewall_enable = default_policy = NULL;
	firewall_enable = strdup(web_get("portFilterEnabled", input, 1));
	default_policy = strdup(web_get("defaultFirewallPolicy", input, 1));

	switch(atoi(firewall_enable)){
	case 0:
		nvram_set(RT2860_NVRAM, "IPPortFilterEnable", "0");
		break;
	case 1:
		nvram_set(RT2860_NVRAM, "IPPortFilterEnable", "1");
		break;
	}

	switch(atoi(default_policy)){
	case 1:
		nvram_set(RT2860_NVRAM, "DefaultFirewallPolicy", "1");
		break;
	case 0:
	default:
		nvram_set(RT2860_NVRAM, "DefaultFirewallPolicy", "0");
		break;
	}
	nvram_commit(RT2860_NVRAM);

	iptablesBasicSetting();
/*
	websHeader(wp);
	websWrite(wp, T("default_policy: %s<br>\n"), default_policy);
    websFooter(wp);
    websDone(wp, 200);
    */
	free_all(2, default_policy, firewall_enable);

	return;
}

static void iptablesPortForwardFlush(void)
{
	do_system("iptables -t nat -F %s 1>/dev/null 2>&1", PORT_FORWARD_CHAIN);
}
static void iptablesSinglePortForwardFlush(void)
{
	do_system("iptables -t nat -F %s 1>/dev/null 2>&1", PORT_FORWARD_CHAIN);
}
static void iptablesMaliciousFilterFlush(void)
{
	do_system("iptables -F %s  1>/dev/null 2>&1", MALICIOUS_FILTER_CHAIN);
	do_system("iptables -F %s  1>/dev/null 2>&1", SYNFLOOD_FILTER_CHAIN);
	
	do_system("iptables -A %s -p tcp --syn -j %s 1>/dev/null 2>&1", MALICIOUS_FILTER_CHAIN, SYNFLOOD_FILTER_CHAIN);
	
	do_system("iptables -F %s  1>/dev/null 2>&1", MALICIOUS_INPUT_FILTER_CHAIN);
	do_system("iptables -F %s  1>/dev/null 2>&1", SYNFLOOD_INPUT_FILTER_CHAIN);
	
	do_system("iptables -A %s -p tcp --syn -j %s 1>/dev/null 2>&1", MALICIOUS_INPUT_FILTER_CHAIN, SYNFLOOD_INPUT_FILTER_CHAIN);
	
	/* disable tcp_syncookie capacity */
	do_system("echo 0 > /proc/sys/net/ipv4/tcp_syncookies 2>&1 1>/dev/null");
}

static void iptablesDMZFlush(void)
{
	do_system("iptables -t nat -F %s 1>/dev/null 2>&1", DMZ_CHAIN);
	do_system("/bin/super_dmz -f");
}
static void makePortTriggerRule(char *buf, int len, int triggerProto, int triggerPort, int incomingProto, int incomingPort)
{
		int rc = 0;
		char *pos = buf;

		rc = snprintf(pos, len-rc, 
			"iptables -t filter -A %s ", PORT_TRIGGER_CHAIN);
		pos = pos + rc;
    
		// write protocol type
		if(triggerProto == PROTO_TCP)
			rc = snprintf(pos, len-rc, "-p tcp ");
		else if (triggerProto == PROTO_UDP)
			rc = snprintf(pos, len-rc, "-p udp ");
		pos = pos + rc;

		// write trigger port
		rc = snprintf(pos, len-rc, "--dport %d ", triggerPort);
		pos = pos + rc;
		// write TRIGGER target
		rc = snprintf(pos, len-rc, "-j TRIGGER --trigger-type out ");
		pos = pos + rc;

		// write incoming protocol type
		if(incomingProto == PROTO_TCP)
			rc = snprintf(pos, len-rc, "--trigger-proto tcp ");
		else if (incomingProto == PROTO_UDP)
			rc = snprintf(pos, len-rc, "--trigger-proto udp ");
		pos = pos + rc;
		
		rc = snprintf(pos, len-rc, "--trigger-match %d --trigger-relate %d",triggerPort, incomingPort);
		pos = pos + rc;

	
}
static void makePortForwardRule(char *buf, int len, char *wan_ip, char *ip_address, int proto, int prf_int, int prt_int)
{
		int rc = 0;
		char *pos = buf;
//		get_ifip(get_wanif_name(), wan_ip);
		rc = snprintf(pos, len-rc, 
			//"iptables -t nat -A %s -j DNAT -i %s ", PORT_FORWARD_CHAIN, wan_name);
			"iptables -t nat -A %s -j DNAT -d %s ", PORT_FORWARD_CHAIN, wan_ip);
		pos = pos + rc;

		// write protocol type
		if(proto == PROTO_TCP)
			rc = snprintf(pos, len-rc, "-p tcp ");
		else if (proto == PROTO_UDP)
			rc = snprintf(pos, len-rc, "-p udp ");
		else if (proto == PROTO_TCP_UDP)
			rc = snprintf(pos, len-rc, " ");
		pos = pos + rc;

		// write port
		if(prt_int != 0)
			rc = snprintf(pos, len-rc, "--dport %d:%d ", prf_int, prt_int);
		else
			rc = snprintf(pos, len-rc, "--dport %d ", prf_int);
		pos = pos + rc;

		// write remote ip
		rc = snprintf(pos, len-rc, "--to %s ", ip_address);
		pos = pos + rc;
}
static void makeSinglePortForwardRule(char *buf, int len, char *wan_name, char *ip_address, int proto, int publicPort_int, int privatePort_int)
{
		int rc = 0;
		char *pos = buf;

		rc = snprintf(pos, len-rc, 
			"iptables -t nat -A %s -j DNAT -i %s ", PORT_FORWARD_CHAIN, wan_name);
		pos = pos + rc;

		// write protocol type
		if(proto == PROTO_TCP)
			rc = snprintf(pos, len-rc, "-p tcp ");
		else if (proto == PROTO_UDP)
			rc = snprintf(pos, len-rc, "-p udp ");
		else if (proto == PROTO_TCP_UDP)
			rc = snprintf(pos, len-rc, " ");
		pos = pos + rc;

		// write public port
		rc = snprintf(pos, len-rc, "--dport %d ", publicPort_int);
		pos = pos + rc;

		// write remote ip & private port
		rc = snprintf(pos, len-rc, "--to-destination %s:%d", ip_address, privatePort_int);
		//rc = snprintf(pos, len-rc, "--to %s ", ip_address);
}

static void makeDMZRule(char *buf, int len, char *wan_name, char *ip_address, int avoidtcpport80)
{
	int rc = snprintf(buf, len, "iptables -t nat -A %s -j DNAT -i %s --to %s", DMZ_CHAIN, wan_name, ip_address);
		//snprintf(buf+rc, len, ";iptables -t nat -A %s -j DNAT -i %s -p tcp --dport ! %d --to %s", DMZ_CHAIN, wan_name,	getGoAHeadServerPort(), ip_address);
	if(avoidtcpport80)
		snprintf(buf+rc, len, ";iptables -t nat -A %s -j DNAT -i %s -p tcp --dport ! %d --to %s", DMZ_CHAIN, wan_name, 80, ip_address);
	else
		snprintf(buf+rc, len, ";iptables -t nat -A %s -j DNAT -i %s -p tcp --to %s", DMZ_CHAIN, wan_name, ip_address);
}

static void iptablesDMZRun(void)
{
	char *address, *tcpport80;
	char *dmz_enable = (char *)nvram_bufget(RT2860_NVRAM, "DMZEnable");

	if(!dmz_enable){
		DBG_MSG("Warning: can't find \"DMZEnable\" in flash");
		return;
	}

	if(atoi(dmz_enable) == 1){
		address = (char *)nvram_bufget(RT2860_NVRAM, "DMZAddress");
		tcpport80 = (char *)nvram_bufget(RT2860_NVRAM, "DMZAvoidTCPPort80");
		if(!address || !tcpport80){
			DBG_MSG("Warning: can't find \"DMZAddress\" or \"DMZAvoidTCPPort80\" in flash");
			return;
		}

		makeDMZRule(cmd, sizeof(cmd), get_wanif_name(), address, atoi(tcpport80));
		do_system(cmd);
	}else if (atoi(dmz_enable) == 2 /* super dmz */){
		char *portforward = SuperDMZ_MakePortForwardAvoidStr();
		char *single_portforward = SuperDMZ_MakeSinglePortForwardAvoidStr();
		tcpport80 = (char *)nvram_bufget(RT2860_NVRAM, "DMZAvoidTCPPort80");
		do_system("/bin/super_dmz %s %s %s", (tcpport80 && atoi(tcpport80)) ? "-t 80 " : "", portforward ? portforward : "", single_portforward ? single_portforward : "");
	}
	return;
}
inline static void conntrack_flush(void)
{
	do_system("cat /proc/sys/net/netfilter/nf_conntrack_udp_timeout > /var/.udpbackup");
	do_system("echo 0 > /proc/sys/net/netfilter/nf_conntrack_udp_timeout");
	do_system("cat /var/.udpbackup > /proc/sys/net/netfilter/nf_conntrack_udp_timeout; rm -f /var/.udpbackup");

	do_system("cat /proc/sys/net/netfilter/nf_conntrack_tcp_timeout_established > /var/.tcpbackup");
	do_system("echo 0 > /proc/sys/net/netfilter/nf_conntrack_tcp_timeout_established");
	do_system("cat /var/.tcpbackup > /proc/sys/net/netfilter/nf_conntrack_tcp_timeout_established; rm -f /var/.tcpbackup");
}

/* Same as the file "linux/netfilter_ipv4/ipt_webstr.h" */
#define BLK_JAVA                0x01
#define BLK_ACTIVE              0x02
#define BLK_COOKIE              0x04
#define BLK_PROXY               0x08

void iptablesWebsFilterRun(void)
{
	int i;
	int content_filter = 0;
	char entry[256];
	char *url_filter = (char *)nvram_bufget(RT2860_NVRAM, "websURLFilters");
	char *host_filter = (char *)nvram_bufget(RT2860_NVRAM, "websHostFilters");
	char *proxy = (char *)nvram_bufget(RT2860_NVRAM, "websFilterProxy");
	char *java = (char *)nvram_bufget(RT2860_NVRAM, "websFilterJava");
	char *activex = (char *)nvram_bufget(RT2860_NVRAM, "websFilterActivex");
	char *cookies = (char *)nvram_bufget(RT2860_NVRAM, "websFilterCookies");
	if(!url_filter || !host_filter || !proxy || !java || !activex || !cookies)
		return;

	// Content filter
	if(!strcmp(java, "1"))
		content_filter += BLK_JAVA;
	if(!strcmp(activex, "1"))
		content_filter += BLK_ACTIVE;
	if(!strcmp(cookies, "1"))
		content_filter += BLK_COOKIE;
	if(!strcmp(proxy, "1"))
		content_filter += BLK_PROXY;

	if(content_filter){

		// Why only 3 ports are inspected?(This idea is from CyberTAN source code)
		// TODO: use layer7 to inspect HTTP
		do_system("iptables -A " WEB_FILTER_CHAIN " -p tcp -m tcp --dport 80   -m webstr --content %d -j REJECT --reject-with tcp-reset", content_filter);
		do_system("iptables -A " WEB_FILTER_CHAIN " -p tcp -m tcp --dport 3128 -m webstr --content %d -j REJECT --reject-with tcp-reset", content_filter);
		do_system("iptables -A " WEB_FILTER_CHAIN " -p tcp -m tcp --dport 8080 -m webstr --content %d -j REJECT --reject-with tcp-reset", content_filter);
	}

	// URL filter
	i=0;
	while( (get_nth_value(i++, url_filter, ';', entry, sizeof(entry)) != -1) ){
		if(strlen(entry)){
			if(!strncasecmp(entry, "http://", strlen("http://")))
				strcpy(entry, entry + strlen("http://"));
			
				do_system("iptables -A " WEB_FILTER_CHAIN " -p tcp -m tcp -m webstr --url  %s -j REJECT --reject-with tcp-reset", entry);
		}
	}

	// HOST(Keyword) filter
	i=0;
	while( (get_nth_value(i++, host_filter, ';', entry, sizeof(entry)) != -1) ){
		if(strlen(entry)){

			do_system("iptables -A " WEB_FILTER_CHAIN " -p tcp -m tcp -m webstr --host %s -j REJECT --reject-with tcp-reset", entry);
		}
	}

	return;
}

static void iptablesSinglePortForwardRun(void)
{
	int i=0;
	char rec[256];
	char wan_name[16];

	int publicPort_int, privatePort_int, proto;
	char ip_address[32], publicPort[8], privatePort[8], protocol[8];
  char *firewall_enable, *rule;
 
    firewall_enable = (char *)nvram_bufget(RT2860_NVRAM, "SinglePortForwardEnable");
    if(!firewall_enable){
        DBG_MSG("Warning: can't find \"SinglePortForwardEnable\" in flash");
        return;
    }
    if(atoi(firewall_enable)){
        rule = (char *)nvram_bufget(RT2860_NVRAM, "SinglePortForwardRules");
        if(!rule){
            DBG_MSG("Warning: can't find \"SinglePortForwardRules\" in flash");
            return ;
        }
    }else
		return;

	strncpy(wan_name, get_wanif_name(), sizeof(wan_name)-1);

	while( (get_nth_value(i++, rule, ';', rec, sizeof(rec)) != -1) ){
		// get ip address
	
		if((get_nth_value(0, rec, ',', ip_address, sizeof(ip_address)) == -1)){
			DBG_MSG("ip_address = %s", ip_address);	
			continue;
		}
		if(!is_ip_valid(ip_address)){	
			continue;
   		}
		// get public port
		if((get_nth_value(1, rec, ',', publicPort, sizeof(publicPort)) == -1)){
			DBG_MSG("publicPort = %s", publicPort);	
			continue;
		}
		if( (publicPort_int = atoi(publicPort)) == 0 || publicPort_int > 65535)
			continue;

		// get private port
		if((get_nth_value(2, rec, ',', privatePort, sizeof(privatePort)) == -1)){
			DBG_MSG("privatePort = %s", privatePort);	
			continue;
		}
		if( (privatePort_int = atoi(privatePort)) == 0 || privatePort_int > 65535)
			continue;

		// get protocol
		if((get_nth_value(3, rec, ',', protocol, sizeof(protocol)) == -1))
			continue;
		proto = atoi(protocol);
		switch(proto){
			case PROTO_TCP:
			case PROTO_UDP:
				
				makeSinglePortForwardRule(cmd, sizeof(cmd), wan_name, ip_address, proto, publicPort_int, privatePort_int);
				do_system(cmd);
				break;
			case PROTO_TCP_UDP:
			
				makeSinglePortForwardRule(cmd, sizeof(cmd), wan_name, ip_address, PROTO_TCP, publicPort_int, privatePort_int);
				do_system(cmd);
				makeSinglePortForwardRule(cmd, sizeof(cmd), wan_name, ip_address, PROTO_UDP, publicPort_int, privatePort_int);
				do_system(cmd);
				break;
			default:
				continue;
		}
	}
}
static void iptablesPortTriggerRun(void)
{
	int i=0;
	char rec[256];

	int triggerPort_int, incomingPort_int, triggerProto, incomingProto;
	char triggerPort[8], incomingPort[8], triggerProtocol[8], incomingProtocol[8];

  char *firewall_enable, *rule;
    firewall_enable = (char *)nvram_bufget(RT2860_NVRAM, "PortTriggerEnable");
    if(!firewall_enable){
        DBG_MSG("Warning: can't find \"PortTriggerEnable\" in flash");
        return;
    }
	if(atoi(firewall_enable)){
        rule = (char *)nvram_bufget(RT2860_NVRAM, "PortTriggerRules");
        if(!rule){
            DBG_MSG("Warning: can't find \"PortTriggerRules\" in flash");
            return ;
        }
   }else{
		return;
	}
	while( (get_nth_value(i++, rule, ';', rec, sizeof(rec)) != -1) ){
		//get trigger protocol
		if((get_nth_value(0, rec, ',', triggerProtocol, sizeof(triggerProtocol)) == -1))
			continue;
		triggerProto = atoi(triggerProtocol);
		// get trigger port
		if((get_nth_value(1, rec, ',', triggerPort, sizeof(triggerPort)) == -1)){
			//DBG_MSG("triggerPort = %s", triggerPort);	
			continue;
		}
		if( (triggerPort_int = atoi(triggerPort)) == 0 || triggerPort_int > 65535)
			continue;
        // get incoming portoocol
        if((get_nth_value(2, rec, ',', incomingProtocol, sizeof(incomingProtocol)) == -1))
			continue;
		incomingProto = atoi(incomingProtocol);
		// get incoming port
        if((get_nth_value(3, rec, ',', incomingPort, sizeof(incomingPort)) == -1)){
			//DBG_MSG("incomingPort = %s", incomingPort);	
			continue;
		}
		if( (incomingPort_int = atoi(incomingPort)) == 0 || incomingPort_int > 65535)
			continue;
	
		makePortTriggerRule(cmd, sizeof(cmd), triggerProto, triggerPort_int, incomingProto, incomingPort_int);
		do_system(cmd);
	}
		do_system("iptables -t nat -A %s -j TRIGGER --trigger-type dnat", PORT_TRIGGER_PREROUTING_CHAIN);
}
static void iptablesPortForwardRun(void)
{
	int i=0;
	char rec[256];
	//char wan_ip[16];
	int prf_int, prt_int, proto;
	char ip_address[32], prf[8], prt[8], protocol[8];
  char *firewall_enable, *rule, *wan_ip;

    firewall_enable = (char *)nvram_bufget(RT2860_NVRAM, "PortForwardEnable");
    if(!firewall_enable){
        DBG_MSG("Warning: can't find \"PortForwardEnable\" in flash");
        return;
    }
    if(atoi(firewall_enable)){
        rule = (char *)nvram_bufget(RT2860_NVRAM, "PortForwardRules");
        if(!rule){
            DBG_MSG("Warning: can't find \"PortForwardRules\" in flash");
            return ;
        }
    }else
		return;


wan_ip = (char *)nvram_bufget(RT2860_NVRAM, "wan_ipaddr");

//	strncpy(wan_name, get_wanif_name(), sizeof(wan_name)-1);

	while( (get_nth_value(i++, rule, ';', rec, sizeof(rec)) != -1) ){
		// get ip address

		if((get_nth_value(0, rec, ',', ip_address, sizeof(ip_address)) == -1)){
			DBG_MSG("ip address=%s", ip_address);
			continue;
		}
		
		if(!is_ip_valid(ip_address))
			continue;
			

		// get port range "from"
		if((get_nth_value(1, rec, ',', prf, sizeof(prf)) == -1)){
			DBG_MSG("prf = %s", prf);	
			continue;
		}
		if( (prf_int = atoi(prf)) == 0 || prf_int > 65535)
			continue;

		// get port range "to"
		if((get_nth_value(2, rec, ',', prt, sizeof(prt)) == -1)){
			DBG_MSG("prt = %s", prt);	
			continue;
		}
		if( (prt_int = atoi(prt)) > 65535)
			continue;

		// get protocol
		if((get_nth_value(3, rec, ',', protocol, sizeof(protocol)) == -1))
			continue;
		proto = atoi(protocol);
		switch(proto){
			case PROTO_TCP:
			case PROTO_UDP:
				
				makePortForwardRule(cmd, sizeof(cmd), wan_ip, ip_address, proto, prf_int, prt_int);
				do_system(cmd);
				break;
			case PROTO_TCP_UDP:
				
				makePortForwardRule(cmd, sizeof(cmd), wan_ip, ip_address, PROTO_TCP, prf_int, prt_int);
				do_system(cmd);
				makePortForwardRule(cmd, sizeof(cmd), wan_ip, ip_address, PROTO_UDP, prf_int, prt_int);
				do_system(cmd);
				break;
			default:
				continue;
		}
	}

}
static void websURLFilterDelete(char *input)
{
	int i, j, rule_count;
	char name_buf[16];
	//char_t *value;
	char *value;
	int *deleArray;

	char *new_rules;	

    const char *rules = (char *)nvram_bufget(RT2860_NVRAM, "websURLFilters");
    if(!rules || !strlen(rules) )
        return;

	rule_count = getRuleNums((char *)rules);
	if(!rule_count)
		return;

	
	deleArray = (int *)malloc(rule_count * sizeof(int));
	if(!deleArray)
		return;

    new_rules = strdup(rules);
    if(!new_rules){
        free(deleArray);
        return;
    }

	web_debug_header();
	for(i=0, j=0; i< rule_count; i++){
		snprintf(name_buf, 16, "DR%d", i);
		//value = websGetVar(wp, name_buf, NULL);
		value = web_get(name_buf, input, 1);
		if(strcmp(value, "")!=0){
			deleArray[j++] = i;
		}
	}
	if(!j){
        free(deleArray);
        free(new_rules);
		
		web_debug_header();
		printf("You didn't select any rules to delete.<br>\n");
		/*
	    websHeader(wp);
	    websWrite(wp, T("You didn't select any rules to delete.<br>\n"));
	    websFooter(wp);
	    websDone(wp, 200);
	    */
		return;
	}

	delete_nth_value(deleArray, rule_count, new_rules, ';');
	free(deleArray);

	nvram_set(RT2860_NVRAM, "websURLFilters", new_rules);
	nvram_commit(RT2860_NVRAM);

	free(new_rules);

	/* TODO: check YY */
	do_system("iptables -t filter -F " WEB_FILTER_CHAIN);
	iptablesWebsFilterRun();
	printf("Delete : success<br>\n");
/*
    websHeader(wp);
    websWrite(wp, T("Delete : success<br>\n") );
    websFooter(wp);
    websDone(wp, 200);
*/
	return;
}
static void websHostFilterDelete(char *input)
{
	int i, j, rule_count;
	char name_buf[16];
	//char_t *value;
	char *value;
	int *deleArray;

	char *new_rules;
	const char *rules = (char *)nvram_bufget(RT2860_NVRAM, "websHostFilters");
    if(!rules || !strlen(rules) )
        return;

	rule_count = getRuleNums((char *)rules);
	if(!rule_count)
		return;

	deleArray = (int *)malloc(rule_count * sizeof(int));
	if(!deleArray)
		return;

    new_rules = strdup(rules);
    if(!new_rules){
        free(deleArray);
        return;
    }

	web_debug_header();
	for(i=0, j=0; i< rule_count; i++){
		snprintf(name_buf, 16, "DR%d", i);
		//value = websGetVar(wp, name_buf, NULL);
		value = web_get(name_buf, input, 1);
		if(strcmp(value, "")!=0){
			deleArray[j++] = i;
		}
	}
	if(!j){
		free(deleArray);
		free(new_rules);
	  web_debug_header();
		printf("You didn't select any rules to delete.<br>\n");
		/*
	    websHeader(wp);
	    websWrite(wp, T("You didn't select any rules to delete.<br>\n"));
	    websFooter(wp);
	    websDone(wp, 200);
	    */
		return;
	}

	delete_nth_value(deleArray, rule_count, new_rules, ';');
	free(deleArray);

	nvram_set(RT2860_NVRAM, "websHostFilters", new_rules);
	nvram_commit(RT2860_NVRAM);

	free(new_rules);

	/* TODO: check YY */
	do_system("iptables -t filter -F " WEB_FILTER_CHAIN);
	iptablesWebsFilterRun();
	printf("Delete : success<br>\n");
/*
    websHeader(wp);
    websWrite(wp, T("Delete : success<br>\n") );
    websFooter(wp);
    websDone(wp, 200);
*/
	return;
}

static void ipportFilterDelete(char *input)
{
	int i, j, rule_count;
	char name_buf[16];
	//char_t *value;
	char *value;
	int *deleArray;
//	char *firewall_enable;

	char *new_rules;
    const char *rules = (char *)nvram_bufget(RT2860_NVRAM, "IPPortFilterRules");
    if(!rules || !strlen(rules) )
        return;

	rule_count = getRuleNums((char *)rules);
	if(!rule_count)
		return;

	deleArray = (int *)malloc(rule_count * sizeof(int));
	if(!deleArray)
		return;
	
	new_rules = strdup(rules);
	if(!new_rules){
		free(deleArray);
		return;
	}

	web_debug_header();
	for(i=0, j=0; i< rule_count; i++){
		snprintf(name_buf, 16, "delRule%d", i);
		//value = websGetVar(wp, name_buf, NULL);
		value = web_get(name_buf, input, 1);
		if(strcmp(value, "")!=0){
			deleArray[j++] = i;
		}
	}
	if(!j){
		free(deleArray);
		free(new_rules);
		web_debug_header();
		printf("You didn't select any rules to delete.<br>\n");
		/*
	    websHeader(wp);
	    websWrite(wp, T("You didn't select any rules to delete.<br>\n"));
	    websFooter(wp);
	    websDone(wp, 200);
	    */
		return;
	}

	delete_nth_value(deleArray, rule_count, new_rules, ';');
	free(deleArray);

	nvram_set(RT2860_NVRAM, "IPPortFilterRules", new_rules);
	nvram_commit(RT2860_NVRAM);
	free(new_rules);

	iptablesIPPortFilterFlush();
	iptablesIPPortFilterRun();
	
/*
    websHeader(wp);
    websWrite(wp, T("s<br>\n") );
    websWrite(wp, T("fromPort: <br>\n"));
    websWrite(wp, T("toPort: <br>\n"));
    websWrite(wp, T("protocol: <br>\n"));
    websWrite(wp, T("comment: <br>\n"));
    websFooter(wp);
    websDone(wp, 200);
*/
	return;
}

static void portForwardDelete(char *input)
{
	int i, j, rule_count;
	char name_buf[16];
	//char_t *value;
	char *value;
	int *deleArray;
	char *firewall_enable;

	char *new_rules;
    const char *rules = (char *)nvram_bufget(RT2860_NVRAM, "PortForwardRules");
    if(!rules || !strlen(rules) )
        return;

	rule_count = getRuleNums((char *)rules);
	if(!rule_count)
		return;

	deleArray = (int *)malloc(rule_count * sizeof(int));
	if(!deleArray)
		return;

	new_rules = strdup(rules);
	if(!new_rules){
		free(deleArray);
		return;
	}

	web_debug_header();
	for(i=0, j=0; i< rule_count; i++){
		snprintf(name_buf, 16, "delRule%d", i);
		value = web_get(name_buf, input, 1);
		if(strcmp(value, "")!=0){
			deleArray[j++] = i;
		}
	}

    if(!j){
		free(deleArray);
		free(new_rules);
		printf("You didn't select any rules to delete.<br>\n");
/*		
        websHeader(wp);
        websWrite(wp, T("You didn't select any rules to delete.<br>\n"));
        websFooter(wp);
        websDone(wp, 200);
*/
        return;
    }

	delete_nth_value(deleArray, rule_count, new_rules, ';');
	free(deleArray);

	nvram_set(RT2860_NVRAM, "PortForwardRules", new_rules);
	nvram_commit(RT2860_NVRAM);
	free(new_rules);

	// restart iptables if it is running
	firewall_enable = (char *)nvram_bufget(RT2860_NVRAM, "PortForwardEnable");
	if(firewall_enable){
		if(atoi(firewall_enable)){
			// call iptables
			iptablesPortForwardFlush();
			iptablesPortForwardRun();
			iptablesSinglePortForwardRun();
			// restart DMZ
			iptablesDMZFlush();
			iptablesDMZRun();

			// flush conntrack
			conntrack_flush();
		}
	}
/*
    websHeader(wp);
    websWrite(wp, T("s<br>\n") );
    websWrite(wp, T("fromPort: <br>\n"));
    websWrite(wp, T("toPort: <br>\n"));
    websWrite(wp, T("protocol: <br>\n"));
    websWrite(wp, T("comment: <br>\n"));
    websFooter(wp);
    websDone(wp, 200);
*/
	return;
}
static void portTriggerDelete(char *input)
{
	int i, j, rule_count;
	char name_buf[16];
	//char *value;
	char *value;
	int *deleArray;
	char *firewall_enable;

	char *new_rules;
	
    const char *rules = (char *)nvram_bufget(RT2860_NVRAM, "PortTriggerRules");
    if(!rules || !strlen(rules) )
        return;

	rule_count = getRuleNums((char *)rules);
	if(!rule_count)
		return;

	deleArray = (int *)malloc(rule_count * sizeof(int));
	if(!deleArray)
		return;

	new_rules = strdup(rules);
	if(!new_rules){
		free(deleArray);
		return;
	}

	web_debug_header();
	for(i=0, j=0; i< rule_count; i++){
		snprintf(name_buf, 16, "delRule%d", i);
		value = web_get(name_buf, input, 1);
		if(strcmp(value, "")!=0){
			deleArray[j++] = i;
		}
	}

    if(!j){
		free(deleArray);
		free(new_rules);
		printf("You didn't select any rules to delete.\n");
		/*
        websHeader(wp);
        websWrite(wp, T("You didn't select any rules to delete.<br>\n"));
        websFooter(wp);
        websDone(wp, 200);
    */
        return;
    }

	delete_nth_value(deleArray, rule_count, new_rules, ';');
	free(deleArray);

	nvram_set(RT2860_NVRAM, "PortTriggerRules", new_rules);
	nvram_commit(RT2860_NVRAM);
	free(new_rules);

	// restart iptables if it is running
	firewall_enable = (char *)nvram_bufget(RT2860_NVRAM, "PortTriggerEnable");
	if(firewall_enable){
		if(atoi(firewall_enable)){
			// call iptables
			iptablesPortTriggerFlush();
			iptablesPortTriggerRun();
		}
		// restart DMZ
		iptablesDMZFlush();
		iptablesDMZRun();

		conntrack_flush();
	}

	//printf("s<br>\n");
	printf("triggerPortProtocol:\n");
	printf("triggerPortNumber: \n");
	printf("incomingPortProtocol: \n");
	printf("incomingPortNumber: \n");	
	printf("comment: \n");
/*
    websHeader(wp);
    websWrite(wp, T("triggerPortProtocol: <br>\n"));
    websWrite(wp, T("triggerPortNumber: <br>\n"));
	websWrite(wp, T("incomingPortProtocol: <br>\n"));
	websWrite(wp, T("incomingPortNumber: <br>\n"));
    websWrite(wp, T("comment: <br>\n"));
    websFooter(wp);
    websDone(wp, 200);
*/
	return;
}              
static void singlePortForwardDelete(char *input)
{
	int i, j, rule_count;
	char name_buf[16];
	//char_t *value;
	char *value;
	int *deleArray;
	char *firewall_enable;

	char *new_rules;
    const char *rules = (char *)nvram_bufget(RT2860_NVRAM, "SinglePortForwardRules");
    if(!rules || !strlen(rules) )
        return;

	rule_count = getRuleNums((char *)rules);
	if(!rule_count)
		return;

	deleArray = (int *)malloc(rule_count * sizeof(int));
	if(!deleArray)
		return;

	new_rules = strdup(rules);
	if(!new_rules){
		free(deleArray);
		return;
	}

	web_debug_header();
	for(i=0, j=0; i< rule_count; i++){
		snprintf(name_buf, 16, "delRule%d", i);
		//value = websGetVar(wp, T(name_buf), NULL);
		value = web_get(name_buf, input, 1);
		if(strcmp(value, "")!=0){
			deleArray[j++] = i;
		}
	}

    if(!j){
		free(deleArray);
		free(new_rules);
		printf("You didn't select any rules to delete.<br>\n");
		/*
        websHeader(wp);
        websWrite(wp, T("You didn't select any rules to delete.<br>\n"));
        websFooter(wp);
        websDone(wp, 200);
        */
        return;
    }

	delete_nth_value(deleArray, rule_count, new_rules, ';');
	free(deleArray);

	nvram_set(RT2860_NVRAM, "SinglePortForwardRules", new_rules);
	nvram_commit(RT2860_NVRAM);
	free(new_rules);

	// restart iptables if it is running
	firewall_enable = (char *)nvram_bufget(RT2860_NVRAM, "SinglePortForwardEnable");
	if(firewall_enable){
		if(atoi(firewall_enable)){
			// call iptables
			iptablesSinglePortForwardFlush();
			iptablesSinglePortForwardRun();
			iptablesPortForwardRun();
		}
		// restart DMZ
		iptablesDMZFlush();
		iptablesDMZRun();

		conntrack_flush();
	}
	//printf("s<br>\n");
	printf("publicPort: \n");
	printf("privatePort: \n");
	printf("protocol: \n");
	printf("comment: \n");

/*
    websHeader(wp);
    websWrite(wp, T("s<br>\n") );
    websWrite(wp, T("publicPort: <br>\n"));
    websWrite(wp, T("privatePort: <br>\n"));
    websWrite(wp, T("protocol: <br>\n"));
    websWrite(wp, T("comment: <br>\n"));
    websFooter(wp);
    websDone(wp, 200);
*/
	return;
}


static void DMZ(char *input)
{
/*   char *dmzE, *address, *avoidtcp80;
	dmzE = websGetVar(wp, T("DMZEnabled"), T(""));
	address = websGetVar(wp, T("DMZAddress"), T(""));
	avoidtcp80 = websGetVar(wp, T("DMZTCPPort80"), T(""));
*/
  char *dmzE, *address, *avoidtcp80;

  	dmzE = address = avoidtcp80 = NULL;
	web_debug_header();
	dmzE = strdup(web_get("DMZEnabled",input, 1));
	address = strdup(web_get("DMZAddress",input, 1));
	avoidtcp80 = strdup(web_get("DMZTCPPort80",input, 1));
	// someone use malform page.....
	if(!dmzE && !strlen(dmzE))
		goto leave;

	if((atoi(dmzE)==1) && !is_ip_valid(address))	// enable && invalid mac address
		goto leave;
	if((atoi(dmzE)==2) && !is_mac_valid(address))	// enable && invalid mac address
		goto leave;

	iptablesDMZFlush();

	if(atoi(dmzE) == 1){

		nvram_set(RT2860_NVRAM, "DMZEnable", "1");
		if(strlen(address)){
			nvram_set(RT2860_NVRAM, "DMZAddress", address);
		
		}
			nvram_set(RT2860_NVRAM, "DMZAvoidTCPPort80", strlen(avoidtcp80) ? avoidtcp80 : "0");
	}else if (atoi(dmzE)==2){					// enable
		nvram_set(RT2860_NVRAM, "DMZEnable", "2");
		if(strlen(address))
			nvram_set(RT2860_NVRAM, "DMZAddress", address);
			nvram_set(RT2860_NVRAM, "DMZAvoidTCPPort80", strlen(avoidtcp80) ? avoidtcp80 : "0");
	}else{
		nvram_set(RT2860_NVRAM, "DMZEnable", "0");
		nvram_set(RT2860_NVRAM, "DMZAvoidTCPPort80", "0");
	}

	nvram_commit(RT2860_NVRAM);
	iptablesDMZRun();

leave:
/*
	websHeader(wp);
	websWrite(wp, T("DMZEnabled: %s<br>\n"), dmzE);
	websWrite(wp, T("address: %s<br>\n"), address);
    websFooter(wp);
    websDone(wp, 200);
*/
	free_all(3, dmzE, address, avoidtcp80);
}

static void websSysFirewall(char *input)
{
/*
	char *rmE = websGetVar(wp, T("remoteManagementEnabled"), T(""));
	char *wpfE = websGetVar(wp, T("pingFrmWANFilterEnabled"), T(""));
	char *bpsE = websGetVar(wp, T("blockPortScanEnabled"), T(""));
	char *bsfE = websGetVar(wp, T("blockSynFloodEnabled"), T(""));
	char *spifw = websGetVar(wp, T("spiFWEnabled"), T("1"));
*/
	char *rmE, *wpfE, *bpsE, *bsfE, *spifw;

	rmE = wpfE = bpsE = bsfE = spifw = NULL;
	web_debug_header();
	rmE = strdup(web_get("remoteManagementEnabled", input, 1));
	wpfE = strdup(web_get("pingFrmWANFilterEnabled", input, 1));
	bpsE = strdup(web_get("blockPortScanEnabled", input, 1));
	bsfE = strdup(web_get("blockSynFloodEnabled", input, 1));
	spifw = strdup(web_get("spiFWEnabled", input, 1));
	// someone use malform page.....
	if(!rmE || !strlen(rmE))
		goto leave;
	if(!wpfE || !strlen(wpfE))
		goto leave;
	if(!spifw || !strlen(spifw))
		goto leave;
	if(!bpsE || !strlen(bpsE))
		goto leave;
	if(!bsfE || !strlen(bsfE))
		goto leave;

	// TODO: make a new chain instead of flushing the whole INPUT chain
//	do_system("iptables -t filter -F INPUT");

	if(atoi(rmE) == 0){		// disable
		nvram_bufset(RT2860_NVRAM, "RemoteManagement", "0");
	}else{					// enable
		nvram_bufset(RT2860_NVRAM, "RemoteManagement", "1");
	}

	if(atoi(wpfE) == 0){		// disable
		nvram_bufset(RT2860_NVRAM, "WANPingFilter", "0");
	}else{					// enable
		nvram_bufset(RT2860_NVRAM, "WANPingFilter", "1");
//		do_system("iptables -t filter -A INPUT -i %s -p icmp -j DROP", get_wanif_name());
	}

	if(atoi(spifw) == 0){		// disable
		nvram_bufset(RT2860_NVRAM, "SPIFWEnabled", "0");
	}else{					// enable
		nvram_bufset(RT2860_NVRAM, "SPIFWEnabled", "1");
	}

	if(atoi(bpsE) == 0){		// disable
		nvram_bufset(RT2860_NVRAM, "BlockPortScan", "0");
	}else{					// enable
		nvram_bufset(RT2860_NVRAM, "BlockPortScan", "1");
	}

	if(atoi(bsfE) == 0){		// disable
		nvram_bufset(RT2860_NVRAM, "BlockSynFlood", "0");
	}else{					// enable
		nvram_bufset(RT2860_NVRAM, "BlockSynFlood", "1");
	}
	nvram_commit(RT2860_NVRAM);

	iptablesIPPortFilterFlush();
	iptablesIPPortFilterRun();

	iptablesMaliciousFilterFlush();
	iptablesMaliciousFilterRun();

	iptablesRemoteManagementRun();
	
leave:
/*
	websHeader(wp);
	websWrite(wp, T("RemoteManage: %s<br>\n"), rmE);
	websWrite(wp, T("WANPingFilter: %s<br>\n"), wpfE);
	websWrite(wp, T("SPIFWEnabled: %s<br>\n"), spifw);
	websWrite(wp, T("BlockPortScan: %s<br>\n"), bpsE);
	websWrite(wp, T("BlockSynFlood: %s<br>\n"), bsfE);
    websFooter(wp);
    websDone(wp, 200);
*/
	free_all(5, rmE, wpfE, bpsE, bsfE, spifw);
	return;
}

static void websHostFilter(char *input)
{
	char *hostfilters = (char *)nvram_bufget(RT2860_NVRAM, "websHostFilters");
	//char *rule = websGetVar(wp, T("addHostFilter"), T(""));
	char *rule;
	char *new_hostfilters;

	web_debug_header();
	rule = web_get("addHostFilter", input, 1);
	if(!rule)
		return;
	if(strchr(rule, ';'))
		return;

	if(!hostfilters || !strlen(hostfilters))
		nvram_bufset(RT2860_NVRAM, "websHostFilters", rule);
	else{
		if(! (new_hostfilters = (char *)malloc(sizeof(char) * (strlen(hostfilters)+strlen(rule)+2))))
			return;
		new_hostfilters[0] = '\0';
		strcat(new_hostfilters, hostfilters);
		strcat(new_hostfilters, ";");
		strcat(new_hostfilters, rule);
		nvram_bufset(RT2860_NVRAM, "websHostFilters", new_hostfilters);
		free(new_hostfilters);
	}
	nvram_commit(RT2860_NVRAM);

	/* TODO: check YY */
	do_system("iptables -t filter -F " WEB_FILTER_CHAIN);
	iptablesWebsFilterRun();
/*
    websHeader(wp);
    websWrite(wp, T("add Host filter: %s<br>\n"), rule);
    websFooter(wp);
    websDone(wp, 200);
*/
}

static void websURLFilter(char *input)
{
	char *urlfilters = (char *)nvram_bufget(RT2860_NVRAM, "websURLFilters");
	//char *rule = websGetVar(wp, T("addURLFilter"), T(""));
	char *rule;
	char *new_urlfilters;

	web_debug_header();
	rule = web_get("addURLFilter", input, 1);

	if(!rule)
		return;
	if(strchr(rule, ';'))
		return;

	if(!urlfilters || !strlen(urlfilters)){

		nvram_bufset(RT2860_NVRAM, "websURLFilters", rule);
	}
	else{
		if(! (new_urlfilters = (char *)malloc(sizeof(char) * (strlen(urlfilters)+strlen(rule)+2))))
			return;
			new_urlfilters[0] = '\0';
			strcat(new_urlfilters, urlfilters);
			strcat(new_urlfilters, ";");
			strcat(new_urlfilters, rule);
			nvram_bufset(RT2860_NVRAM, "websURLFilters", new_urlfilters);
			free(new_urlfilters);
	}
	nvram_commit(RT2860_NVRAM);

	/* TODO: check YY */
	do_system("iptables -t filter -F " WEB_FILTER_CHAIN);
	iptablesWebsFilterRun();
	//printf("add URL filter: %s<br>\n",rule);
	
/*
    websHeader(wp);
    websWrite(wp, T("add URL filter: %s<br>\n"), rule);
    websFooter(wp);
    websDone(wp, 200);
    */

}

static void webContentFilter(char *input)
{
/*
	char *proxy = websGetVar(wp, T("websFilterProxy"), T(""));
	char *java = websGetVar(wp, T("websFilterJava"), T(""));
	char *activex = websGetVar(wp, T("websFilterActivex"), T(""));
	char *cookies = websGetVar(wp, T("websFilterCookies"), T(""));
*/
	char *proxy, *java, *activex, *cookies;

	proxy = java = activex = cookies = NULL;
	web_debug_header();
	proxy = strdup(web_get("websFilterProxy", input, 1));
	java = strdup(web_get("websFilterJava", input, 1));
	activex = strdup(web_get("websFilterActivex", input, 1));
	cookies = strdup(web_get("websFilterCookies", input, 1));


	// someone use malform page.....
	if(!proxy || !java || !activex || !cookies)
		goto leave;

	nvram_bufset(RT2860_NVRAM, "websFilterProxy",   atoi(proxy)   ? "1" : "0" );
	nvram_bufset(RT2860_NVRAM, "websFilterJava",    atoi(java)    ? "1" : "0" );
	nvram_bufset(RT2860_NVRAM, "websFilterActivex", atoi(activex) ? "1" : "0" );
	nvram_bufset(RT2860_NVRAM, "websFilterCookies", atoi(cookies) ? "1" : "0" );
	nvram_commit(RT2860_NVRAM);

	/* TODO: check YY */
	do_system("iptables -t filter -F " WEB_FILTER_CHAIN);
	iptablesWebsFilterRun();

	printf("Proxy: %s \n", atoi(proxy) ? "enable" : "disable");
	printf("Java: %s \n", atoi(java) ? "enable" : "disable");
	printf("Activex: %s \n", atoi(activex) ? "enable" : "disable");
	printf("Cookies: %s \n", atoi(cookies) ? "enable" : "disable");
	
/*
	websHeader(wp);
	websWrite(wp, T("Proxy: %s<br>\n"),  atoi(proxy) ? "enable" : "disable");
	websWrite(wp, T("Java: %s<br>\n"),   atoi(java) ? "enable" : "disable");
	websWrite(wp, T("Activex: %s<br>\n"), atoi(activex) ? "enable" : "disable");
	websWrite(wp, T("Cookies: %s<br>\n"), atoi(cookies) ? "enable" : "disable");
    websFooter(wp);
    websDone(wp, 200);
    */
leave:
	free_all(4, proxy, java, activex, cookies);
	return;
}

static void portForward(char *input)
{
	char rule[8192];
	char *ip_address, *pfe, *prf, *prt, *protocol, *comment;
	char *PortForwardRules;
	int prf_int, prt_int, proto;
  
	ip_address = pfe = prf = prt = protocol = comment = NULL;
	web_debug_header();
	pfe = strdup(web_get("portForwardEnabled",input, 1));
	ip_address = strdup(web_get("ip_address", input, 1));
	prf = strdup(web_get("fromPort", input, 1));
	prt = strdup(web_get("toPort" , input, 1));
	protocol = strdup(web_get("protocol" , input, 1));
	comment = strdup(web_get("comment", input, 1));

	if(!pfe && !strlen(pfe))
		goto end;

	if(!atoi(pfe)){
		nvram_set(RT2860_NVRAM, "PortForwardEnable", "0");
		iptablesPortForwardFlush();		//disable
		iptablesDMZFlush();
		iptablesDMZRun();
		conntrack_flush();
		goto end;
	}

	if(!strlen(ip_address) && !strlen(prf) && !strlen(prt) && !strlen(comment)){	// user choose nothing but press "apply" only
		nvram_set(RT2860_NVRAM, "PortForwardEnable", "1");
		iptablesPortForwardFlush();
		iptablesPortForwardRun();
		iptablesSinglePortForwardRun();
		iptablesDMZFlush();
		iptablesDMZRun();
		conntrack_flush();
		goto end;
	}
	if(!ip_address && !strlen(ip_address))
		goto end;
	if(!is_ip_valid(ip_address))
		goto end;

	// we dont trust user input.....
	if(!prf && !strlen(prf))
		goto end;
	if(!(prf_int = atoi(prf)) )
		goto end;
	if(prf_int > 65535)
		goto end;

	if(!prt)
		goto end;
	if(strlen(prt)){
		if( !(prt_int = atoi(prt)) )
			goto end;
		if(prt_int < prf_int)
			goto end;
		if(prt_int > 65535)
			goto end;
	}else{
		prt_int = 0;
	}

	if(! strcmp(protocol, "TCP"))
		proto = PROTO_TCP;
	else if( !strcmp(protocol, "UDP"))
		proto = PROTO_UDP;
	else if( !strcmp(protocol, "TCP&UDP"))
		proto = PROTO_TCP_UDP;
	else
		goto end;

	if(strlen(comment) > 32)
		goto end;
	/* i know you will try to break our box... ;) */
	if(strchr(comment, ';') || strchr(comment, ','))
		goto end;

	nvram_set(RT2860_NVRAM, "PortForwardEnable", "1");

	if(( PortForwardRules = (char *)nvram_bufget(RT2860_NVRAM, "PortForwardRules")) && strlen( PortForwardRules) )
		snprintf(rule, sizeof(rule), "%s;%s,%d,%d,%d,%s",  PortForwardRules, ip_address, prf_int, prt_int, proto, comment);
	else
		snprintf(rule, sizeof(rule), "%s,%d,%d,%d,%s", ip_address, prf_int, prt_int, proto, comment);

	nvram_set(RT2860_NVRAM, "PortForwardRules", rule);
	nvram_commit(RT2860_NVRAM);

	iptablesPortForwardFlush();
	// call iptables
	iptablesPortForwardRun();
	iptablesSinglePortForwardRun();

	iptablesDMZFlush();
	iptablesDMZRun();
end:
	
/*	
	websHeader(wp);
	printf("portForwardEnabled: %s<br>\n", pfe);
	printf("ip: %s<br>\n", ip_address);
	printf("fromPort: %s<br>\n", prf);
	printf("toPort: %s<br>\n", prt);
	printf("protocol: %s<br>\n", protocol);
	printf("comment: %s<br>\n", comment);
   	websFooter(wp);
    	websDone(wp, 200);
*/
	free_all(6, ip_address, pfe, prf, prt, protocol, comment);

	return;
}
static void portTrigger(char *input)
{
  	char *triggerPort, *incomingPort, *triggerProtocol, *incomingProtocol, *comment, *spfe;
	int triggerPort_int, incomingPort_int, triggerProto, incomingProto;
	char *PortTriggerRules;
	char rule[8192];

	triggerPort = incomingPort = triggerProtocol  = NULL;
	incomingProtocol = comment = spfe = NULL;
	web_debug_header();
	spfe = strdup(web_get("portTriggerEnabled", input, 1));
	triggerPort = strdup(web_get("triggerPortNumber" ,input, 1));
	incomingPort = strdup(web_get("incomingPortNumber" ,input, 1));
	triggerProtocol = strdup(web_get("triggerPortProtocol" ,input, 1));
	incomingProtocol = strdup(web_get("incomingPortProtocol" ,input, 1));
	comment = strdup(web_get("comment" ,input, 1));
	
	if(!spfe && !strlen(spfe))
		goto end;

	if(!atoi(spfe)){
		nvram_set(RT2860_NVRAM, "PortTriggerEnable", "0");
		iptablesPortTriggerFlush();	
		//disable
		iptablesSinglePortForwardFlush();
		//no chainge in rules
		iptablesDMZFlush();
		iptablesDMZRun();
		conntrack_flush();
		goto end;
	}

	if(!strlen(triggerPort) && !strlen(incomingPort) && !strlen(triggerProtocol) && !strlen(incomingProtocol) && !strlen(comment)){	// user choose nothing but press "apply" only
		nvram_set(RT2860_NVRAM, "PortTriggerEnable", "1");
		iptablesPortTriggerFlush();	
		iptablesPortTriggerRun();
		iptablesSinglePortForwardFlush();
		iptablesSinglePortForwardRun();
		iptablesDMZFlush();
		iptablesDMZRun();
	    conntrack_flush();
		goto end;
	}


	// we dont trust user input.....
	if(!triggerPort && !strlen(triggerPort))
		goto end;
	if(!(triggerPort_int = atoi(triggerPort)) )
		goto end;
	if(triggerPort_int > 65535)
		goto end;

	if(!incomingPort && !strlen(incomingPort))
		goto end;
	if(!(incomingPort_int = atoi(incomingPort)) )
		goto end;
	if(incomingPort_int > 65535)
		goto end;

	if(! strcmp(triggerProtocol, "TCP"))
		triggerProto = PROTO_TCP;
	else if( !strcmp(triggerProtocol, "UDP"))
		triggerProto = PROTO_UDP;
	else
		goto end;

	if(! strcmp(incomingProtocol, "TCP"))
		incomingProto = PROTO_TCP;
	else if( !strcmp(incomingProtocol, "UDP"))
		incomingProto = PROTO_UDP;
	else
		goto end;

	if(strlen(comment) > 32)
		goto end;
	/* i know you will try to break our box... ;) */
	if(strchr(comment, ';') || strchr(comment, ','))
		goto end;

	nvram_set(RT2860_NVRAM, "PortTriggerEnable", "1");
	if(( PortTriggerRules = (char *)nvram_bufget(RT2860_NVRAM, "PortTriggerRules")) && strlen( PortTriggerRules) )
		snprintf(rule, sizeof(rule), "%s;%d,%d,%d,%d,%s",  PortTriggerRules, triggerProto, triggerPort_int, incomingProto, incomingPort_int ,comment);
	else
		snprintf(rule, sizeof(rule), "%d,%d,%d,%d,%s", triggerProto, triggerPort_int, incomingProto, incomingPort_int, comment);

	nvram_set(RT2860_NVRAM, "PortTriggerRules", rule);
	nvram_commit(RT2860_NVRAM);

	iptablesPortTriggerFlush();
	// call iptables
	iptablesPortTriggerRun();
	iptablesDMZFlush();
	iptablesDMZRun();
end:
/*
	websHeader(wp);
	websWrite(wp, T("PortTriggerEnabled: %s<br>\n"), spfe);
	websWrite(wp, T("triggerProtocol: %s<br>\n"), triggerProtocol);
	websWrite(wp, T("triggerPort: %s<br>\n"), triggerPort);
	websWrite(wp, T("incomingProtocol: %s<br>\n"), incomingProtocol);
	websWrite(wp, T("incomingPort: %s<br>\n"), incomingPort);
	websWrite(wp, T("comment: %s<br>\n"), comment);

    websFooter(wp);
    websDone(wp, 200);    	
*/	
	free_all(6, triggerPort = incomingPort = triggerProtocol,
		    incomingProtocol = comment = spfe);

	return;
}

static void singlePortForward(char *input)
{
	char rule[8192];
	char *ip_address, *spfe, *publicPort, *privatePort, *protocol, *comment;
	char *SinglePortForwardRules;

	int publicPort_int, privatePort_int, proto;

	ip_address = spfe = publicPort = privatePort = protocol = comment = NULL;
	web_debug_header();
	spfe = strdup(web_get("singlePortForwardEnabled", input, 1));
	ip_address = strdup(web_get("ip_address" ,input, 1));
	publicPort = strdup(web_get("publicPort" ,input, 1));
	privatePort = strdup(web_get("privatePort" ,input, 1));
	protocol = strdup(web_get("protocol" ,input, 1));
	comment = strdup(web_get("comment" ,input, 1));
	
	if(!spfe && !strlen(spfe))
		goto end;

	if(!atoi(spfe)){
		nvram_set(RT2860_NVRAM, "SinglePortForwardEnable", "0");
		iptablesSinglePortForwardFlush();		//disable
		//no chainge in rules
		iptablesDMZFlush();
		iptablesDMZRun();
		conntrack_flush();
		goto end;
	}

	if(!strlen(ip_address) && !strlen(publicPort) && !strlen(privatePort) && !strlen(comment)){	// user choose nothing but press "apply" only
		nvram_set(RT2860_NVRAM, "SinglePortForwardEnable", "1");
		iptablesSinglePortForwardFlush();
		iptablesSinglePortForwardRun();
		iptablesPortForwardRun();
		iptablesDMZFlush();
		iptablesDMZRun();
	    conntrack_flush();
		goto end;
	}

	if(!ip_address && !strlen(ip_address))
		return;
	if(!is_ip_valid(ip_address))
		return;

	// we dont trust user input.....
	if(!publicPort && !strlen(publicPort))
		return;
	if(!(publicPort_int = atoi(publicPort)) )
		return;
	if(publicPort_int > 65535)
		return;

	if(!privatePort && !strlen(privatePort))
		return;
	if(!(privatePort_int = atoi(privatePort)) )
		return;
	if(privatePort_int > 65535)
		return;

	if(! strcmp(protocol, "TCP"))
		proto = PROTO_TCP;
	else if( !strcmp(protocol, "UDP"))
		proto = PROTO_UDP;
	else if( !strcmp(protocol, "TCP&UDP"))
		proto = PROTO_TCP_UDP;
	else
		return;

	if(strlen(comment) > 32)
		return;
	/* i know you will try to break our box... ;) */
	if(strchr(comment, ';') || strchr(comment, ','))
		return;

	nvram_set(RT2860_NVRAM, "SinglePortForwardEnable", "1");

	if(( SinglePortForwardRules = (char *)nvram_bufget(RT2860_NVRAM, "SinglePortForwardRules")) && strlen( SinglePortForwardRules) )
		snprintf(rule, sizeof(rule), "%s;%s,%d,%d,%d,%s",  SinglePortForwardRules, ip_address, publicPort_int, privatePort_int, proto, comment);
	else
		snprintf(rule, sizeof(rule), "%s,%d,%d,%d,%s", ip_address, publicPort_int, privatePort_int, proto, comment);

	nvram_set(RT2860_NVRAM, "SinglePortForwardRules", rule);
	nvram_commit(RT2860_NVRAM);

	iptablesSinglePortForwardFlush();
	// call iptables
	iptablesSinglePortForwardRun();
	iptablesPortForwardRun();
	iptablesDMZFlush();
	iptablesDMZRun();

end:
	/*
	websHeader(wp);
	printf("singlePortForwardEnabled: %s<br>\n"), spfe);
	printf("ip: %s<br>\n"), ip_address);
	printf("publicPort: %s<br>\n"), publicPort);
	printf("privatePort: %s<br>\n"), privatePort);
	printf("protocol: %s<br>\n"), protocol);
	printf("comment: %s<br>\n"), comment);
    websFooter(wp);
    websDone(wp, 200);
    */
	free_all(6, ip_address, spfe, publicPort, privatePort, protocol, comment);

	return;
}
static void iptablesNATPortTriggerClear(void)
{
	do_system("iptables -t nat -D PREROUTING -j %s 1>/dev/null 2>&1", PORT_TRIGGER_PREROUTING_CHAIN);
	do_system("iptables -t nat -F %s  1>/dev/null 2>&1; iptables -t nat -X %s  1>/dev/null 2>&1", PORT_TRIGGER_PREROUTING_CHAIN, PORT_TRIGGER_PREROUTING_CHAIN);
}
static void iptablesForwardFilterClear(void)
{
	do_system("iptables -F -t filter 1>/dev/null 2>&1");
}
static void iptablesDMZClear(void)
{
	do_system("iptables -t nat -D PREROUTING -j %s 1>/dev/null 2>&1", DMZ_CHAIN); // remove rule in PREROUTING chain
	do_system("iptables -t nat -F %s 1>/dev/null 2>&1; iptables -t nat -X %s  1>/dev/null 2>&1", DMZ_CHAIN, DMZ_CHAIN);
}
static void iptablesPortForwardClear(void)
{
	do_system("iptables -t nat -D PREROUTING -j %s 1>/dev/null 2>&1", PORT_FORWARD_CHAIN);
	do_system("iptables -t nat -F %s  1>/dev/null 2>&1; iptables -t nat -X %s  1>/dev/null 2>&1", PORT_FORWARD_CHAIN, PORT_FORWARD_CHAIN);
}
static void iptablesIPPortFilterClear(void)
{
	do_system("iptables -D FORWARD -j %s 1>/dev/null 2>&1", IPPORT_FILTER_CHAIN);
	do_system("iptables -F %s 1>/dev/null 2>&1", IPPORT_FILTER_CHAIN);
}
static void iptablesWebContentFilterClear(void)
{
	do_system("iptables -D FORWARD -j %s  1>/dev/null 2>&1", WEB_FILTER_CHAIN);
	do_system("iptables -F %s  1>/dev/null 2>&1", WEB_FILTER_CHAIN);
}
static void iptablesMaliciousFilterClear(void)
{
	do_system("iptables -D FORWARD -j %s 1>/dev/null 2>&1", MALICIOUS_FILTER_CHAIN);
	do_system("iptables -F %s  1>/dev/null 2>&1", MALICIOUS_FILTER_CHAIN);
	do_system("iptables -D INPUT -j %s 1>/dev/null 2>&1", MALICIOUS_INPUT_FILTER_CHAIN);
	do_system("iptables -F %s  1>/dev/null 2>&1", MALICIOUS_INPUT_FILTER_CHAIN);
}
static void iptablesPortTriggerClear(void)
{
	do_system("iptables -t filter -D FORWARD -j %s 1>/dev/null 2>&1", PORT_TRIGGER_CHAIN);
	do_system("iptables -t filter -F %s 1>/dev/null 2>&1", PORT_TRIGGER_CHAIN);
}
static void iptablesAllFilterClear(void)
{
	iptablesForwardFilterClear();
	iptablesIPPortFilterClear();
	iptablesWebContentFilterClear();
	iptablesMaliciousFilterClear();
  iptablesPortTriggerClear();
	iptablesNATPortTriggerClear();
	do_system("iptables -P INPUT ACCEPT");
	do_system("iptables -P OUTPUT ACCEPT");
	do_system("iptables -P FORWARD ACCEPT");
}

static void iptablesAllFilterRun(void)
{
	iptablesIPPortFilterRun();

	iptablesWebsFilterRun();

	/* system filter */
	iptablesRemoteManagementRun();

	iptablesMaliciousFilterRun();
}
static void iptablesAllNATClear(void)
{
	do_system("/bin/super_dmz -f");
	iptablesPortForwardClear();
	iptablesDMZClear();
	conntrack_flush();
}
static void iptablesAllNATRun(void)
{
	iptablesPortForwardRun();
	iptablesSinglePortForwardRun();
	iptablesDMZRun();
}

void firewall_init(void)
{
	//LoadLayer7FilterName();

	// init filter
	iptablesAllFilterClear();
	// make a new chain
	do_system("iptables -t filter -N %s 1>/dev/null 2>&1", WEB_FILTER_CHAIN);
	do_system("iptables -t filter -N %s 1>/dev/null 2>&1", IPPORT_FILTER_CHAIN);
	do_system("iptables -t filter -N %s 1>/dev/null 2>&1", MALICIOUS_FILTER_CHAIN);
	do_system("iptables -t filter -N %s 1>/dev/null 2>&1", SYNFLOOD_FILTER_CHAIN);
	do_system("iptables -t filter -N %s 1>/dev/null 2>&1", MALICIOUS_INPUT_FILTER_CHAIN);
	do_system("iptables -t filter -N %s 1>/dev/null 2>&1", SYNFLOOD_INPUT_FILTER_CHAIN);

	do_system("iptables -t filter -A FORWARD -j %s 1>/dev/null 2>&1", WEB_FILTER_CHAIN);
	do_system("iptables -t filter -A FORWARD -j %s 1>/dev/null 2>&1", IPPORT_FILTER_CHAIN);
	do_system("iptables -t filter -A FORWARD -j %s 1>/dev/null 2>&1", MALICIOUS_FILTER_CHAIN);
	do_system("iptables -t filter -A %s -p tcp --syn -j %s 1>/dev/null 2>&1", MALICIOUS_FILTER_CHAIN, SYNFLOOD_FILTER_CHAIN);
	do_system("iptables -t filter -A INPUT -j %s 1>/dev/null 2>&1", MALICIOUS_INPUT_FILTER_CHAIN);
	do_system("iptables -t filter -A %s -p tcp --syn -j %s 1>/dev/null 2>&1", MALICIOUS_INPUT_FILTER_CHAIN, SYNFLOOD_INPUT_FILTER_CHAIN);
	do_system("iptables -A FORWARD -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu 1>/dev/null 2>&1");
	iptablesAllFilterRun();

	// init NAT(DMZ)
	// We use -I instead of -A here to prevent from default MASQUERADE NAT rule
	// being in front of us.
	// So "port forward chain" has the highest priority in the system, and "DMZ chain" is the second one.
	iptablesAllNATClear();
	do_system("iptables -t nat -N %s 1>/dev/null 2>&1; iptables -t nat -I PREROUTING 1 -j %s 1>/dev/null 2>&1", PORT_FORWARD_CHAIN, PORT_FORWARD_CHAIN);
	do_system("iptables -t nat -N %s 1>/dev/null 2>&1; iptables -t nat -I PREROUTING 2 -j %s 1>/dev/null 2>&1", DMZ_CHAIN, DMZ_CHAIN);
	iptablesAllNATRun();
}

int main(int argc, char *argv[]) 
{
	char *firewall, *inStr;
	long inLen;
	
	nvram_init(RT2860_NVRAM);
	if ((argc > 1) && (!strcmp(argv[1], "init"))) {
		firewall_init();
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
	firewall = web_get("firewall", inStr, 0);
	// init filter
	iptablesAllFilterClear();
	do_system("iptables -t filter -N %s 1>/dev/null 2>&1", WEB_FILTER_CHAIN);
	do_system("iptables -t filter -N %s 1>/dev/null 2>&1", IPPORT_FILTER_CHAIN);
	
	do_system("iptables -t filter -N %s 1>/dev/null 2>&1", MALICIOUS_FILTER_CHAIN);
	do_system("iptables -t filter -N %s 1>/dev/null 2>&1", SYNFLOOD_FILTER_CHAIN);
	do_system("iptables -t filter -N %s 1>/dev/null 2>&1", MALICIOUS_INPUT_FILTER_CHAIN);
	do_system("iptables -t filter -N %s 1>/dev/null 2>&1", SYNFLOOD_INPUT_FILTER_CHAIN);
	do_system("iptables -t filter -A FORWARD -j %s 1>/dev/null 2>&1", WEB_FILTER_CHAIN);
	do_system("iptables -t filter -A FORWARD -j %s 1>/dev/null 2>&1", IPPORT_FILTER_CHAIN);
	do_system("iptables -t filter -A FORWARD -j %s 1>/dev/null 2>&1", MALICIOUS_FILTER_CHAIN);
	do_system("iptables -t filter -A %s -p tcp --syn -j %s 1>/dev/null 2>&1", MALICIOUS_FILTER_CHAIN, SYNFLOOD_FILTER_CHAIN);
	do_system("iptables -t filter -A INPUT -j %s 1>/dev/null 2>&1", MALICIOUS_INPUT_FILTER_CHAIN);
	do_system("iptables -t filter -A %s -p tcp --syn -j %s 1>/dev/null 2>&1", MALICIOUS_INPUT_FILTER_CHAIN, SYNFLOOD_INPUT_FILTER_CHAIN);
	do_system("iptables -A FORWARD -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu 1>/dev/null 2>&1");
	do_system("iptables -t filter -N %s 1>/dev/null 2>&1", PORT_TRIGGER_CHAIN);
	do_system("iptables -t filter -A FORWARD -j %s 1>/dev/null 2>&1", PORT_TRIGGER_CHAIN);
	do_system("iptables -t nat -N %s 1>/dev/null 2>&1", PORT_TRIGGER_PREROUTING_CHAIN);
	do_system("iptables -t nat -A PREROUTING -j %s 1>/dev/null 2>&1", PORT_TRIGGER_PREROUTING_CHAIN);
	iptablesAllFilterRun();

	// init NAT(DMZ)
	// We use -I instead of -A here to prevent from default MASQUERADE NAT rule 
	// being in front of us.
	// So "port forward chain" has the highest priority in the system, and "DMZ chain" is the second one.
	iptablesAllNATClear();
	do_system("iptables -t nat -N %s 1>/dev/null 2>&1; iptables -t nat -I PREROUTING 1 -j %s 1>/dev/null 2>&1", PORT_FORWARD_CHAIN, PORT_FORWARD_CHAIN);
	do_system("iptables -t nat -N %s 1>/dev/null 2>&1; iptables -t nat -I PREROUTING 2 -j %s 1>/dev/null 2>&1", DMZ_CHAIN, DMZ_CHAIN);
	
	iptablesAllNATRun();
	
	
	if (!strcmp(firewall, "portForward")) {
		portForward(inStr);
	} else if (!strcmp(firewall, "singlePortForward")){
		singlePortForward(inStr);
	} else if (!strcmp(firewall, "portForwardDelete")){
		portForwardDelete(inStr);
	} else if (!strcmp(firewall, "singlePortForwardDelete")){
		singlePortForwardDelete(inStr);
	} else if (!strcmp(firewall, "BasicSettings")){
		BasicSettings(inStr);
	} else if (!strcmp(firewall, "ipportFilter")){
		ipportFilter(inStr);
	} else if (!strcmp(firewall, "ipportFilterDelete")){
		ipportFilterDelete(inStr);
	} else if (!strcmp(firewall, "webContentFilter")){
		webContentFilter(inStr);
	} else if (!strcmp(firewall, "websURLFilterDelete")){
	  websURLFilterDelete(inStr);
	} else if (!strcmp(firewall, "websURLFilter")){
		websURLFilter(inStr);
	} else if (!strcmp(firewall, "websHostFilterDelete")){
		websHostFilterDelete(inStr);
	} else if (!strcmp(firewall, "websHostFilter")){
		websHostFilter(inStr);
	} else if (!strcmp(firewall, "DMZ")){
		DMZ(inStr);
	} else if (!strcmp(firewall, "websSysFirewall")){
		websSysFirewall(inStr);
	}	else if (!strcmp(firewall, "portTrigger")){
		portTrigger(inStr);
	}	else if (!strcmp(firewall, "portTriggerDelete")){
		portTriggerDelete(inStr);
	}
	free(inStr);

leave:
	nvram_close(RT2860_NVRAM);
	
	return 0;
}


