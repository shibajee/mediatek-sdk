#include "utils.h"
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <linux/wireless.h>

#include "oid.h"

static void revise_mbss_value(int nvram, int old_num, int new_num)
{
	/* {{{ The parameters that support multiple BSSID is listed as followed,
	 1.) SSID,                 char SSID[33];
	 2.) AuthMode,             char AuthMode[14];
	 3.) EncrypType,           char EncrypType[8];
	 4.) WPAPSK,               char WPAPSK[65];
	 5.) DefaultKeyID,         int  DefaultKeyID;
	 6.) Key1Type,             int  Key1Type;
	 7.) Key1Str,              char Key1Str[27];
	 8.) Key2Type,             int  Key2Type;
	 9.) Key2Str,              char Key2Str[27];
	10.) Key3Type,            int  Key3Type;
	11.) Key3Str,             char Key3Str[27];
	12.) Key4Type,            int  Key4Type;
	13.) Key4Str,             char Key4Str[27];
	14.) AccessPolicy,
	15.) AccessControlList,
	16.) NoForwarding,
	17.) IEEE8021X,           int  IEEE8021X;
	18.) TxRate,              int  TxRate;
	19.) HideSSID,            int  HideSSID;
	20.) PreAuth,             int  PreAuth;
	21.) WmmCapable
	                          int  SecurityMode;
	                          char VlanName[20];
	                          int  VlanId;
	                          int  VlanPriority;
	   }}} */
	char new_value[264], *p;
	const char *old_value;
	int i;

#define MBSS_INIT(field, default_value) \
	do { \
		old_value = nvram_bufget(nvram, #field); \
		snprintf(new_value, 264, "%s", old_value); \
		p = new_value + strlen(old_value); \
		for (i = old_num; i < new_num; i++) { \
			snprintf(p, 264 - (p - new_value), ";%s", default_value); \
			p += 1 + strlen(default_value); \
		} \
		nvram_bufset(nvram, #field, new_value); \
	} while (0)

#define MBSS_REMOVE(field) \
	do { \
		old_value = nvram_bufget(nvram, #field); \
		snprintf(new_value, 264, "%s", old_value); \
		p = new_value; \
		for (i = 0; i < new_num; i++) { \
			if (0 == i) \
			p = strchr(p, ';'); \
			else \
			p = strchr(p+1, ';'); \
			if (NULL == p) \
			break; \
		} \
		if (p) \
		*p = '\0'; \
		nvram_bufset(nvram, #field, new_value); \
	} while (0)

	if (new_num > old_num) {
		//MBSS_INIT(SSID, "ssid");
		MBSS_INIT(AuthMode, "OPEN");
		MBSS_INIT(EncrypType, "NONE");
		//MBSS_INIT(WPAPSK, "12345678");
		MBSS_INIT(DefaultKeyID, "1");
		MBSS_INIT(Key1Type, "0");
		//MBSS_INIT(Key1Str, "");
		MBSS_INIT(Key2Type, "0");
		//MBSS_INIT(Key2Str, "");
		MBSS_INIT(Key3Type, "0");
		//MBSS_INIT(Key3Str, "");
		MBSS_INIT(Key4Type, "0");
		//MBSS_INIT(Key4Str, "");
		/*      MBSS_INIT(AccessPolicy0, "0");
		        MBSS_INIT(AccessControlList0, "");
		        MBSS_INIT(AccessPolicy1, "0");
		        MBSS_INIT(AccessControlList1, "");
		        MBSS_INIT(AccessPolicy2, "0");
		        MBSS_INIT(AccessControlList2, "");
		        MBSS_INIT(AccessPolicy3, "0");
		        MBSS_INIT(AccessControlList3, ""); */
		MBSS_INIT(NoForwarding, "0");
		MBSS_INIT(NoForwardingBTNBSSID, "0");
		MBSS_INIT(IEEE8021X, "0");
		MBSS_INIT(RADIUS_Server, "0");
		MBSS_INIT(RADIUS_Port, "1812");
		MBSS_INIT(TxRate, "0");
		//MBSS_INIT(HideSSID, "0");
		MBSS_INIT(PreAuth, "0");
		MBSS_INIT(WmmCapable, "1");
		for (i = old_num + 1; i <= new_num; i++) {
			nvram_bufset(nvram, racat("WPAPSK", i), "12345678");
			nvram_bufset(nvram, racat("Key1Str", i), "");
			nvram_bufset(nvram, racat("Key2Str", i), "");
			nvram_bufset(nvram, racat("Key3Str", i), "");
			nvram_bufset(nvram, racat("Key4Str", i), "");
			// The index of AccessPolicy & AccessControlList starts at 0.
			nvram_bufset(nvram, racat("AccessPolicy", i-1), "0");
			nvram_bufset(nvram, racat("AccessControlList", i-1), "");
		}
	}
	else if (new_num < old_num) {
		//MBSS_REMOVE(SSID);
		MBSS_REMOVE(AuthMode);
		MBSS_REMOVE(EncrypType);
		//MBSS_REMOVE(WPAPSK);
		MBSS_REMOVE(DefaultKeyID);
		MBSS_REMOVE(Key1Type);
		//MBSS_REMOVE(Key1Str);
		MBSS_REMOVE(Key2Type);
		//MBSS_REMOVE(Key2Str);
		MBSS_REMOVE(Key2Type);
		//MBSS_REMOVE(Key2Str);
		MBSS_REMOVE(Key3Type);
		//MBSS_REMOVE(Key3Str);
		MBSS_REMOVE(Key4Type);
		//MBSS_REMOVE(Key4Str);
		/*      MBSS_REMOVE(AccessPolicy0);
		        MBSS_REMOVE(AccessControlList0);
		        MBSS_REMOVE(AccessPolicy1);
		        MBSS_REMOVE(AccessControlList1);
		        MBSS_REMOVE(AccessPolicy2);
		        MBSS_REMOVE(AccessControlList2);
		        MBSS_REMOVE(AccessPolicy3);
		        MBSS_REMOVE(AccessControlList3); */
		MBSS_REMOVE(NoForwarding);
		MBSS_REMOVE(NoForwardingBTNBSSID);
		MBSS_REMOVE(IEEE8021X);
		MBSS_REMOVE(RADIUS_Server);
		MBSS_REMOVE(RADIUS_Port);
		MBSS_REMOVE(TxRate);
		MBSS_REMOVE(HideSSID);
		MBSS_REMOVE(PreAuth);
		MBSS_REMOVE(WmmCapable);
		for (i = new_num + 1; i <= old_num; i++) {
			nvram_bufset(nvram, racat("SSID", i), "");
			nvram_bufset(nvram, racat("WPAPSK", i), "");
			nvram_bufset(nvram, racat("Key1Str", i), "");
			nvram_bufset(nvram, racat("Key2Str", i), "");
			nvram_bufset(nvram, racat("Key3Str", i), "");
			nvram_bufset(nvram, racat("Key4Str", i), "");
			// The index of AccessPolicy & AccessControlList starts at 0.
			nvram_bufset(nvram, racat("AccessPolicy", i-1), "0");
			nvram_bufset(nvram, racat("AccessControlList", i-1), "");
		}
	}
}

void set_wifi_basic(int nvram, char *input)
{
	char *value;
	char *wirelessmode, *hssid, *isolated_ssid;
#if defined (RT2860_NEW_MBSS_SUPPORT) || defined (RTDEV_NEW_MBSS_SUPPORT)
	char    hidden_ssid[32], noforwarding[32];
#else
	char    hidden_ssid[16], noforwarding[16];
#endif
	char *sz11aChannel, *sz11bChannel, *sz11gChannel;
	char *abg_rate, *n_mcs;
	int i, new_bssid_num = 0, old_bssid_num, is_n = 0, is_ac = 0;
	char bssid_num[3];
	int max_mbssid = 1;
	char ifprefix[4];

#if ! defined CONFIG_FIRST_IF_NONE 
	if (nvram == RT2860_NVRAM) {
		strcpy(ifprefix, "ra");	
#if defined (RT2860_NEW_MBSS_SUPPORT)
		max_mbssid = 16;
#elif defined (RT2860_MBSS_SUPPORT)
		max_mbssid = 8;
#endif
	}
#endif
#if ! defined CONFIG_SECOND_IF_NONE 
	if (nvram == RTDEV_NVRAM) {
		strcpy(ifprefix, "rai");	
#if defined (RTDEV_NEW_MBSS_SUPPORT)
		max_mbssid = 16;
#elif defined (RTDEV_MBSS_SUPPORT)
		max_mbssid = 8;
#endif
	}
#endif

	value = web_get("radiohiddenButton", input, 0);
	if (!strncmp(value, "0", 2)) {
		do_system("iwpriv %s0 set RadioOn=0", ifprefix);
		nvram_set(nvram, "RadioOff", "1");
		nvram_commit(nvram);
		web_redirect(getenv("HTTP_REFERER"));
		return;
	}
	else if (!strncmp(value, "1", 2)) {
		do_system("iwpriv %s0 set RadioOn=1", ifprefix);
		nvram_set(nvram, "RadioOff", "0");
		nvram_commit(nvram);
		web_redirect(getenv("HTTP_REFERER"));
		return;
	}
	value = web_get("wifihiddenButton", input, 0);
	if (!strncmp(value, "0", 2)) {
#if defined (RT2860_MBSS_SUPPORT) || defined (RTDEV_MBSS_SUPPORT)
		int bssid_num = strtol(nvram_bufget(nvram, "BssidNum"), NULL, 10);
		do {
			bssid_num--;
			do_system("ifconfig %s%d down", ifprefix, bssid_num);
		} while (bssid_num > 0);
#else
		do_system("ifconfig %s0 down", ifprefix);
#endif
		//do_system("rmmod rt2860v2_ap");
		nvram_set(nvram, "WiFiOff", "1");
		nvram_commit(nvram);
		web_redirect(getenv("HTTP_REFERER"));
		return;
	}
	else if (!strncmp(value, "1", 2)) {
		do_system("insmod -q rt2860v2_ap");
#if defined (RT2860_MBSS_SUPPORT) || defined (RTDEV_MBSS_SUPPORT)
		int idx = 0;
		int bssid_num = strtol(nvram_bufget(nvram, "BssidNum"), NULL, 10);
		do {
			do_system("ifconfig %s%d up", ifprefix, idx);
			idx++;
		} while (idx < bssid_num);
#else
		do_system("ifconfig %s0 up", ifprefix);
#endif
		nvram_set(nvram, "WiFiOff", "0");
		nvram_commit(nvram);
		web_redirect(getenv("HTTP_REFERER"));
		return;
	}

	wirelessmode = hssid = isolated_ssid = NULL;
	sz11aChannel = sz11bChannel = sz11gChannel = NULL;
	abg_rate = n_mcs = NULL;
	web_debug_header();
	//fetch from web input
	hssid = strdup(web_get("hssid", input, 1));
	isolated_ssid = strdup(web_get("isolated_ssid", input, 1));
	for (i = 0, value = web_get(racat("mssid_", i), input, 1); 
	     i < max_mbssid && strlen(value) > 0; 
	     i++, value = web_get(racat("mssid_", i), input, 1)) {
		new_bssid_num++;
		nvram_bufset(nvram, racat("SSID", new_bssid_num), value);
		if (new_bssid_num > 1) {
			sprintf(hidden_ssid, "%s;", hidden_ssid);
			sprintf(noforwarding, "%s;", hidden_ssid);
		} else {
			sprintf(hidden_ssid, "");
			sprintf(noforwarding, "");
		}
		if (strchr(hssid, i+0x30) != NULL)
			sprintf(hidden_ssid, "%s%s", hidden_ssid, "1");
		else
			sprintf(hidden_ssid, "%s%s", hidden_ssid, "0");
		if (strchr(isolated_ssid, i+0x30) != NULL)
			sprintf(noforwarding, "%s%s", hidden_ssid, "1");
		else
			sprintf(noforwarding, "%s%s", hidden_ssid, "0");
	}
	if (new_bssid_num < 1) {
		DBG_MSG("'SSID' should not be empty!");
		goto leave;
	}
	wirelessmode = strdup(web_get("wirelessmode", input, 1)); //9: bgn mode
	WebTF(input, "mbssidapisolated", nvram, "NoForwardingBTNBSSID", 1);
	sz11aChannel = strdup(web_get("sz11aChannel", input, 1));
	sz11bChannel = strdup(web_get("sz11bChannel", input, 1));
	sz11gChannel = strdup(web_get("sz11gChannel", input, 1));
	abg_rate = strdup(web_get("abg_rate", input, 1));
	WebTF(input, "tx_stream", nvram, "HT_TxStream", 1);
	WebTF(input, "rx_stream", nvram, "HT_RxStream", 1);

	old_bssid_num = strtol(nvram_bufget(nvram, "BssidNum"), NULL, 10);

	nvram_bufset(nvram, "WirelessMode", wirelessmode);
	//BasicRate: bg,bgn,n:15, b:3; g,gn:351
	if (!strncmp(wirelessmode, "4", 2) || !strncmp(wirelessmode, "7", 2)) //g, gn
		nvram_bufset(nvram, "BasicRate", "351");
	else if (!strncmp(wirelessmode, "1", 2)) //b
		nvram_bufset(nvram, "BasicRate", "3");
	else //bg,bgn,n
		nvram_bufset(nvram, "BasicRate", "15");

	if (!strncmp(wirelessmode, "8", 2) || !strncmp(wirelessmode, "9", 2) ||
	    !strncmp(wirelessmode, "6", 2) || !strncmp(wirelessmode, "11", 3))
		is_n = 1;
#if defined (RT2860_AC_SUPPORT) || defined (RTDEV_AC_SUPPORT)
	else if (!strncmp(wirelessmode, "14", 2) || !strncmp(wirelessmode, "15", 2))
		is_ac = 1;
#endif

	//#WPS
	if (!strcmp(hidden_ssid, "1")) {
		nvram_bufset(nvram, "WscModeOption", "0");
		do_system("miniupnpd.sh init");
		do_system("route delete 239.255.255.250");
	} else {
		const char *wordlist= nvram_bufget(nvram, "WscModeOption");
		if(wordlist){
			if (strcmp(wordlist, "0"))
				do_system("iwpriv %s0 set WscConfStatus=1", ifprefix);
			nvram_bufset(nvram, "WscConfigured", "1");
		}
	}

	sprintf(bssid_num, "%d", new_bssid_num);
	nvram_bufset(nvram, "BssidNum", bssid_num);
#if ! defined CONFIG_FIRST_IF_NONE 
#if defined (RT2860_NEW_MBSS_SUPPORT)
	if ((nvram == RT2860_NVRAM) && 
	 	(new_bssid_num < 1 || new_bssid_num > 16)) {
#else
	if ((nvram == RT2860_NVRAM) && 
		(new_bssid_num < 1 || new_bssid_num > 8)) {
#endif
		DBG_MSG("'bssid_num' %d is out of range!", new_bssid_num);
		goto leave;
	}
#endif
#if ! defined CONFIG_SECOND_IF_NONE 
#if defined (RTDEV_NEW_MBSS_SUPPORT)
	if ((nvram == RTDEV_NVRAM) && 
		(new_bssid_num < 1 || new_bssid_num > 16)) {
#else
	if ((nvram == RTDEV_NVRAM) && 
		(new_bssid_num < 1 || new_bssid_num > 8)) {
#endif
		DBG_MSG("'bssid_num' %d is out of range!", new_bssid_num);
		goto leave;
	}
#endif
	revise_mbss_value(nvram, old_bssid_num, new_bssid_num);

	//Broadcast SSID
	nvram_bufset(nvram, "HideSSID", hidden_ssid);

	// NoForwarding and NoForwardingBTNBSSID
	nvram_bufset(nvram, "NoForwarding", noforwarding);

	//11abg Channel or AutoSelect
	if ((0 == strlen(sz11aChannel)) && (0 == strlen(sz11bChannel)) &&
	    (0 == strlen(sz11gChannel))) {
		DBG_MSG("'Channel' should not be empty!");
		goto leave;
	}
	if (!strncmp(sz11aChannel, "0", 2) || !strncmp(sz11bChannel, "0", 2) ||
	    !strncmp(sz11gChannel, "0", 2))
		nvram_bufset(nvram, "AutoChannelSelect", "1");
	else
		nvram_bufset(nvram, "AutoChannelSelect", "0");
	if (0 != strlen(sz11aChannel))
	{
		nvram_bufset(nvram, "Channel", sz11aChannel);
		do_system("iwpriv %s0 set Channel=%s", ifprefix, sz11aChannel);
	}
	if (0 != strlen(sz11bChannel))
	{
		nvram_bufset(nvram, "Channel", sz11bChannel);
		do_system("iwpriv %s0 set Channel=%s", ifprefix, sz11bChannel);
	}
	if (0 != strlen(sz11gChannel))
	{
		nvram_bufset(nvram, "Channel", sz11gChannel);
		do_system("iwpriv %s0 set Channel=%s", ifprefix, sz11gChannel);
	}
	sleep(1);

	//Rate for a, b, g
	if (strncmp(abg_rate, "", 1)) {
		int rate = strtol(abg_rate, NULL, 10);
		if (!strncmp(wirelessmode, "0", 2) || !strncmp(wirelessmode, "2", 2) || !strncmp(wirelessmode, "4", 2)) {
			if (rate == 1 || rate == 2 || rate == 5 || rate == 11)
				nvram_bufset(nvram, "FixedTxMode", "CCK");
			else
				nvram_bufset(nvram, "FixedTxMode", "OFDM");

			if (rate == 1)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "0");
			else if (rate == 2)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "1");
			else if (rate == 5)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "2");
			else if (rate == 6)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "0");
			else if (rate == 9)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "1");
			else if (rate == 11)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "3");
			else if (rate == 12)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "2");
			else if (rate == 18)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "3");
			else if (rate == 24)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "4");
			else if (rate == 36)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "5");
			else if (rate == 48)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "6");
			else if (rate == 54)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "7");
			else
				SetAllValues(new_bssid_num, nvram, HT_MCS, "33");
		}
		else if (!strncmp(wirelessmode, "1", 2)) {
			nvram_bufset(nvram, "FixedTxMode", "CCK");
			if (rate == 1)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "0");
			else if (rate == 2)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "1");
			else if (rate == 5)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "2");
			else if (rate == 11)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "3");
		}
		else //shall not happen
			DBG_MSG("wrong configurations from web UI");
	}
	else
		nvram_bufset(nvram, "FixedTxMode", "HT");

	//HT_OpMode, HT_BW, HT_GI, HT_MCS, HT_RDG, HT_EXTCHA, HT_AMSDU, HT_TxStream, HT_RxStream
	if (is_n | is_ac) {
		WebTF(input, "n_mode", nvram, "HT_OpMode", 1);
		WebTF(input, "n_bandwidth", nvram, "HT_BW", 1);
		WebTF(input, "n_gi", nvram, "HT_GI", 1);
		n_mcs = strdup(web_get("n_mcs", input, 1));
		SetAllValues(new_bssid_num, nvram, HT_MCS, n_mcs);
		WebTF(input, "n_rdg", nvram, "HT_RDG", 1);
		WebTF(input, "n_extcha", nvram, "HT_EXTCHA", 1);
		WebTF(input, "n_stbc", nvram, "HT_STBC", 1);
		WebTF(input, "n_amsdu", nvram, "HT_AMSDU", 1);
		WebTF(input, "n_autoba", nvram, "HT_AutoBA", 1);
		WebTF(input, "n_badecline", nvram, "HT_BADecline", 1);
		WebTF(input, "n_disallow_tkip", nvram, "HT_DisallowTKIP", 1);
		WebTF(input, "n_2040_coexit", nvram, "HT_BSSCoexistence", 1);
		WebTF(input, "n_ldpc", nvram, "HT_LDPC", 1);
	}
#if defined (RT2860_AC_SUPPORT) || defined (RTDEV_AC_SUPPORT)
	if (is_ac) {
		WebTF(input, "vht_bandwidth", nvram, "VHT_BW", 1);
		WebTF(input, "vht_stbc", nvram, "VHT_STBC", 1);
		WebTF(input, "vht_sgi", nvram, "VHT_SGI", 1);
		WebTF(input, "vht_bw_signal", nvram, "VHT_BW_SIGNAL", 1);
		WebTF(input, "vht_ldpc", nvram, "VHT_LDPC", 1);
	}
#endif

	if (!strncmp(wirelessmode, "6", 2) || !strncmp(wirelessmode, "11", 2))
	{
		int mbssid_num = strtol(nvram_bufget(nvram, "BssidNum"), NULL, 10);
		int i = 0;

		while (i < mbssid_num)
		{
			char tmp[128];
			char *EncrypType = (char *) nvram_bufget(nvram, "EncrypType");
			get_nth_value(i, EncrypType, ';', tmp, 128);
			if (!strcmp(tmp, "NONE") || !strcmp(tmp, "AES"))
			{
				memset(tmp, 0, 128);
				char *IEEE8021X = (char *) nvram_bufget(nvram, "IEEE8021X");
				get_nth_value(i, IEEE8021X, ';', tmp, 128);
				if (!strcmp(tmp, "0"))
				{
					i++;
					continue;
				}
			}
			set_nth_value_flash(nvram, i, "AuthMode", "OPEN");
			set_nth_value_flash(nvram, i, "EncrypType", "NONE");
			set_nth_value_flash(nvram, i, "IEEE8021X", "0");
			i++;
		}
	}
	nvram_commit(nvram);
	/*  this is a workaround:
	 *  when AP is built as kernel
	 *  if more ssids are created, driver won't exe RT28xx_MBSS_Init again
	 *  therefore, we reboot to make it available
	 *  (PS. CONFIG_MIPS would be "y")
	 */
	if (new_bssid_num != old_bssid_num)
		do_system("reboot");

	do_system("init_system restart");
leave:
	free_all(8, hssid, isolated_ssid, wirelessmode, 
		    sz11aChannel, sz11bChannel, sz11gChannel,
		    abg_rate, n_mcs);
}

void set_wifi_advance(int nvram, char *input)
{
	char 	*wmm_capable, *countrycode, *countryregion;
	char 	wmm_enable[16];
#if defined CONFIG_RA_CLASSIFIER_MODULE && defined CONFIG_RT2860V2_AP_VIDEO_TURBINE && defined (RT2860_TXBF_SUPPORT)
	char  	*rvt = NULL;
	int     oldrvt;
	const char 	*oldrx, *oldtx;
#endif // getRVTBuilt //
#if defined (RT2860_TXBF_SUPPORT) || defined (RTDEV_TXBF_SUPPORT)
	char  	*txbf = NULL;
#endif // TXBF //
	int     i, ssid_num, wlan_mode;

	wmm_capable = countrycode = NULL;
	web_debug_header();
	//fetch from web input
	WebTF(input, "bg_protection", nvram, "BGProtection", 1);
	// WebTF(input, "basic_rate", nvram, "BasicRate", 1);
	WebTF(input, "beacon", nvram, "BeaconPeriod", 1);
	WebTF(input, "dtim", nvram, "DtimPeriod", 1);
	WebTF(input, "fragment", nvram, "FragThreshold", 1);
	WebTF(input, "rts", nvram, "RTSThreshold", 1);
	WebTF(input, "tx_power", nvram, "TxPower", 1);
	WebTF(input, "short_preamble", nvram, "TxPreamble", 1);
	WebTF(input, "short_slot", nvram, "ShortSlot", 1);
	WebTF(input, "tx_burst", nvram, "TxBurst", 1);
	WebTF(input, "pkt_aggregate", nvram, "PktAggregate", 1);
	WebTF(input, "ieee_80211h", nvram, "IEEE80211H", 1);
	wmm_capable = strdup(web_get("wmm_capable", input, 1));
	WebTF(input, "apsd_capable", nvram, "APSDCapable", 1);
	WebTF(input, "dls_capable", nvram, "DLSCapable", 1);
	countrycode = strdup(web_get("country_code", input, 1));
	countryregion = strdup(web_get("country_region", input, 1));
	WebTF(input, "m2u_enable", nvram, "M2UEnabled", 1);
	WebTF(input, "rd_region", nvram, "RDRegion", 1);
	WebTF(input, "carrier_detect", nvram, "CarrierDetect", 1);
	WebTF(input, "ieee_80211h", nvram, "IEEE80211H", 1);
	WebTF(input, "ieee_80211h", nvram, "IEEE80211H", 1);
#ifdef CONFIG_RT2860V2_AUTO_PROVISION
	WebTF(input, "auto_provision", nvram, "AutoProvisionEn", 1);
#endif
#if defined CONFIG_RA_CLASSIFIER_MODULE && defined CONFIG_RT2860V2_AP_VIDEO_TURBINE && defined (RT2860_TXBF_SUPPORT)
	rvt = strdup(web_get("rvt", input, 1));
#endif // getRVTBuilt //
#if defined (RT2860_TXBF_SUPPORT) || defined (RTDEV_TXBF_SUPPORT)
	txbf = strdup(web_get("txbf", input, 1));
#endif // TXBF //

	if (NULL != nvram_bufget(nvram, "BssidNum"))
		ssid_num = strtol(nvram_bufget(nvram, "BssidNum"), NULL, 10);
	else
		ssid_num = 1;
	wlan_mode = strtol(nvram_bufget(nvram, "WirelessMode"), NULL, 10);
#if defined CONFIG_RA_CLASSIFIER_MODULE && defined CONFIG_RT2860V2_AP_VIDEO_TURBINE && defined (RT2860_TXBF_SUPPORT)
	if (NULL != nvram_bufget(nvram, "RVT"))
		oldrvt = strtol(nvram_bufget(nvram, "RVT"), NULL, 10);
	else
		oldrvt = 0;
#endif // getRVTBuilt //

	//set to nvram
#if defined CONFIG_RA_CLASSIFIER_MODULE && defined CONFIG_RT2860V2_AP_VIDEO_TURBINE && defined (RT2860_TXBF_SUPPORT)
	if (!strncmp(rvt, "1", 2) && oldrvt != 1) {
		//backup the previous configurations for Rx/TxStream
		oldrx = nvram_bufget(nvram, "HT_RxStream");
		oldtx = nvram_bufget(nvram, "HT_TxStream");
		nvram_bufset(nvram, "HT_RxStream_old", oldrx);
		nvram_bufset(nvram, "HT_TxStream_old", oldtx);

		nvram_bufset(nvram, "RVT", "1");
		nvram_bufset(nvram, "HT_RxStream", "1");
		nvram_bufset(nvram, "HT_TxStream", "1");
		nvram_bufset(nvram, "HT_GI", "0");
		nvram_bufset(nvram, "VideoTurbine", "1");
	}
	else if (!strncmp(rvt, "0", 2) && oldrvt != 0) {
		//restore the previous configurations for Rx/TxStream
		oldrx = nvram_bufget(nvram, "HT_RxStream_old");
		oldtx = nvram_bufget(nvram, "HT_TxStream_old");

		nvram_bufset(nvram, "RVT", "0");
		nvram_bufset(nvram, "HT_RxStream", oldrx);
		nvram_bufset(nvram, "HT_TxStream", oldtx);
		nvram_bufset(nvram, "HT_GI", "1");
		nvram_bufset(nvram, "VideoTurbine", "0");
	}
#endif // getRVTBuilt //
#if defined (RT2860_TXBF_SUPPORT) || defined (RTDEV_TXBF_SUPPORT)
	if (!strncmp(txbf, "3", 2)) {
		nvram_bufset(nvram, "ETxBfEnCond", "1");
		nvram_bufset(nvram, "ETxBfIncapable", "0");
		nvram_bufset(nvram, "ITxBfEn", "1");
	}
	else if (!strncmp(txbf, "2", 2)) {
		nvram_bufset(nvram, "ETxBfEnCond", "1");
		nvram_bufset(nvram, "ETxBfIncapable", "0");
		nvram_bufset(nvram, "ITxBfEn", "0");
	}
	else if (!strncmp(txbf, "1", 2)) {
		nvram_bufset(nvram, "ETxBfEnCond", "0");
		nvram_bufset(nvram, "ETxBfIncapable", "1");
		nvram_bufset(nvram, "ITxBfEn", "1");
	}
	else {
		nvram_bufset(nvram, "ETxBfEnCond", "0");
		nvram_bufset(nvram, "ETxBfIncapable", "1");
		nvram_bufset(nvram, "ITxBfEn", "0");
	}
#endif // TXBF //

	bzero(wmm_enable, sizeof(char)*16);
	for (i = 0; i < ssid_num; i++)
	{
		sprintf(wmm_enable+strlen(wmm_enable), "%d", strtol(wmm_capable, NULL, 10));
		sprintf(wmm_enable+strlen(wmm_enable), "%c", ';');
	}
	wmm_enable[strlen(wmm_enable) - 1] = '\0';
	nvram_bufset(nvram, "WmmCapable", wmm_enable);

	if (!strncmp(wmm_capable, "1", 2)) {
		if (wlan_mode < 5)
			nvram_bufset(nvram, "TxBurst", "0");
	}

#if ! defined CONFIG_EXT_CHANNEL_LIST
	if (wlan_mode == 0 || wlan_mode == 1 || wlan_mode == 4 ||
	    wlan_mode == 9 || wlan_mode == 6) {
		if (!strcmp(countryregion, "8"))
			nvram_bufset(RTDEV_NVRAM, "CountryRegion", "31");
		else if (!strcmp(countryregion, "9"))
			nvram_bufset(RTDEV_NVRAM, "CountryRegion", "32");
		else if (!strcmp(countryregion, "10"))
			nvram_bufset(RTDEV_NVRAM, "CountryRegion", "33");
		else
			nvram_bufset(RTDEV_NVRAM, "CountryRegion", countryregion);
	} else if (wlan_mode == 2 || wlan_mode == 8 || wlan_mode == 11 ||
		   wlan_mode == 14 || wlan_mode == 15) {
		nvram_bufset(RTDEV_NVRAM, "CountryRegionABand", countryregion);
	}
#endif
	nvram_bufset(nvram, "CountryCode", countrycode);

	nvram_commit(nvram);
	do_system("init_system restart");

	free_all(3, wmm_capable, countrycode, countryregion);
#if defined CONFIG_RA_CLASSIFIER_MODULE && defined CONFIG_RT2860V2_AP_VIDEO_TURBINE && defined (RT2860_TXBF_SUPPORT)
	free(rvt);
#endif
#if defined (RT2860_TXBF_SUPPORT) || defined (RTDEV_TXBF_SUPPORT)
	free(txbf);
#endif
}

void set_wifi_wmm(int nvram, char *input)
{
	WebTF(input, "ap_aifsn_all", nvram, "APAifsn", 0);
	WebTF(input, "ap_cwmin_all", nvram, "APCwmin", 0);
	WebTF(input, "ap_cwmax_all", nvram, "APCwmax", 0);
	WebTF(input, "ap_txop_all", nvram, "APTxop", 0);
	WebTF(input, "ap_acm_all", nvram, "APACM", 0);
	WebTF(input, "ap_ackpolicy_all", nvram, "AckPolicy", 0);
	WebTF(input, "sta_aifsn_all", nvram, "BSSAifsn", 0);
	WebTF(input, "sta_cwmin_all", nvram, "BSSCwmin", 0);
	WebTF(input, "sta_cwmax_all", nvram, "BSSCwmax", 0);
	WebTF(input, "sta_txop_all", nvram, "BSSTxop", 0);
	WebTF(input, "sta_acm_all", nvram, "BSSACM", 0);

	 nvram_commit(nvram);
	 do_system("init_system restart");

	 web_back_parentpage();
}

static void clear_radius_setting(int nvram, int mbssid)
{
	set_nth_value_flash(nvram, mbssid, "IEEE8021X", "0");
	set_nth_value_flash(nvram, mbssid, "RADIUS_Server", "0");
	set_nth_value_flash(nvram, mbssid, "RADIUS_Port", "1812");
	set_nth_value_flash(nvram, mbssid, racat("RADIUS_Key", mbssid+1), "ralink");
	// set_nth_value_flash(nvram, mbssid, "session_timeout_interval", "");
}

void conf_wep(char *input, int nvram, int mbssid)
{
	WebNthTF(input, "wep_default_key", nvram, mbssid, "DefaultKeyID", 1);
	WebTF(input, "wep_key_1", nvram, racat("Key1Str", mbssid+1), 1);
	WebTF(input, "wep_key_2", nvram, racat("Key2Str", mbssid+1), 1);
	WebTF(input, "wep_key_3", nvram, racat("Key3Str", mbssid+1), 1);
	WebTF(input, "wep_key_4", nvram, racat("Key4Str", mbssid+1), 1);
	WebNthTF(input, "WEP1Select", nvram, mbssid, "Key1Type", 1);
	WebNthTF(input, "WEP2Select", nvram, mbssid, "Key2Type", 1);
	WebNthTF(input, "WEP3Select", nvram, mbssid, "Key3Type", 1);
	WebNthTF(input, "WEP4Select", nvram, mbssid, "Key4Type", 1);
}

void conf_8021x(char *input, int nvram, int mbssid)
{
	char	*rs_session_to;

	WebNthTF(input, "RadiusServerIP", nvram, mbssid, "RADIUS_Server", 1);
	WebNthTF(input, "RadiusServerPort", nvram, mbssid, "RADIUS_Port", 1);
	WebTF(input, "RadiusServerSecret", nvram, racat("RADIUS_Key", mbssid+1), 1);
	rs_session_to = web_get("RadiusServerSessionTimeout", input, 1);
	if(!strlen(rs_session_to))
		rs_session_to = "0";
	set_nth_value_flash(nvram, mbssid, "session_timeout_interval", rs_session_to);

	update_flash_8021x(nvram);
}

void conf_wapi_general(char *input, int nvram, int mbssid)
{
	char *cipher_str = web_get("cipher", input, 1);

	switch(cipher_str[0]){
	case '0':
		set_nth_value_flash(nvram, mbssid, "EncrypType", "TKIP");
		break;
	case '1':
		set_nth_value_flash(nvram, mbssid, "EncrypType", "AES");
		break;
	case '2':
		set_nth_value_flash(nvram, mbssid, "EncrypType", "TKIPAES");
	}
	set_nth_value_flash(nvram, mbssid, "DefaultKeyID", "2");	// DefaultKeyID is 2
	WebNthTF(input, "keyRenewalInterval", nvram, mbssid, "RekeyInterval", 1);
	set_nth_value_flash(nvram, mbssid, "RekeyMethod", "TIME");
	set_nth_value_flash(nvram, mbssid, "IEEE8021X", "0");
}

int access_policy_handle(char *input, int nvram, int mbssid)
{
	char *apselect, *newap_list;
	char ap_list[2048];

	if(mbssid > 8 || mbssid < 0)
		return -1;

	apselect = web_get(racat("apselect_", mbssid), input, 1);
	if(!strlen(apselect)){
		DBG_MSG("cant find %s", apselect);
		return -1;
	}
	nvram_bufset(nvram, racat("AccessPolicy", mbssid), apselect);

	newap_list = web_get(racat("newap_text_", mbssid), input, 1);
	if(!newap_list)
		return -1;
	if(!strlen(newap_list))
		return 0;
	strcpy(ap_list, nvram_bufget(nvram, racat("AccessControlList", mbssid)));
	if(strlen(ap_list))
		sprintf(ap_list, "%s;%s", ap_list, newap_list);
	else
		sprintf(ap_list, "%s", newap_list);

	//DBG_MSG("%s", ap_list);
	nvram_bufset(nvram, racat("AccessControlList", mbssid), ap_list);
	return 0;
}

void set_wifi_security(int nvram, char *input)
{
	char 	*value;
	int 	mbssid;
	char 	*security_mode;
	char 	ifprefix[4];

#if ! defined CONFIG_FIRST_IF_NONE 
	if (nvram == RT2860_NVRAM) {
		strcpy(ifprefix, "ra");	
	}
#endif
#if ! defined CONFIG_SECOND_IF_NONE 
	if (nvram == RTDEV_NVRAM) {
		strcpy(ifprefix, "rai");	
	}
#endif
	security_mode = NULL;
	web_debug_header();
	WebTF(input, "auto_provision", nvram, "AutoProvisionEn", 1);
	value = web_get("ssidIndex", input, 1);
	if(!strlen(value))
		return;

	mbssid = strtol(value, NULL, 10);
	security_mode = strdup(web_get("security_mode", input, 1));

	//clear Radius settings
	clear_radius_setting(nvram, mbssid);

	if ((!strcmp(security_mode, "OPEN")) || 	// OPEN WEP Mode
	    (!strcmp(security_mode, "SHARED")) ||	// SHARE WEP Mode
	    (!strcmp(security_mode, "WEPAUTO"))) {      // AUTO WEP Mode
		conf_wep(input, nvram,  mbssid);
		set_nth_value_flash(nvram, mbssid, "AuthMode", security_mode);
		set_nth_value_flash(nvram, mbssid, "EncrypType", "WEP");
		nvram_bufset(nvram, "WscModeOption", "0");
		do_system("miniupnpd.sh init");
		do_system("route delete 239.255.255.250");
	} else if ((!strcmp(security_mode, "WPAPSK")) ||		// WPA Personal Mode
		   (!strcmp(security_mode, "WPA2PSK")) ||		// WPA2 Personal Mode
		   (!strcmp(security_mode, "WPAPSKWPA2PSK"))) {      	// WPA/WPA2 mixed Personal Mode
		conf_wapi_general(input, nvram, mbssid);
		set_nth_value_flash(nvram, mbssid, "AuthMode", security_mode);
		WebTF(input, "passphrase", nvram, racat("WPAPSK", mbssid+1), 1);
	} else if (!strcmp(security_mode, "WPA")) {         // WPA Enterprise Mode
		conf_8021x(input, nvram, mbssid);
		conf_wapi_general(input, nvram, mbssid);
		set_nth_value_flash(nvram, mbssid, "AuthMode", security_mode);
	} else if ((!strcmp(security_mode, "WPA2")) ||            // WPA2 Enterprise Mode
		   (!strcmp(security_mode, "WPA1WPA2"))) {        // WPA Enterprise Mode
		conf_8021x(input, nvram, mbssid);
		conf_wapi_general(input, nvram, mbssid);
		set_nth_value_flash(nvram, mbssid, "AuthMode", security_mode);
		WebNthTF(input, "PMKCachePeriod", nvram, mbssid, "PMKCachePeriod", 1);
		WebNthTF(input, "PreAuthentication", nvram, mbssid, "PreAuth", 1);
	} else if (!strcmp(security_mode, "IEEE8021X")) {       // 802.1X WEP Mode
		char *ieee8021x_wep = web_get("ieee8021x_wep", input, 1);

		set_nth_value_flash(nvram, mbssid, "IEEE8021X", "1");
		set_nth_value_flash(nvram, mbssid, "AuthMode", "OPEN");
		if(ieee8021x_wep[0] == '0')
			set_nth_value_flash(nvram, mbssid, "EncrypType", "NONE");
		else
			set_nth_value_flash(nvram, mbssid, "EncrypType", "WEP");
		conf_8021x(input, nvram, mbssid);
	}
#if defined (RT2860_WAPI_SUPPORT) || defined (RTDEV_WAPI_SUPPORT)
	else if (!strcmp(security_mode, "WAICERT")) {           // WAPI-CERT Mode
		set_nth_value_flash(nvram, mbssid, "AuthMode", security_mode);
		set_nth_value_flash(nvram, mbssid, "EncrypType", "SMS4");
		nvram_bufset(nvram, "Wapiifname", "br0");
		WebTF(input, "wapicert_asipaddr", nvram, "WapiAsIpAddr", 1);
		nvram_bufset(nvram, "WapiAsPort", "3810");
		WebTF(input, "wapicert_ascert", nvram, "WapiAsCertPath", 1);
		WebTF(input, "wapicert_usercert", nvram, "WapiUserCertPath", 1);
	} else if (!strcmp(security_mode, "WAIPSK")) {          // WAPI-PSK Mode
		set_nth_value_flash(nvram, mbssid, "AuthMode", security_mode);
		set_nth_value_flash(nvram, mbssid, "EncrypType", "SMS4");
		WebNthTF(input, "wapipsk_keytype", nvram, mbssid, "WapiPskType", 1);
		WebTF(input, "wapipsk_prekey", nvram, racat("WapiPsk", mbssid+1), 1);
	}
#endif
	else {                                                  // Open-None Mode
		set_nth_value_flash(nvram, mbssid, "AuthMode", "OPEN");
		set_nth_value_flash(nvram, mbssid, "EncrypType", "NONE");
	}

	//# Access Policy
	if(access_policy_handle(input, nvram, mbssid) == -1)
		DBG_MSG("error");

	//# WPS
	const char *wordlist= nvram_bufget(nvram, "WscModeOption");
	if (strcmp(wordlist, "0")) {
		if (nvram == RT2860_NVRAM)
			do_system("iwpriv %s%d set WscConfStatus=1", ifprefix, 0);
	}
	nvram_bufset(nvram, "WscConfigured", "1");
	nvram_commit(nvram);
	do_system("init_system restart");
	free(security_mode);
}

#if defined (RT2860_WDS_SUPPORT)
void set_wifi_wds(int nvram, char *input)
{
	char	*wds_mode, *wds_list;

	wds_mode = NULL;
	web_debug_header();
	wds_mode = strdup(web_get("wds_mode", input, 1));
	nvram_bufset(nvram, "WdsEnable", wds_mode);
	if (strncmp(wds_mode, "0", 2)) {
		WebTF(input, "wds_phy_mode", nvram, "WdsPhyMode", 1);
		WebNthTF(input, "wds_encryp_type0", nvram, 0, "WdsEncrypType", 1);
		WebNthTF(input, "wds_encryp_type1", nvram, 1, "WdsEncrypType", 1);
		WebNthTF(input, "wds_encryp_type2", nvram, 2, "WdsEncrypType", 1);
		WebNthTF(input, "wds_encryp_type3", nvram, 3, "WdsEncrypType", 1);
		WebTF(input, "wds_encryp_key0", nvram, "Wds0Key", 1);
		WebTF(input, "wds_encryp_key1", nvram, "Wds1Key", 1);
		WebTF(input, "wds_encryp_key2", nvram, "Wds2Key", 1);
		WebTF(input, "wds_encryp_key3", nvram, "Wds3Key", 1);
		if (!strncmp(wds_mode, "2", 2) || !strncmp(wds_mode, "3", 2)) {
			wds_list = web_get("wds_list", input, 1);
			if (0 != strlen(wds_list))
				nvram_bufset(nvram, "WdsList", wds_list);
		}
	}
	nvram_commit(nvram);
	do_system("init_system restart");
	free(wds_mode);
}
#endif

#if defined (RT2860_APCLI_SUPPORT)
void set_wifi_apclient(int nvram, char *input)
{
	char  	*ssid; 

	web_debug_header();
	ssid = web_get("apcli_ssid", input, 1);
	if (strlen(ssid) == 0) {
		DBG_MSG("SSID is empty");
		return;
	}
	nvram_bufset(nvram, "ApCliSsid", ssid);
	nvram_bufset(RT2860_NVRAM, "ApCliEnable", "1");
	WebTF(input, "apcli_bssid", nvram, "ApCliBssid", 1);
	WebTF(input, "apcli_mode", nvram, "ApCliAuthMode", 1);
	WebTF(input, "apcli_enc", nvram, "ApCliEncrypType", 1);
	WebTF(input, "apcli_wpapsk", nvram, "ApCliWPAPSK", 1);
	WebTF(input, "apcli_default_key", nvram, "ApCliDefaultKeyID", 1);
	WebTF(input, "apcli_key1type", nvram, "ApCliKey1Type", 1);
	WebTF(input, "apcli_key2type", nvram, "ApCliKey2Type", 1);
	WebTF(input, "apcli_key3type", nvram, "ApCliKey3Type", 1);
	WebTF(input, "apcli_key4type", nvram, "ApCliKey4Type", 1);
	WebTF(input, "apcli_key1", nvram, "ApCliKey1Str", 1);
	WebTF(input, "apcli_key2", nvram, "ApCliKey2Str", 1);
	WebTF(input, "apcli_key3", nvram, "ApCliKey3Str", 1);
	WebTF(input, "apcli_key4", nvram, "ApCliKey4Str", 1);
	nvram_commit(RT2860_NVRAM);
	do_system("init_system restart");
}
#endif

void reset_wifi_state(int nvram, char *input)
{
	char 	ifprefix[4];

#if ! defined CONFIG_FIRST_IF_NONE 
	if (nvram == RT2860_NVRAM) {
		strcpy(ifprefix, "ra");	
	}
#endif
#if ! defined CONFIG_SECOND_IF_NONE 
	if (nvram == RTDEV_NVRAM) {
		strcpy(ifprefix, "rai");	
	}
#endif
	do_system("iwpriv %s0 set ResetCounter=0", ifprefix);
	web_redirect(getenv("HTTP_REFERER"));
}

#if ! defined CONFIG_FIRST_IF_NONE || ! defined CONFIG_SECOND_IF_NONE
static void restart_8021x(int nvram)
{
	int i, num, apd_flag = 0;
	const char *auth_mode = (char *) nvram_bufget(nvram, "AuthMode");
	const char *ieee8021x = (char *)nvram_bufget(nvram, "IEEE8021X");
	const char *num_s = nvram_bufget(nvram, "BssidNum");
	if(!num_s)
		return;
	num = strtol(num_s, NULL, 10);

#if ! defined CONFIG_FIRST_IF_NONE 
	if(nvram == RT2860_NVRAM)
		do_system("killall rt2860apd");
#endif
#if ! defined CONFIG_SECOND_IF_NONE
	if(nvram == RTDEV_NVRAM)
		do_system("killall rtinicapd");
#endif

	/*
	 * In fact we only support mbssid[0] to use 802.1x radius settings.
	 */
	for(i=0; i<num; i++){
		char tmp_auth[128];
		if (get_nth_value(i, (char *)auth_mode, ';', tmp_auth, 128) != -1){
			if(!strcmp(tmp_auth, "WPA") || !strcmp(tmp_auth, "WPA2") || !strcmp(tmp_auth, "WPA1WPA2")){
				apd_flag = 1;
				break;
			}
		}

		if (get_nth_value(i, (char *)ieee8021x, ';', tmp_auth, 128) != -1){
			if(!strcmp(tmp_auth, "1")){
				apd_flag = 1;
				break;
			}
		}
	}

	if (apd_flag) {
#if ! defined CONFIG_FIRST_IF_NONE 
		if(nvram == RT2860_NVRAM)
			do_system("rt2860apd");
#endif
#if ! defined CONFIG_SECOND_IF_NONE
		if(nvram == RTDEV_NVRAM)
			do_system("rtinicapd");
#endif
	}
}
#endif

#if defined (RT2860_WSC_SUPPORT) || defined (RTDEV_WSC_SUPPORT)
static void restart_wps(int nvram, int wsc_enable)
{
	char 	ifprefix[4];

#if ! defined CONFIG_FIRST_IF_NONE 
	if (nvram == RT2860_NVRAM) {
		strcpy(ifprefix, "ra");	
	}
#endif
#if ! defined CONFIG_SECOND_IF_NONE 
	if (nvram == RTDEV_NVRAM) {
		strcpy(ifprefix, "rai");	
	}
#endif
	do_system("iwpriv %s0 set WscConfMode=0", ifprefix);
	do_system("route delete 239.255.255.250");
	if (wsc_enable > 0) {
		do_system("iwpriv %s0 set WscConfMode=%d", ifprefix, 7);
		do_system("route add -host 239.255.255.250 dev br0");
	}
	do_system("miniupnpd.sh init");
}

void set_wifi_wpsconf(int nvram, char *input)
{
	int	wsc_enable = 0;
	char 	ifprefix[4];

#if ! defined CONFIG_FIRST_IF_NONE 
	if (nvram == RT2860_NVRAM) {
		strcpy(ifprefix, "ra");	
	}
#endif
#if ! defined CONFIG_SECOND_IF_NONE 
	if (nvram == RTDEV_NVRAM) {
		strcpy(ifprefix, "rai");	
	}
#endif
	wsc_enable = strtol(web_get("WPSEnable", input, 0), NULL, 0);

	//resetTimerAll();
	//g_WscResult = 0;
	//LedReset();

	if (wsc_enable == 0){
		nvram_bufset(nvram, "WscModeOption", "0");
		do_system("iwpriv %s0 set WscConfMode=0", ifprefix);
		do_system("route delete 239.255.255.250");
	}else{
		nvram_bufset(nvram, "WscModeOption", "7");
		do_system("iwpriv %s0 set WscConfMode=7", ifprefix);
		do_system("route add -host 239.255.255.250 dev br0");
	}
	nvram_commit(nvram);
	restart_wps(nvram, wsc_enable);
	web_redirect(getenv("HTTP_REFERER"));
}

static unsigned int get_ap_pin(char *interface)
{
	int socket_id;
	struct iwreq wrq;
	unsigned int data = 0;
	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	strcpy(wrq.ifr_name, interface);
	wrq.u.data.length = sizeof(data);
	wrq.u.data.pointer = (caddr_t) &data;
	wrq.u.data.flags = RT_OID_WSC_PIN_CODE;
	if( ioctl(socket_id, RT_PRIV_IOCTL, &wrq) == -1)
		DBG_MSG("ioctl error");
	close(socket_id);
	return data;
}

void set_wifi_gen_pin(int nvram, char *input)
{
	char new_pin[9];
	char iface[6];

#if ! defined CONFIG_FIRST_IF_NONE 
	if (nvram == RT2860_NVRAM) {
		strcpy(iface, "ra0");	
	}
#endif
#if ! defined CONFIG_SECOND_IF_NONE 
	if (nvram == RTDEV_NVRAM) {
		strcpy(iface, "rai0");	
	}
#endif

	do_system("iwpriv %s  set WscGenPinCode", iface);
	sprintf(new_pin, "%08d", get_ap_pin(iface));
	nvram_bufset(nvram, "WscVendorPinCode", new_pin);
	nvram_commit(nvram);

	if (nvram == RT2860_NVRAM)
		do_system("ralink_init make_wireless_config rt2860");
	else
		do_system("ralink_init make_wireless_config rtdev");
	web_redirect(getenv("HTTP_REFERER"));
}

void set_wifi_wps_oob(int nvram, char *input)
{
	char SSID[64], *mac;
	char iface[6];

#if ! defined CONFIG_FIRST_IF_NONE 
	if (nvram == RT2860_NVRAM) {
		strcpy(iface, "ra0");	
	}
#endif
#if ! defined CONFIG_SECOND_IF_NONE 
	if (nvram == RTDEV_NVRAM) {
		strcpy(iface, "rai0");	
	}
#endif

	//resetTimerAll();
	//g_WscResult = 0;
	//LedReset();
	
	if ((mac = get_macaddr(iface)) != NULL)
		sprintf(SSID, "RalinkInitAP_%s", mac);
	else
		sprintf(SSID, "RalinkInitAP_unknown");
	nvram_bufset(nvram, "SSID1", SSID);

	nvram_bufset(nvram, "WscConfigured", "0");

	set_nth_value_flash(nvram, 0, "AuthMode", "WPA2PSK");
	set_nth_value_flash(nvram, 0, "EncrypType", "AES");
	set_nth_value_flash(nvram, 0, "DefaultKeyID", "2");
	nvram_bufset(nvram, "WPAPSK1", "12345678");

	set_nth_value_flash(nvram, 0, "IEEE8021X", "0");

	/*
	 *   IMPORTANT !!!!!
	 *   5VT doesn't need it cause it will reboot after OOB reset, but RT2880 does.
	 */
	//g_wsc_configured = 0;

	nvram_commit(nvram);

	do_system("iwpriv %s set SSID=%s", iface,  SSID);
	do_system("iwpriv %s set AuthMode=WPA2PSK", iface);
	do_system("iwpriv %s set EncrypType=AES", iface);
	do_system("iwpriv %s set WPAPSK=12345678", iface);
	do_system("iwpriv %s set SSID=%s", iface, SSID);

#if ! defined CONFIG_FIRST_IF_NONE || ! defined CONFIG_SECOND_IF_NONE
	restart_8021x(nvram);
#endif
	restart_wps(nvram, 1);
	do_system("iwpriv %s set WscConfStatus=1", iface);
	web_redirect(getenv("HTTP_REFERER"));
}

static void wps_ap_pbc_start_all(int nvram)
{
	const char *wsc_enable = nvram_bufget(nvram, "WscModeOption");
	char iface[6];

#if ! defined CONFIG_FIRST_IF_NONE 
	if (nvram == RT2860_NVRAM) {
		strcpy(iface, "ra0");	
	}
#endif
#if ! defined CONFIG_SECOND_IF_NONE 
	if (nvram == RTDEV_NVRAM) {
		strcpy(iface, "rai0");	
	}
#endif

	// It is possible user press PBC button when WPS is disabled.
	if (!strcmp(wsc_enable, "0")) {
		DBG_MSG("The PBC button is pressed but WPS is disabled now.");
		return;
	}

	do_system("iwpriv %s set WscMode=2", iface);
	do_system("iwpriv %s set WscGetConf=1", iface);

	//resetTimerAll();
	//setTimer(WPS_AP_CATCH_CONFIGURED_TIMER * 1000, WPSAPTimerHandler);
}

void set_wifi_do_wps(int nvram, char *input)
{
	char *wsc_config_option, *wsc_pin_code_w;
	char iface[6];

#if ! defined CONFIG_FIRST_IF_NONE 
	if (nvram == RT2860_NVRAM) {
		strcpy(iface, "ra0");	
	}
#endif
#if ! defined CONFIG_SECOND_IF_NONE 
	if (nvram == RTDEV_NVRAM) {
		strcpy(iface, "rai0");	
	}
#endif

	wsc_config_option = wsc_pin_code_w = NULL;
	wsc_config_option = web_get("PINPBCRadio", input, 0);

	// reset wsc result indicator
	//g_WscResult = 0;
	//LedReset();
	if (!strcmp(wsc_config_option, "1")) {
		do_system("iwpriv %s set WscMode=1", iface);

		// get pin code
		wsc_pin_code_w = web_get("PIN", input, 0);

		do_system("iwpriv %s set WscPinCode=%s", iface, wsc_pin_code_w);
		do_system("iwpriv %s set WscGetConf=1", iface);
		//DBG_MSG("%s ; %s", iface, wsc_pin_code_w);

		//resetTimerAll();
		//setTimer(WPS_AP_CATCH_CONFIGURED_TIMER * 1000, WPSAPTimerHandler);
	} else if (! strcmp(wsc_config_option, "2")) {
		wps_ap_pbc_start_all(nvram);
	} else {
		DBG_MSG("ignore unknown WSC method: %s", wsc_config_option);
	}
	do_system("killall -SIGXFSZ nvram_daemon");
	web_redirect(getenv("HTTP_REFERER"));
}

void set_wifi_cancel_wps(int nvram, char *input)
{
	char iface[6];

#if ! defined CONFIG_FIRST_IF_NONE 
	if (nvram == RT2860_NVRAM) {
		strcpy(iface, "ra0");	
	}
#endif
#if ! defined CONFIG_SECOND_IF_NONE 
	if (nvram == RTDEV_NVRAM) {
		strcpy(iface, "rai0");	
	}
#endif
	//resetTimerAll();
	do_system("iwpriv %s set WscStop=1", iface);
	do_system("miniupnpd.sh init");
	web_redirect(getenv("HTTP_REFERER"));
}
#endif

#if defined (RT2860_WAPI_SUPPORT) || defined (RTDEV_WAPI_SUPPORT)
static void restart_wapi(void)
{
	const char *auth_mode;

#if defined (RT2860_WAPI_SUPPORT)
	auth_mode = nvram_bufget(RT2860_NVRAM, "AuthMode");
	do_system("killall wapid");
	if (NULL !=strstr(auth_mode, "WAIPSK") || NULL !=strstr(auth_mode, "WAICERT")) {
		do_system("ralink_init gen wapi");
		do_system("wapid");
	}
#endif
#if defined (RTDEV_WAPI_SUPPORT)
	auth_mode = nvram_bufget(RTDEV_NVRAM, "AuthMode");
	if (NULL !=strstr(auth_mode, "WAIPSK") || NULL !=strstr(auth_mode, "WAICERT")) {
		do_system("ralink_init gen wapi");
		do_system("wapid -i rai0");
	}
#endif
}
#endif
#if defined (CONFIG_RALINK_MT7628)	
void obtw(int nvram, char *input)
{
	char *obtw_enable, *CCK_1M, *CCK_5M, *OFDM_6M, *OFDM_12M, *HT20_MCS_0, *HT20_MCS_32, *HT20_MCS_1_2, *HT40_MCS_0, *HT40_MCS_32, *HT40_MCS_1_2;
	web_debug_header();
	obtw_enable = strdup(web_get("obtw_enable", input, 1));
	CCK_1M = strdup(web_get("CCK_1M", input, 1));
	CCK_5M = strdup(web_get("CCK_5M", input, 1));
	OFDM_6M = strdup(web_get("OFDM_6M", input, 1));
	OFDM_12M = strdup(web_get("OFDM_12M", input, 1));
	HT20_MCS_0 = strdup(web_get("HT20_MCS_0", input, 1));
	//HT20_MCS_32 = strdup(web_get("HT20_MCS_32", input, 1));
	HT20_MCS_1_2 = strdup(web_get("HT20_MCS_1_2", input, 1));
	HT40_MCS_0 = strdup(web_get("HT40_MCS_0", input, 1));
	HT40_MCS_32 = strdup(web_get("HT40_MCS_32", input, 1));
	HT40_MCS_1_2 = strdup(web_get("HT40_MCS_1_2", input, 1));
	
	
	
	nvram_bufset(RT2860_NVRAM , "obtw", obtw_enable);
	nvram_bufset(RT2860_NVRAM , "CCK_1M", CCK_1M);
	nvram_bufset(RT2860_NVRAM , "CCK_5M", CCK_5M);
	nvram_bufset(RT2860_NVRAM , "OFDM_6M", OFDM_6M);
	nvram_bufset(RT2860_NVRAM , "OFDM_12M", OFDM_12M);
	nvram_bufset(RT2860_NVRAM , "HT20_MCS_0", HT20_MCS_0);
	//nvram_bufset(RT2860_NVRAM , "HT20_MCS_32", HT20_MCS_32);
	nvram_bufset(RT2860_NVRAM , "HT20_MCS_1_2", HT20_MCS_1_2);
	nvram_bufset(RT2860_NVRAM , "HT40_MCS_0", HT40_MCS_0);
	nvram_bufset(RT2860_NVRAM , "HT40_MCS_32", HT40_MCS_32);
	nvram_bufset(RT2860_NVRAM , "HT40_MCS_1_2", HT40_MCS_1_2);	
	
		if(!strcmp(obtw_enable, "0")){
			do_system("iwpriv ra0 set obtw=disable");
		}else{
			do_system("iwpriv ra0 set obtw=cck1m_%s_cck5m_%s_ofdm6m_%s_ofdm12m_%s_ht20mcs0_%s_ht20mcs1_%s_ht40mcs0_%s_ht40mcs32_%s_ht40mcs1_%s", CCK_1M, CCK_5M, OFDM_6M, OFDM_12M, HT20_MCS_0, HT20_MCS_1_2, HT40_MCS_0, HT40_MCS_32, HT40_MCS_1_2);
		}
	/*
	reip = strdup(web_get("stunIp", input, 1));
	reip = strdup(web_get("stunPort", input, 1));
	*/
	nvram_commit(RT2860_NVRAM );
	free_all(11, obtw_enable, CCK_1M, CCK_5M, OFDM_6M, OFDM_12M, HT20_MCS_0, HT20_MCS_32, HT20_MCS_1_2, HT40_MCS_0, HT40_MCS_32, HT40_MCS_1_2);

}
#endif
#if defined (CONFIG_SECOND_IF_MT7612E)	&& defined(CONFIG_FIRST_IF_MT7628)
void obtw_mt7612(int nvram, char *input)
{
	char *obtw_enable, *OFDM_6M, *OFDM_9M,*OFDM_12M, *OFDM_18M,*HT20_MCS_0, *HT20_MCS_1, *HT20_MCS_2, *HT40_MCS_0, *HT40_MCS_1, *HT40_MCS_2, *VHT80_MCS_0, *VHT80_MCS_1, *VHT80_MCS_2;
	int HT40_MCS_32 = 0;
	web_debug_header();
	obtw_enable = strdup(web_get("obtw_enable", input, 1));

	OFDM_6M = strdup(web_get("OFDM_6M", input, 1));
	OFDM_9M = strdup(web_get("OFDM_9M", input, 1));
	OFDM_12M = strdup(web_get("OFDM_12M", input, 1));
	OFDM_18M = strdup(web_get("OFDM_18M", input, 1));
	HT20_MCS_0 = strdup(web_get("HT20_MCS_0", input, 1));
  HT20_MCS_1 = strdup(web_get("HT20_MCS_1", input, 1));
  HT20_MCS_2 = strdup(web_get("HT20_MCS_2", input, 1));
  
	HT40_MCS_0 = strdup(web_get("HT40_MCS_0", input, 1));
	HT40_MCS_1 = strdup(web_get("HT40_MCS_1", input, 1));
	HT40_MCS_2 = strdup(web_get("HT40_MCS_2", input, 1));
	//HT40_MCS_32 = strdup(web_get("HT40_MCS_32", input, 1));
	
	VHT80_MCS_0 = strdup(web_get("VHT80_MCS_0", input, 1));
	VHT80_MCS_1 = strdup(web_get("VHT80_MCS_1", input, 1));
	VHT80_MCS_2	= strdup(web_get("VHT80_MCS_2", input, 1));
	
	
	nvram_bufset(RT2860_NVRAM , "obtw_mt7612", obtw_enable);
	nvram_bufset(RT2860_NVRAM , "OFDM_6M", OFDM_6M);
	nvram_bufset(RT2860_NVRAM , "OFDM_9M", OFDM_9M);	
	nvram_bufset(RT2860_NVRAM , "OFDM_12M", OFDM_12M);
	nvram_bufset(RT2860_NVRAM , "OFDM_18M", OFDM_18M);
	nvram_bufset(RT2860_NVRAM , "HT20_MCS_0", HT20_MCS_0);
	nvram_bufset(RT2860_NVRAM , "HT20_MCS_1", HT20_MCS_1);
	nvram_bufset(RT2860_NVRAM , "HT20_MCS_2", HT20_MCS_2);	
	nvram_bufset(RT2860_NVRAM , "HT40_MCS_0", HT40_MCS_0);
	//nvram_bufset(RT2860_NVRAM , "HT40_MCS_32", HT40_MCS_32);
	nvram_bufset(RT2860_NVRAM , "HT40_MCS_1", HT40_MCS_1);	
	nvram_bufset(RT2860_NVRAM , "HT40_MCS_2", HT40_MCS_2);
	
	nvram_bufset(RT2860_NVRAM , "VHT80_MCS_0", VHT80_MCS_0);	
	nvram_bufset(RT2860_NVRAM , "VHT80_MCS_1", VHT80_MCS_1);
	nvram_bufset(RT2860_NVRAM , "VHT80_MCS_2", VHT80_MCS_2);	

		if(!strcmp(obtw_enable, "0")){
			do_system("iwpriv rai0 set obtw=disable");
		}else{
			do_system("iwpriv rai0 set obtw=ofdm6m_%s_ofdm9m_%s_ofdm12m_%s_ofdm18m_%s_ht20mcs0_%s_ht20mcs1_%s_ht20mcs2_%s_ht40mcs0_%s_ht40mcs1_%s_ht40mcs2_%s_ht40mcs32_%d_vht80mcs0_%s_vht80mcs1_%s_vht80mcs2_%s", OFDM_6M, OFDM_9M, OFDM_12M, OFDM_18M, HT20_MCS_0, HT20_MCS_1, HT20_MCS_2, HT40_MCS_0, HT40_MCS_1, HT40_MCS_2, HT40_MCS_32, VHT80_MCS_0, VHT80_MCS_1, VHT80_MCS_2);
		}
	/*
	reip = strdup(web_get("stunIp", input, 1));
	reip = strdup(web_get("stunPort", input, 1));
	*/
	nvram_commit(RT2860_NVRAM );
	free_all(14, obtw_enable, OFDM_6M, OFDM_9M, OFDM_12M, OFDM_18M, HT20_MCS_0, HT20_MCS_1, HT20_MCS_2, HT40_MCS_0, HT40_MCS_1, HT40_MCS_2,VHT80_MCS_0, VHT80_MCS_1, VHT80_MCS_2);

}
#endif
int main(int argc, char *argv[]) 
{
	char *page, *inStr;
	int wps_enable, nvram_id;
	long inLen;

	if ((argc > 1) && (!strcmp(argv[1], "init"))) {
#if ! defined CONFIG_FIRST_IF_NONE 
		nvram_init(RT2860_NVRAM);
#if defined (RT2860_WSC_SUPPORT)
		restart_wps(RT2860_NVRAM, 
				strtol(nvram_bufget(RT2860_NVRAM, "WscModeOption"), NULL, 10));
#endif
		update_flash_8021x(RT2860_NVRAM);
		restart_8021x(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
#endif
#if ! defined CONFIG_SECOND_IF_NONE
		nvram_init(RTDEV_NVRAM);
#if defined (RTDEV_WSC_SUPPORT)
		restart_wps(RTDEV_NVRAM, 
				strtol(nvram_bufget(RTDEV_NVRAM, "WscModeOption"), NULL, 10));
#endif
		update_flash_8021x(RT2860_NVRAM);
		restart_8021x(RTDEV_NVRAM);
		nvram_close(RTDEV_NVRAM);
#endif
#if defined (RT2860_WAPI_SUPPORT) || defined (RTDEV_WAPI_SUPPORT)
		restart_wapi();
#endif
		return;
	} else if ((argc > 1) && (!strcmp(argv[1], "wps_pbc"))) {
#if defined (RT2860_WSC_SUPPORT) || defined (RTDEV_WSC_SUPPORT)
		wps_ap_pbc_start_all(strtol(argv[2], NULL, 10));
#endif
		return;
	}
	inLen = strtol(getenv("CONTENT_LENGTH"), NULL, 10) + 1;
	if (inLen <= 1) {
		DBG_MSG("get no data!");
		return;
	}
	inStr = malloc(inLen);
	memset(inStr, 0, inLen);
	fgets(inStr, inLen, stdin);
	//DBG_MSG("%s\n", inStr);
	if ((nvram_id = getNvramIndex(web_get("wlan_conf", inStr, 0))) == -1) {
		DBG_MSG("get config zone fail");
		return;
	}
	nvram_init(nvram_id);
	page = web_get("page", inStr, 0);
	if (!strcmp(page, "basic")) {
		set_wifi_basic(nvram_id, inStr);
	} else if (!strcmp(page, "advance")) {
		set_wifi_advance(nvram_id, inStr);
	} else if (!strcmp(page, "wmm")) {
		set_wifi_wmm(nvram_id, inStr);
	} else if (!strcmp(page, "security")) {
		set_wifi_security(nvram_id, inStr);
#if defined (RT2860_WDS_SUPPORT) || defined (RTDEV_WDS_SUPPORT)
	} else if (!strcmp(page, "wds")) {
		set_wifi_wds(nvram_id, inStr);
#endif
#if defined (RT2860_APCLI_SUPPORT)
	} else if (!strcmp(page, "apclient")) {
		set_wifi_apclient(nvram_id, inStr);
#endif
	} else if (!strcmp(page, "apstatistics")) {
		reset_wifi_state(nvram_id, inStr);
#if defined (RT2860_WSC_SUPPORT) || defined (RTDEV_WSC_SUPPORT)
	} else if (!strcmp(page, "WPSConfig")) {
		set_wifi_wpsconf(nvram_id, inStr);
	} else if (!strcmp(page, "GenPINCode")) {
		set_wifi_gen_pin(nvram_id, inStr);
	} else if (!strcmp(page, "SubmitOOB")) {
		set_wifi_wps_oob(nvram_id, inStr);
	} else if (!strcmp(page, "WPS")) {
		set_wifi_do_wps(nvram_id, inStr);
	} else if (!strcmp(page, "wps_cancel")) {
		set_wifi_cancel_wps(nvram_id, inStr);
#endif	 
#if defined (CONFIG_RALINK_MT7628)	
	}else if (!strcmp(page, "obtw")) {
     obtw(nvram_id, inStr);
#endif
#if defined (CONFIG_SECOND_IF_MT7612E)	&& defined(CONFIG_FIRST_IF_MT7628)	
	}else if (!strcmp(page, "obtw_mt7612")) {
     obtw_mt7612(nvram_id, inStr);
#endif
	}
	free(inStr);
	nvram_close(nvram_id);
	
	return 0;
}
