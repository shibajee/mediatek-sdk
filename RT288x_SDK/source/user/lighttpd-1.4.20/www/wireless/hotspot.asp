<html><head><!-- Copyright (c), Ralink Technology Corporation All Rights Reserved. -->

<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<script type="text/javascript" src="/lang/b28n.js"></script>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css"><title>Ralink Wireless Hotspot Settings</title>

<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("wireless");

var selectedIF;
var BssidNum = "<% getCfgZero(1, "BssidNum"); %>";
var PhyMode  = "<% getCfgZero(1, "WirelessMode"); %>";
var AUTH = new Array();
var WPS = new Array();
var HS = new Array();
var INTERNET = new Array();
var HESSID = new Array();
var ROAMING = new Array();
var NAI = new Array();
var REALM1 = new Array();
var REALM2 = new Array();
var REALM3 = new Array();
var REALM4 = new Array();
var EAP1 = new Array();
var EAP2 = new Array();
var EAP3 = new Array();
var EAP4 = new Array();
var LINKSTATUS = new Array();
var CAPACITY = new Array();
var DLSPEED = new Array();
var ULSPEED = new Array();
var DLLOAD = new Array();
var ULLOAD = new Array();
var LMD = new Array();
var PLMN = new Array();
var MCC1 = new Array();
var MCC2 = new Array();
var MCC3 = new Array();
var MCC4 = new Array();
var MCC5 = new Array();
var MCC6 = new Array();
var MNC1 = new Array();
var MNC2 = new Array();
var MNC3 = new Array();
var MNC4 = new Array();
var MNC5 = new Array();
var MNC6 = new Array();
var PROXYARP = new Array();
var DGAF = new Array();
var L2F = new Array();
var ICMP = new Array();

function QuerryValue() 
{
	var str;
		
	str = "<% getCfgGeneral(1, "HS_enabled"); %>";
	HS = str.split(";");
	str = "<% getCfgGeneral(1, "AuthMode"); %>";
	AUTH = str.split(";");
	str = "<% getCfgGeneral(1, "WscModeOption"); %>";
	WPS = str.split(";");
	str = "<% getCfgGeneral(1, "HS_internet"); %>";	
	INTERNET = str.split(";");
	str = "<% getCfgGeneral(1, "HS_hessid"); %>";	
	HESSID = str.split(";");
	str = "<% getCfgGeneral(1, "HS_roaming_consortium_oi"); %>";	
	ROAMING = str.split(";");
	str = "<% getCfgGeneral(1, "HS_nai"); %>";	
	NAI = str.split(";");
	str = "<% getCfgGeneral(1, "HS_nai1_realm"); %>";	
	REALM1 = str.split(";");
	str = "<% getCfgGeneral(1, "HS_nai2_realm"); %>";	
	REALM2 = str.split(";");
	str = "<% getCfgGeneral(1, "HS_nai3_realm"); %>";	
	REALM3 = str.split(";");
	str = "<% getCfgGeneral(1, "HS_nai4_realm"); %>";	
	REALM4 = str.split(";");
	str = "<% getCfgGeneral(1, "HS_nai1_eap_method"); %>";	
	EAP1 = str.split(";");
	str = "<% getCfgGeneral(1, "HS_nai2_eap_method"); %>";	
	EAP2 = str.split(";");
	str = "<% getCfgGeneral(1, "HS_nai3_eap_method"); %>";	
	EAP3 = str.split(";");
	str = "<% getCfgGeneral(1, "HS_nai4_eap_method"); %>";	
	EAP4 = str.split(";");
	str = "<% getCfgGeneral(1, "HS_link_status"); %>";	
	LINKSTATUS = str.split(";");
	str = "<% getCfgGeneral(1, "HS_capacity"); %>";	
	CAPACITY = str.split(";");
	str = "<% getCfgGeneral(1, "HS_dl_speed"); %>";	
	DLSPEED = str.split(";");
	str = "<% getCfgGeneral(1, "HS_ul_speed"); %>";	
	ULSPEED = str.split(";");
	str = "<% getCfgGeneral(1, "HS_dl_load"); %>";	
	DLLOAD = str.split(";");
	str = "<% getCfgGeneral(1, "HS_ul_load"); %>";	
	ULLOAD = str.split(";");
	str = "<% getCfgGeneral(1, "HS_lmd"); %>";	
	LMD = str.split(";");
	str = "<% getCfgGeneral(1, "HS_plmn"); %>";	
	PLMN = str.split(";");
	str = "<% getCfgGeneral(1, "HS_plmn1_mcc"); %>";	
	MCC1 = str.split(";");
	str = "<% getCfgGeneral(1, "HS_plmn2_mcc"); %>";	
	MCC2 = str.split(";");
	str = "<% getCfgGeneral(1, "HS_plmn3_mcc"); %>";	
	MCC3 = str.split(";");
	str = "<% getCfgGeneral(1, "HS_plmn4_mcc"); %>";	
	MCC4 = str.split(";");
	str = "<% getCfgGeneral(1, "HS_plmn5_mcc"); %>";	
	MCC5 = str.split(";");
	str = "<% getCfgGeneral(1, "HS_plmn6_mcc"); %>";	
	MCC6 = str.split(";");
	str = "<% getCfgGeneral(1, "HS_plmn1_mnc"); %>";	
	MNC1 = str.split(";");
	str = "<% getCfgGeneral(1, "HS_plmn2_mnc"); %>";	
	MNC2 = str.split(";");
	str = "<% getCfgGeneral(1, "HS_plmn3_mnc"); %>";	
	MNC3 = str.split(";");
	str = "<% getCfgGeneral(1, "HS_plmn4_mnc"); %>";	
	MNC4 = str.split(";");
	str = "<% getCfgGeneral(1, "HS_plmn5_mnc"); %>";	
	MNC5 = str.split(";");
	str = "<% getCfgGeneral(1, "HS_plmn6_mnc"); %>";	
	MNC6 = str.split(";");
	str = "<% getCfgGeneral(1, "HS_proxy_arp"); %>";	
	PROXYARP = str.split(";");
	str = "<% getCfgGeneral(1, "HS_dgaf_disabled"); %>";	
	DGAF = str.split(";");
	str = "<% getCfgGeneral(1, "HS_l2_filter"); %>";	
	L2F = str.split(";");
	str = "<% getCfgGeneral(1, "HS_icmpv4_deny"); %>";	
	ICMP = str.split(";");
}

function checkMac(str){
	var len = str.length;
	if(len!=17)
		return false;

	for (var i=0; i<str.length; i++) {
		if((i%3) == 2){
			if(str.charAt(i) == ':')
				continue;
		}else{
			if (    (str.charAt(i) >= '0' && str.charAt(i) <= '9') ||
					(str.charAt(i) >= 'a' && str.charAt(i) <= 'f') ||
					(str.charAt(i) >= 'A' && str.charAt(i) <= 'F') )
			continue;
		}
		return false;
	}
	return true;
}

function checkInjection(str)
{
	var len = str.length;
	for (var i=0; i<str.length; i++) {
		if (str.charAt(i) == ';') {
			return false;
		} else {
			continue;
		}
	}
    return true;
}

function style_display_on()
{
	if (window.ActiveXObject) { // IE
		return "block";
	}
	else if (window.XMLHttpRequest) { // Mozilla, Safari,...
		return "table";
	}
}

function checkData()
{
	var idx;
	var realm_data;
	var mcc_data;
	var mnc_data;
	var input;

	if (document.hotspot_form.hessid.value != "") {
		if (document.hotspot_form.hessid.value != "bssid" && checkMac(document.hotspot_form.hessid.value) == false) {
			alert("please input correct HESSID");
			document.hotspot_form.hessid.select();
			return false;
		} else if (checkInjection(document.hotspot_form.hessid.value) == false) {
			alert("Don't include \";\" character");
			return false;
		}
	}
	if (checkInjection(document.hotspot_form.roaming_consortium_oi.value) == false) {
		alert("Don't include \";\" character");
		return false;
	}
	if (document.hotspot_form.nai_enabled[0].checked == true) {
		input = 0;
		for (idx=1; idx<4; idx++) {
			realm_data = eval("document.hotspot_form.realm"+idx);
			if (realm_data.value != "") {
				if (checkInjection(realm_data.value) == false) {
					alert("Don't include \";\" character");
					return false;
				}
				input = 1;
			}
		}
		if (input == 0) {
			alert("please input at least one set of NAI configuration!");
			document.hotspot_form.realm1.select();
			return false;
		}
	}
	if (document.hotspot_form.plmn_enabled[0].checked == true) {
		input = 0;
		for (idx=1; idx<7; idx++) {
			mcc_data = eval("document.hotspot_form.mcc"+idx);
			mnc_data = eval("document.hotspot_form.mnc"+idx);
			if (mcc_data.value != "" || mnc_data.value != "") {
				if (mcc_data.value == "" || checkInjection(mcc_data.value) == false) {
					alert("please input correct MCC and don't include \";\" character");
					mcc_data.select();
					return false;
				}
				if (mnc_data.value == "" || checkInjection(mnc_data.value) == false) {
					alert("please input correct MNC and don't include \";\" character");
					mnc_data.select();
					return false;
				}
				input = 1;
			}
		}
		if (input == 0) {
			alert("please input at least one set of 3GPP configuration!");
			document.hotspot_form.mcc1.select();
			return false;
		}
	}

	return true;
}

function submit_apply()
{
	if (1*WPS[selectedIF] > 0) {
		alert("please turn off ra"+selectedIF+"\'s WPS!"); 
		return false;
	}

	if (AUTH[selectedIF] != "WPA2") {
		alert("please change ra"+selectedIF+"\'s security to WPA2-Enterprise!"); 
		return false;
	}

	if (checkData() == true) {
		document.hotspot_form.submit();
	}
}

function initAll()
{
	var idx;

	alert("1.Please make sure security mode at WPA2 Enterprise before Hotspot configuration.\n2.Hotspot doesn't support WPS");
	QuerryValue();
	for (idx=0; idx<BssidNum; idx++) {
		document.hotspot_form.wifiIndex.options[idx] = new Option("ra"+idx, idx);
	}
	/*
	for (idx=0; idx<BssidNum; idx++) {
		alert("ra"+idx+": AUTH-"+AUTH[idx]+"; INTERNET-"+INTERNET[idx]+"; HESSID-"+HESSID[idx]+"; REALM1-"+REALM1[idx]+"; REALM2-"+REALM2[idx]+"; REALM3-"+REALM3[idx]+"; REALM4-"+REALM4[idx]+"; EAP1-"+EAP1[idx]+"; EAP2-"+EAP2[idx]+"; EAP3-"+EAP3[idx]+"; EAP4-"+EAP4[idx]+"; LINKSTATUS-"+LINKSTATUS[idx]+"; CAPACITY-"+CAPACITY[idx]+"; DLSPEED-"+DLSPEED[idx]+"; ULSPEED-"+ULSPEED[idx]+"; DLLOAD-"+DLLOAD[idx]+"; ULLOAD-"+ULLOAD[idx]+"; LMD-"+LMD[idx]+"; MCC1-"+MCC1[idx]+"; MCC2-"+MCC2[idx]+"; MCC3-"+MCC3[idx]+"; MCC4-"+MCC4[idx]+"; MCC5-"+MCC5[idx]+"; MCC6-"+MCC6[idx]+"; MNC1-"+MNC1[idx]+"; MNC2-"+MNC2[idx]+"; MNC3-"+MNC3[idx]+"; MNC4-"+MNC4[idx]+"; MNC5-"+MNC5[idx]+"; MNC6-"+MNC6[idx]+"; PROXYARP-"+PROXYARP[idx]+"; DGAF-"+DGAF[idx]+"; L2F-"+L2F[idx]+"; ICMP-"+ICMP[idx]);
	}
	*/
	document.hotspot_form.wifiIndex.selectedIndex = 0;
	switch_if();
}

function switch_l2f(on)
{
	if (on == 1) {
		document.hotspot_form.l2_filter[0].checked = true;
		document.hotspot_form.icmpv4_deny[0].disabled = false;
		document.hotspot_form.icmpv4_deny[1].disabled = false;
		if (ICMP[selectedIF] == "1") {
			document.hotspot_form.icmpv4_deny[0].checked = true;
		} else {
			document.hotspot_form.icmpv4_deny[1].checked = true;
		}
	} else {
		document.hotspot_form.l2_filter[1].checked = true;
		document.hotspot_form.icmpv4_deny[0].disabled = true;
		document.hotspot_form.icmpv4_deny[1].disabled = true;
	}
}

function UpdateBasic()
{
	if (INTERNET[selectedIF] == "1") {
		document.hotspot_form.internet[0].checked = true;
	} else {
		document.hotspot_form.internet[1].checked = true;
	}
	if (HESSID[selectedIF] != "" && HESSID[selectedIF]) {
		document.hotspot_form.hessid.value = HESSID[selectedIF];
	} else {
		document.hotspot_form.hessid.value = "";
	}
	if (ROAMING[selectedIF] != "" && ROAMING[selectedIF]) {
		document.hotspot_form.roaming_consortium_oi.value = ROAMING[selectedIF];
	} else {
		document.hotspot_form.roaming_consortium_oi.value = "";
	}
	if (PROXYARP[selectedIF] == "1") {
		document.hotspot_form.proxy_arp[0].checked = true;
	} else {
		document.hotspot_form.proxy_arp[1].checked = true;
	}
	if (DGAF[selectedIF] == "1") {
		document.hotspot_form.dgaf_disabled[0].checked = true;
	} else {
		document.hotspot_form.dgaf_disabled[1].checked = true;
	}
	if (L2F[selectedIF] == "1") {
		switch_l2f(1);
	} else {
		switch_l2f(0);
	}
}

function swtich_nai(on)
{
	var idx;
	var realm_value;
	var eap_value;
	var realm;
	var eap;

	if (on == 1) {
		document.hotspot_form.nai_enabled[0].checked = true;
		document.getElementById("div_nai_table").style.visibility = "visible";
		document.getElementById("div_nai_table").style.display = style_display_on();
	} else {
		document.hotspot_form.nai_enabled[1].checked = true;
		document.getElementById("div_nai_table").style.visibility = "hidden";
		document.getElementById("div_nai_table").style.display = "none";
		for (idx=1; idx<5; idx++) {
			realm = eval("document.hotspot_form.realm"+idx);
			eap = eval("document.hotspot_form.eap_method"+idx);
			realm.disabled = true;
			eap.disabled = true;
		}
		return;
	}

	for (idx=1; idx<5; idx++) {
		realm_value = eval("REALM"+idx);
		eap_value = eval("EAP"+idx);
		realm = eval("document.hotspot_form.realm"+idx);
		eap = eval("document.hotspot_form.eap_method"+idx);
		realm.disabled = false;
		eap.disabled = false;

		if (realm_value[selectedIF] == "" || !realm_value[selectedIF]) {
			realm.value = "";
			continue;
		}
		realm.value = realm_value[selectedIF];
		if (eap_value[selectedIF] == "eap-ttls/eap-tls") {
			eap.selectedIndex = 2;
		} else if (eap_value[selectedIF] == "eap-tls") {
			eap.selectedIndex = 1;
		} else {
			eap.selectedIndex = 0;
		}
	}
}

function swtich_plmn(on)
{
	var idx;
	var mcc_value;
	var mnc_value;
	var mcc;
	var mnc;

	if (on == 1) {
		document.hotspot_form.plmn_enabled[0].checked = true;
		document.getElementById("div_plmn_table").style.visibility = "visible";
		document.getElementById("div_plmn_table").style.display = style_display_on();
	} else {
		document.hotspot_form.plmn_enabled[1].checked = true;
		document.getElementById("div_plmn_table").style.visibility = "hidden";
		document.getElementById("div_plmn_table").style.display = "none";
		for (idx=1; idx<7; idx++) {
			mcc = eval("document.hotspot_form.mcc"+idx);
			mnc = eval("document.hotspot_form.mnc"+idx);
			mcc.disabled = true;
			mnc.disabled = true;
		}
		return;
	}

	for (idx=1; idx<7; idx++) {
		mcc_value = eval("MCC"+idx);
		mnc_value = eval("MNC"+idx);
		mcc = eval("document.hotspot_form.mcc"+idx);
		mnc = eval("document.hotspot_form.mnc"+idx);
		mcc.disabled = false;
		mnc.disabled = false;

		if (mcc_value[selectedIF] == "" || !mcc_value[selectedIF]) {
			mcc.value = "";
			mnc.value = "";
			continue;
		}
		mcc.value = mcc_value[selectedIF];
		mnc.value = mnc_value[selectedIF];
	}
}

function UpdateWANMetrics()
{
	if (LINKSTATUS[selectedIF] != "" && LINKSTATUS[selectedIF]) {
		document.getElementById("link_status").innerHTML = LINKSTATUS[selectedIF];
	} else {
		document.getElementById("link_status").innerHTML = "0";
	}
	if (CAPACITY[selectedIF] != "" && CAPACITY[selectedIF]) {
		document.getElementById("at_capacity").innerHTML = CAPACITY[selectedIF];
	} else {
		document.getElementById("at_capacity").innerHTML = "0";
	}
	if (DLSPEED[selectedIF] != "" && DLSPEED[selectedIF]) {
		document.getElementById("dl_speed").innerHTML = DLSPEED[selectedIF];
	} else {
		document.getElementById("dl_speed").innerHTML = "0";
	}
	if (ULSPEED[selectedIF] != "" && ULSPEED[selectedIF]) {
		document.getElementById("ul_speed").innerHTML = ULSPEED[selectedIF];
	} else {
		document.getElementById("ul_speed").innerHTML = "0";
	}
	if (DLLOAD[selectedIF] != "" && DLLOAD[selectedIF]) {
		document.getElementById("dl_load").innerHTML = DLLOAD[selectedIF];
	} else {
		document.getElementById("dl_load").innerHTML = "0";
	}
	if (ULLOAD[selectedIF] != "" && ULLOAD[selectedIF]) {
		document.getElementById("up_load").innerHTML = ULLOAD[selectedIF];
	} else {
		document.getElementById("up_load").innerHTML = "0";
	}
	if (LMD[selectedIF] != "" && LMD[selectedIF]) {
		document.getElementById("lmd").innerHTML = LMD[selectedIF];
	} else {
		document.getElementById("lmd").innerHTML = "0";
	}
}

function switch_hs(on)
{
	var idx;
	var mcc;
	var mnc;
	if (on == 1) {
		document.hotspot_form.hs_enabled[0].checked = true;
		document.hotspot_form.internet[0].disabled = false;
		document.hotspot_form.internet[1].disabled = false;
		document.hotspot_form.hessid.disabled = false;
		document.hotspot_form.roaming_consortium_oi.disabled = false;
		document.hotspot_form.proxy_arp[0].disabled = false;
		document.hotspot_form.proxy_arp[1].disabled = false;
		document.hotspot_form.dgaf_disabled[0].disabled = false;
		document.hotspot_form.dgaf_disabled[1].disabled = false;
		document.hotspot_form.l2_filter[0].disabled = false;
		document.hotspot_form.l2_filter[1].disabled = false;
		document.hotspot_form.icmpv4_deny[0].disabled = false;
		document.hotspot_form.icmpv4_deny[1].disabled = false;
		document.hotspot_form.nai_enabled[0].disabled = false;
		document.hotspot_form.nai_enabled[1].disabled = false;
		document.hotspot_form.plmn_enabled[0].disabled = false;
		document.hotspot_form.plmn_enabled[1].disabled = false;
		UpdateBasic();
		if (NAI[selectedIF] == "1") {
			swtich_nai(1);
		} else {
			swtich_nai(0);
		}
		if (PLMN[selectedIF] == "1") {
			swtich_plmn(1);
		} else {
			swtich_plmn(0);
		}
	} else {
		document.hotspot_form.hs_enabled[1].checked = true;
		document.hotspot_form.internet[0].disabled = true;
		document.hotspot_form.internet[1].disabled = true;
		document.hotspot_form.hessid.disabled = true;
		document.hotspot_form.roaming_consortium_oi.disabled = true;
		document.hotspot_form.proxy_arp[0].disabled = true;
		document.hotspot_form.proxy_arp[1].disabled = true;
		document.hotspot_form.dgaf_disabled[0].disabled = true;
		document.hotspot_form.dgaf_disabled[1].disabled = true;
		document.hotspot_form.l2_filter[0].disabled = true;
		document.hotspot_form.l2_filter[1].disabled = true;
		document.hotspot_form.icmpv4_deny[0].disabled = true;
		document.hotspot_form.icmpv4_deny[1].disabled = true;
		document.hotspot_form.nai_enabled[0].disabled = true;
		document.hotspot_form.nai_enabled[1].disabled = true;
		document.hotspot_form.plmn_enabled[0].disabled = true;
		document.hotspot_form.plmn_enabled[1].disabled = true;
		swtich_nai(0);
		swtich_plmn(0);
	}
}

function switch_if()
{
	selectedIF = document.hotspot_form.wifiIndex.selectedIndex;
	if (HS[selectedIF] == "1") {
		switch_hs(1);
	} else {
		switch_hs(0);
	}
	UpdateWANMetrics();
}
</script>
</head>
<body onload="initAll()">
<table class="body"><tbody><tr><td>

<h1 id="securityTitle">Wireless Hotspot Settings </h1>

<form method="post" name="hotspot_form" action="/goform/setHotspot">
<input type="hidden" name="selectedif">
<!-- Basic Settings -->
<table border="1" bordercolor="#9babbd" cellpadding="3" cellspacing="1" hspace="2" vspace="2">
  <tr>
    <td class="title" colspan="2" id="basic">Basic Settings</td>
  </tr>
  <tr>
    <td class="head" id="hsWifiIFChoice">WiFi Interface choice</td>
    <td>
      <select name="wifiIndex" size="1" onchange="switch_if()">
			<!-- ....Javascript will update options.... -->
      </select>
    </td>
  </tr>
  <tr>
    <td class="head" id="hsEnabled">Hotspot: </td>
    <td>
      <input type="radio" name="hs_enabled" value="1" onClick="switch_hs(1)">Enabled&nbsp;
      <input type="radio" name="hs_enabled" value="0" onClick="switch_hs(0)">Disabled
    </td>
  </tr>
  <tr>
    <td class="head" id="Internet">Internet Available:</td>
    <td>
      <input type="radio" name="internet" value="1">Enabled&nbsp;
      <input type="radio" name="internet" value="0">Disabled
    </td>
  </tr>
  <tr>
    <td class="head" id="Hessid">HESSID:</td>
    <td><input type="text" name="hessid" id="hessid" size="20" maxlength="20"><br /><font color="#808080">ex:00:0C:43:38:83:00 or bssid<br />(bssid=AP own mac address)</font></td>
  </tr>
  <tr>
    <td class="head" id="Roaming">Roaming Consortium:</td>
    <td><input type="text" name="roaming_consortium_oi" id="roaming_consortium_oi" size="30" maxlength="30"><br /><font color="#808080">ex:50-6F-9A,00-1B-C5-04-BD</font></td>
  </tr>
  <tr>
    <td class="head" id="ProxyARP">Proxy ARP serive:</td>
    <td>
      <input type="radio" name="proxy_arp" value="1">Enabled&nbsp;
      <input type="radio" name="proxy_arp" value="0">Disabled
    </td>
  </tr>
  <tr>
    <td class="head" id="DGAF">Deactivation of Broadcast/Multicast Functionality:</td>
    <td>
      <input type="radio" name="dgaf_disabled" value="1">Enabled&nbsp;
      <input type="radio" name="dgaf_disabled" value="0">Disabled
    </td>
  </tr>
  <tr>
    <td class="head" id="L2F">L2 Traffic Inspection and Filtering:</td>
    <td>
      <input type="radio" name="l2_filter" value="1" onClick="switch_l2f(1)">Enabled&nbsp;
      <input type="radio" name="l2_filter" value="0" onClick="switch_l2f(0)">Disabled
      <br />ICMPv4 Deny:
      <input type="radio" name="icmpv4_deny" value="1">Enabled&nbsp;
      <input type="radio" name="icmpv4_deny" value="0">Disabled
    </td>
  </tr>
</table>
<br />

<!-- NAI Realm List -->
<table border="1" bordercolor="#9babbd" cellpadding="3" cellspacing="1" hspace="2" vspace="2">
  <tr>
    <td class="head">NAI Realm List: 
      <input type="radio" value="1" name="nai_enabled" onClick="swtich_nai(1)">Enabled&nbsp;
      <input type="radio" value="0" name="nai_enabled" onClick="swtich_nai(0)">Disabled
<script language="JavaScript" type="text/javascript">
	var nai_idx;
	document.write("<table border=\"1\" bordercolor=\"#9babbd\" cellpadding=\"3\" cellspacing=\"1\" hspace=\"2\" vspace=\"2\" id=\"div_nai_table\">");
	for (nai_idx=1; nai_idx<5; nai_idx++) {
		document.write("<tr><td class=\"title\" colspan=\"2\" id=\"naiSet"+nai_idx+"\">NAI Settings</td></tr>");
    		document.write("<tr><td class=\"head\" id=\"naiRealm\">Realm:</td>");
    		document.write("<td><input type=\"text\" name=\"realm"+nai_idx+"\" id=\"realm"+nai_idx+"\" size=\"32\" maxlength=\"64\"></td></tr>");
		document.write("<tr><td class=\"head\" id=\"naiEAP\">EAP Method:</td>");
		document.write("<td><select name=\"eap_method"+nai_idx+"\" size=\"1\">");
		document.write("<option value=\"eap-ttls\">TTLS</option>");
		document.write("<option value=\"eap-tls\">TLS</option>");
		document.write("<option value=\"eap-ttls/eap-tls\">TTLS/TLS</option>");
		document.write("</select></td></tr>");
	}
	document.write("</table>");
</script>
    </td>
  </tr>
</table>
<br />

<!-- 3GPP Cellular Network information -->
<table border="1" bordercolor="#9babbd" cellpadding="3" cellspacing="1" hspace="2" vspace="2">
  <tr>
    <td class="head">3GPP Cellular Network information:
  	<input type="radio" value="1" name="plmn_enabled" onClick="swtich_plmn(1)">Enabled&nbsp;
	<input type="radio" value="0" name="plmn_enabled" onClick="swtich_plmn(0)">Disabled
<script language="JavaScript" type="text/javascript">
	var plmn_idx;
	document.write("<table border=\"1\" bordercolor=\"#9babbd\" cellpadding=\"3\" cellspacing=\"1\" hspace=\"2\" vspace=\"2\" id=\"div_plmn_table\">");
	for (plmn_idx=1; plmn_idx<7; plmn_idx++) {
		document.write("<tr><td class=\"title\" colspan=\"2\" id=\"plmnSet"+plmn_idx+"\">3GPP Settings</td></tr>");
    		document.write("<tr><td class=\"head\" id=\"plmnMcc\">MCC:</td>");
    		document.write("<td><input type=\"text\" name=\"mcc"+plmn_idx+"\" id=\"mcc"+plmn_idx+"\" size=\"5\" maxlength=\"5\"></td></tr>");
		document.write("<tr><td class=\"head\" id=\"plmnMnc\">MNC:</td>");
    		document.write("<td><input type=\"text\" name=\"mnc"+plmn_idx+"\" id=\"mnc"+plmn_idx+"\" size=\"5\" maxlength=\"5\"></td></tr>");
	}
	document.write("</table>");
</script>
    </td
  ></tr>
</table>
<br />

<!-- WAN Metrics -->
<table border="1" bordercolor="#9babbd" cellpadding="3" cellspacing="1" hspace="2" vspace="2" width="540">
  <tr>
    <td class="title" colspan="2" id="wanMetrics">WAN Metrics</td>
  </tr>
  <tr>
    <td class="head" id="wanLinkStatus">Link Status:</td>
    <td><span id="link_status"> </span></td>
  </tr>
  <tr>
    <td class="head" id="wanCapacity">Capacity:</td>
    <td><span id="at_capacity"> </span></td>
  </tr>
  <tr>
    <td class="head" id="wanDLSpeed">Downlink WAN Speed (kbps):</td>
    <td><span id="dl_speed"> </span></td>
  </tr>
  <tr>
    <td class="head" id="wanULSpeed">Uplink WAN Speed (kbps):</td>
    <td><span id="ul_speed"> </span></td>
  </tr>
  <tr>
    <td class="head" id="wanDLLoad">Downlink WAN Loading (%):</td>
    <td><span id="dl_load"> </span></td>
  </tr>
  <tr>
    <td class="head" id="wanULLoad">Uplink WAN Loading (%):</td>
    <td><span id="up_load"> </span></td>
  </tr>
  <tr>
    <td class="head" id="wanLMD">Load Measure:</td>
    <td><span id="lmd"> </span></td>
  </tr>
</table>
<hr />

<br />
<table border="0" cellpadding="2" cellspacing="1" width="540">
  <tr align="center">
    <td>
      <input style="width: 120px;" value="Apply" id="hsApply" onclick="submit_apply()" type="button"> &nbsp; &nbsp;
      <input style="width: 120px;" value="Cancel" id="hsCancel" type="reset" onClick="window.location.reload()" >
    </td>
  </tr>
</table>
</form>

</td></tr></tbody></table>
</body></html>
 
