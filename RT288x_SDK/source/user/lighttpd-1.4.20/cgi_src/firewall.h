/*
 *	inic.h -- Wireless Configuration Header 
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: //WIFI_SOC/main/RT288x_SDK/source/user/goahead/src/firewall.h#6 $
 */

#ifndef HAVE_FIREWALL_H
#define HAVE_FIREWALL_H

#define PROTO_UNKNOWN	0
#define PROTO_TCP		1
#define PROTO_UDP		2
#define PROTO_TCP_UDP	3
#define PROTO_ICMP		4
#define PROTO_NONE		5


#define RULE_MODE_DISABLE	0
#define RULE_MODE_DROP		1
#define RULE_MODE_ACCEPT	2

#define ACTION_DROP		0
#define ACTION_ACCEPT	1

#define WEB_FILTER_CHAIN				"web_filter"
#define IPPORT_FILTER_CHAIN				"macipport_filter"
#define MALICIOUS_FILTER_CHAIN			"malicious_filter"
#define SYNFLOOD_FILTER_CHAIN			"synflood_filter"
#define MALICIOUS_INPUT_FILTER_CHAIN	"malicious_input_filter"
#define SYNFLOOD_INPUT_FILTER_CHAIN		"synflood_input_filter"

#define DMZ_CHAIN				"DMZ"
#define PORT_FORWARD_CHAIN		"port_forward"
#define PORT_TRIGGER_CHAIN      "port_trigger"
#define PORT_TRIGGER_PREROUTING_CHAIN      "trigger_prerouting"
#define MAGIC_NUM			"IPCHAINS"			

#define HTML_NO_FIREWALL_UNDER_BRIDGE_MODE	\
"<img src=\"/graphics/warning.gif\"><font color=#ff0000>&nbsp; &nbsp;Warning: The current operation mode is \"Bridge mode\" and these settings may not be functional.</font>"

void iptablesFilterClear(void);
void iptablesFilterRun(char *rule);
void iptablesWebsFilterRun(void);
void formDefineFirewall(void);
int isMacValid(char *);			// export for QoS

#endif
