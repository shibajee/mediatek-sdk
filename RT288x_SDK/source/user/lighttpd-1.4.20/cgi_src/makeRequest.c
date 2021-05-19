#include "utils.h"
#include <stdlib.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <linux/wireless.h>
#include "oid.h"

extern void getCurrentWscProfile(char *interface, WSC_CONFIGURED_VALUE *data, int len);
extern int getWscStatus(char *interface);

void arplookup(char *ip, char *arp)
{
	char buf[256];
	FILE *fd = fopen("/proc/net/arp", "r");
	if(!fd){
		strcpy(arp, "");
		return;
	}
	strcpy(arp, "00:00:00:00:00:00");
	while(fgets(buf, 256, fd)){
		char ip_entry[12], hw_address[18];
		sscanf(buf, "%s %*s %*s %s %*s %*s", ip_entry, hw_address);
		if(!strcmp(ip, ip_entry)){
			strcpy(arp, hw_address);
			break;
		}
	}
	fclose(fd);
}

void GetCloneMac()
{
	char macAddr[18];
	arplookup(getenv("REMOTE_ADDR"), macAddr);
	web_debug_header();
	printf("%s", macAddr);
}

void getWPSAuthMode(WSC_CONFIGURED_VALUE *result, char *ret_str)
{
	if(result->WscAuthMode & 0x1)
		strcat(ret_str, "Open");
	if(result->WscAuthMode & 0x2)
		strcat(ret_str, "WPA-PSK");
	if(result->WscAuthMode & 0x4)
		strcat(ret_str, "Shared");
	if(result->WscAuthMode & 0x8)
		strcat(ret_str, "WPA");
	if(result->WscAuthMode & 0x10)
		strcat(ret_str, "WPA2");
	if(result->WscAuthMode & 0x20)
		strcat(ret_str, "WPA2-PSK");
}

void getWPSEncrypType(WSC_CONFIGURED_VALUE *result, char *ret_str)
{
	if(result->WscEncrypType & 0x1)
		strcat(ret_str, "None");
	if(result->WscEncrypType & 0x2)
		strcat(ret_str, "WEP");
	if(result->WscEncrypType & 0x4)
		strcat(ret_str, "TKIP");
	if(result->WscEncrypType & 0x8)
		strcat(ret_str, "AES");
}

/*
 *  * these definitions are from rt2860v2 driver include/wsc.h
 *   */
char *getWscStatusStr(int status)
{
	switch(status){
	case 0:
		return "Not used";
	case 1:
		return "Idle";
	case 2:
		return "WSC Fail(Ignore this if Intel/Marvell registrar used)";
	case 3:
		return "Start WSC Process";
	case 4:
		return "Received EAPOL-Start";
	case 5:
		return "Sending EAP-Req(ID)";
	case 6:
		return "Receive EAP-Rsp(ID)";
	case 7:
		return "Receive EAP-Req with wrong WSC SMI Vendor Id";
	case 8:
		return "Receive EAPReq with wrong WSC Vendor Type";
	case 9:
		return "Sending EAP-Req(WSC_START)";
	case 10:
		return "Send M1";
	case 11:
		return "Received M1";
	case 12:
		return "Send M2";
	case 13:
		return "Received M2";
	case 14:
		return "Received M2D";
	case 15:
		return "Send M3";
	case 16:
		return "Received M3";
	case 17:
		return "Send M4";
	case 18:
		return "Received M4";
	case 19:
		return "Send M5";
	case 20:
		return "Received M5";
	case 21:
		return "Send M6";
	case 22:
		return "Received M6";
	case 23:
		return "Send M7";
	case 24:
		return "Received M7";
	case 25:
		return "Send M8";
	case 26:
		return "Received M8";
	case 27:
		return "Processing EAP Response (ACK)";
	case 28:
		return "Processing EAP Request (Done)";
	case 29:
		return "Processing EAP Response (Done)";
	case 30:
		return "Sending EAP-Fail";
	case 31:
		return "WSC_ERROR_HASH_FAIL";
	case 32:
		return "WSC_ERROR_HMAC_FAIL";
	case 33:
		return "WSC_ERROR_DEV_PWD_AUTH_FAIL";
	case 34:
		return "Configured";
	case 35:
		return "SCAN AP";
	case 36:
		return "EAPOL START SENT";
	case 37:
		return "WSC_EAP_RSP_DONE_SENT";
	case 38:
		return "WAIT PINCODE";
	case 39:
		return "WSC_START_ASSOC";
	case 0x101:
		return "PBC:TOO MANY AP";
	case 0x102:
		return "PBC:NO AP";
	case 0x103:
		return "EAP_FAIL_RECEIVED";
	case 0x104:
		return "EAP_NONCE_MISMATCH";
	case 0x105:
		return "EAP_INVALID_DATA";
	case 0x106:
		return "PASSWORD_MISMATCH";
	case 0x107:
		return "EAP_REQ_WRONG_SMI";
	case 0x108:
		return "EAP_REQ_WRONG_VENDOR_TYPE";
	case 0x109:
		return "PBC_SESSION_OVERLAP";
	default:
		return "Unknown";
	}
}

#if defined (RT2860_WSCV2_SUPPORT) || defined (RTDEV_WSCV2_SUPPORT)
int ap_oid_query_info(unsigned long OidQueryCode, int socket_id, char *DeviceName, void *ptr, unsigned long PtrLength)
{
	struct iwreq wrq;

	strcpy(wrq.ifr_name, DeviceName);
	wrq.u.data.length = PtrLength;
	wrq.u.data.pointer = (caddr_t) ptr;
	wrq.u.data.flags = OidQueryCode;

	return (ioctl(socket_id, RT_PRIV_IOCTL, &wrq));
}

static void config_acl()
{
	int i, socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	char temp[18];
	PRT_802_11_ACL alc_list = malloc(sizeof(RT_802_11_ACL));

	ap_oid_query_info(OID_802_11_ACL_LIST, socket_id, "ra0", alc_list, sizeof(RT_802_11_ACL));
	for (i=0; i < alc_list->Num; i++)
	{
		sprintf(temp, "%02X:%02X:%02X:%02X:%02X:%02X",
				alc_list->Entry[i].Addr[0], alc_list->Entry[i].Addr[1], alc_list->Entry[i].Addr[2],
				alc_list->Entry[i].Addr[3], alc_list->Entry[i].Addr[4], alc_list->Entry[i].Addr[5]);
		nvram_init(RT2860_NVRAM);
		set_nth_value_flash(RT2860_NVRAM, i, "AccessControlList0", temp);
		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
	}
	close(socket_id);
	free(alc_list);
}
#endif

void UpdateAPWPS(char *if_name)
{
	int i, status, WscResult = 0;
	char tmp_str[128];
	WSC_CONFIGURED_VALUE result;

	web_debug_header();
	getCurrentWscProfile(if_name, &result, sizeof(WSC_CONFIGURED_VALUE));

	//1. WPSConfigured
	printf("%d\t", result.WscConfigured);

	//2. WPSSSID
	if(strchr(result.WscSsid, '\n')){
		printf("Invalid SSID character: new line\t");
	}else{
		printf("%s\t", result.WscSsid);
	}

	//3. WPSAuthMode
	tmp_str[0] = '\0';
	getWPSAuthMode(&result, tmp_str);
	printf("%s\t", tmp_str);

	//4. EncrypType
	tmp_str[0] = '\0';
	getWPSEncrypType(&result, tmp_str);
	printf("%s\t", tmp_str);

	//5. DefaultKeyIdx
	printf("%d\t", result.DefaultKeyIdx);

	//6. Key
	for(i=0; i<64 && i<strlen(result.WscWPAKey); i++){                // WPA key default length is 64 (defined & hardcode in driver)
		if(i!=0 && !(i % 32))
			printf("<br>");
		printf("%c", result.WscWPAKey[i]);
	}
	printf("\t");

	//7. WSC Status
	status = getWscStatus(if_name);
	//DBG_MSG("%d", status);
	printf("%s\t", getWscStatusStr(status));

	//8. WSC Result
	if (status == 0x2 || status == 0x109)
		WscResult = -1;
	else if (status == 0x34)
		WscResult = 1;
	printf("%d\t", WscResult);

	//9. WSC Status Index
	printf("%d", status);
#if defined (RT2860_WSCV2_SUPPORT)
	if ((status == 0x34) && !strcmp(if_name, "ra0"))
		config_acl();
#endif
#if defined (RTDEV_WSCV2_SUPPORT)
	if ((status == 0x34) && !strcmp(if_name, "rai0"))
		config_acl();
#endif
}

void UpdateWapiCert(void)
{
	nvram_init(WAPI_NVRAM);
	web_debug_header();
	printf("%s\t", nvram_bufget(WAPI_NVRAM, "ASCertFile"));
	printf("%s\t", nvram_bufget(WAPI_NVRAM, "UserCertFile"));
	nvram_close(WAPI_NVRAM);
}

void StartSniffer(char *channel)
{
	char path[512];
	FILE *pp = popen("cat /proc/mounts | grep mmc", "r");

	memset(path, 0, 512);
	while (EOF != fscanf(pp, "%*s %s %*s %*s %*s %*s\n", path)) {
		if (strlen(path) != 0) {
			do_system("ifconfig ra0 down; ifconfig ra0 up");
			do_system("iwconfig ra0 channel %s;iwconfig ra0 mode monitor", channel);
			do_system("tcpdump -i ra0 -s 0 -w %s/wifi_sniffer.pcap &", path);
			break;
		}
	}
}
void StopSniffer(void)
{
	do_system("killall -9 tcpdump");
	do_system("ifconfig ra0 down; ifconfig ra0 up");
}

void DeleteAccessPolicyList(char *input)
{
	int nvram;
	int mbssid, aplist_num;
	char str[32], apl[64*20];
	const char *tmp, tok[8];

	get_nth_value(1, input, '&', tok, 8);
	if (!strcmp(tok, "rai0"))
		nvram = RTDEV_NVRAM;
	else
		nvram = RT2860_NVRAM;

	get_nth_value(2, input, '&', tok, 8);
	sscanf(tok, "%d,%d", &mbssid, &aplist_num);
	sprintf(str, "AccessControlList%d", mbssid);
	if(!(tmp = nvram_bufget(nvram, str))) {
		DBG_MSG("%s is empty!!!", tmp);
		return;
	}
	strcpy(apl, tmp);

	delete_nth_value(&aplist_num, 1, apl, ';');

	nvram_set(nvram, str, apl);
	sprintf(str, "%d", mbssid);
	nvram_set(nvram, "CurrentSSIDIndx", str);

	return;
}

#define TOKEN_LEN 20
int main(void) 
{
	char *value, *inStr;
	long inLen;
	char token[TOKEN_LEN];

	inLen = strtol(getenv("CONTENT_LENGTH"), NULL, 10) + 1;
	if (inLen <= 1) {
		DBG_MSG("get no data!");
		return -1;
	}
	inStr = malloc(inLen);
	memset(inStr, 0, inLen);
	fgets(inStr, inLen, stdin);
	//DBG_MSG("%s", inStr);
	get_nth_value(0, inStr, '&', token, TOKEN_LEN);
	if (!strcmp(token, "clone")) {
		GetCloneMac();
	} else if (!strcmp(token, "delAccessPolicyList")) {
		DeleteAccessPolicyList(inStr);
	} else if (!strcmp(token, "sniffer_start")) {
		get_nth_value(1, inStr, '&', token, TOKEN_LEN);
		StartSniffer(token);
	} else if (!strcmp(token, "sniffer_stop")) {
		StopSniffer();
	} else if (!strcmp(token, "updateAPWPSStatus")) {
		get_nth_value(1, inStr, '&', token, TOKEN_LEN);
		UpdateAPWPS(token);
	} else if (!strcmp(token, "updateWapiCert")) {
		UpdateWapiCert();
	}
	free(inStr);

	return 0;
}
