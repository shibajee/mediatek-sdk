#include "utils.h"
#include <stdlib.h>
#include <net/route.h>

#include "user_conf.h"
#include "busybox_conf.h"
void voip_status(char *input)
{
	char *connect_status;
	connect_status = strdup(web_get("connect_status", input, 1));
	
	if ((strcmp(connect_status, "Connect") == 0))
	{
		printf("Voip connect\n");
		do_system("/etc/init.d/sc connect 1");
	}
	if ((strcmp(connect_status, "Disconnect") == 0))
	{
		printf("Voip disconnect\n");
		do_system("/etc/init.d/sc disconnect 1"); 
	}
		free_all(1, connect_status);
}
void voip_status2(char *input)
{
	char *connect_status2;
	connect_status2 = strdup(web_get("connect_status2", input, 1));
	
	if ((strcmp(connect_status2, "Connect") == 0))
	{
		printf("Voip connect2\n");
		do_system("/etc/init.d/sc connect 2");
	}
	if ((strcmp(connect_status2, "Disconnect") == 0))
	{
		printf("Voip disconnect 2\n");
		do_system("/etc/init.d/sc disconnect 2"); 
	}
	free_all(1, connect_status2);
}
void set_voip(char *input)
{
	char *reIp, *rePort, *sep_reIp, *sep_rePort, *reg_ptime, *proxyIp, *proxyPort, *outboundIp, *outboundPort, *stunIp, *stunPort;
	web_debug_header();
	reIp = strdup(web_get("reIp", input, 1));
	rePort = strdup(web_get("rePort", input, 1));
	sep_reIp = strdup(web_get("sep_reIp", input, 1));
	sep_rePort = strdup(web_get("sep_rePort", input, 1));
	reg_ptime = strdup(web_get("reg_ptime", input, 1));
	proxyIp = strdup(web_get("proxyIp", input, 1));
	proxyPort = strdup(web_get("proxyPort", input, 1));
	outboundIp = strdup(web_get("outboundIp", input, 1));
	outboundPort = strdup(web_get("outboundPort", input, 1));
		stunIp = strdup(web_get("stunIp", input, 1));
	stunPort = strdup(web_get("stunPort", input, 1));
	
	
	
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_SIP_REG_ADDR", reIp);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_SIP_REG_PORT", rePort);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_SIP_SEP_REG_ADDR", sep_reIp);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_SIP_SEP_REG_PORT", sep_rePort);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_SIP_REG_EXPIRY", reg_ptime);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_SIP_PRI_PROXY_ADDR", proxyIp);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_SIP_PRI_PROXY_PORT", proxyPort);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_SIP_OUTBOUND_ADDR", outboundIp);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_SIP_OUTBOUND_PORT", outboundPort);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_NAT_SRV_ADDR", stunIp);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_NAT_SRV_PORT", stunPort);
	
	
	/*
	reip = strdup(web_get("stunIp", input, 1));
	reip = strdup(web_get("stunPort", input, 1));
	*/
	nvram_commit(VOIP_NVRAM );
	free_all(11, reIp, rePort, sep_reIp, sep_rePort, reg_ptime, proxyIp, proxyPort, outboundIp, outboundPort, stunIp, stunPort);
}
void set_voip2(char *input)
{
	char *reIp2, *rePort2, *sep_reIp2, *sep_rePort2, *reg_ptime2, *proxyIp2, *proxyPort2, *outboundIp2, *outboundPort2, *stunIp2, *stunPort2;
	web_debug_header();
	reIp2 = strdup(web_get("reIp2", input, 1));
	rePort2 = strdup(web_get("rePort2", input, 1));
	sep_reIp2 = strdup(web_get("sep_reIp2", input, 1));
	sep_rePort2 = strdup(web_get("sep_rePort2", input, 1));
	reg_ptime2 = strdup(web_get("reg_ptime2", input, 1));
	proxyIp2 = strdup(web_get("proxyIp2", input, 1));
	proxyPort2 = strdup(web_get("proxyPort2", input, 1));
	outboundIp2 = strdup(web_get("outboundIp2", input, 1));
	outboundPort2 = strdup(web_get("outboundPort2", input, 1));
	stunIp2 = strdup(web_get("stunIp2", input, 1));
	stunPort2 = strdup(web_get("stunPort2", input, 1));
	
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_SIP_REG_ADDR", reIp2);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_SIP_REG_PORT", rePort2);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_SIP_SEP_REG_ADDR", sep_reIp2);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_SIP_SEP_REG_PORT", sep_rePort2);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_SIP_REG_EXPIRY", reg_ptime2);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_SIP_PRI_PROXY_ADDR", proxyIp2);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_SIP_PRI_PROXY_PORT", proxyPort2);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_SIP_OUTBOUND_ADDR", outboundIp2);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_SIP_OUTBOUND_PORT", outboundPort2);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_NAT_SRV_ADDR", stunIp2);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_NAT_SRV_PORT", stunPort2);
	/*
	reip = strdup(web_get("stunIp", input, 1));
	reip = strdup(web_get("stunPort", input, 1));
	*/
	nvram_commit(VOIP_NVRAM );
	free_all(11, reIp2, rePort2, sep_reIp2, sep_rePort2, reg_ptime2, proxyIp2, proxyPort2, outboundIp2, outboundPort2, stunIp2, stunPort2);
}
void voip_user(char *input)
{
	char *sip_select, *sip_port, *d_name, *auth_name, *s_password, *codec1st, *codec2nd, *codec3rd, *codec4th, *codec5th, *codec6th, *codec7th, *codec8th, *codec9th, *g723_1_rates
	     ,*ilbc_rates, *setimer_select, *setimer_refresh, *min_se_timer, *se_timer, *s_usr_name, *ring_back_timeout, *subscriber_number;
	
	web_debug_header();
	sip_select = strdup(web_get("sip_select", input, 1));
	sip_port = strdup(web_get("sip_port", input, 1));
	d_name = strdup(web_get("d_name", input, 1));

	auth_name = strdup(web_get("auth_name", input, 1));
	s_password = strdup(web_get("s_password", input, 1));
	codec1st = strdup(web_get("codec1st", input, 1));
	codec2nd = strdup(web_get("codec2nd", input, 1));
	codec3rd = strdup(web_get("codec3rd", input, 1));
	codec4th = strdup(web_get("codec4th", input, 1));
	codec5th = strdup(web_get("codec5th", input, 1));
	codec6th = strdup(web_get("codec6th", input, 1));
	codec7th = strdup(web_get("codec7th", input, 1));
	codec8th = strdup(web_get("codec8th", input, 1));
	codec9th = strdup(web_get("codec9th", input, 1));
		subscriber_number = strdup(web_get("subscriber_number", input, 1));
	g723_1_rates = strdup(web_get("g723_1_rates", input, 1));
	ilbc_rates = strdup(web_get("ilbc_rates", input, 1));
	setimer_select = strdup(web_get("setimer_select", input, 1));	
	
	setimer_refresh = strdup(web_get("setimer_refresh", input, 1));
	min_se_timer = strdup(web_get("min_se_timer", input, 1));
	se_timer = strdup(web_get("se_timer", input, 1));
	
	s_usr_name = strdup(web_get("s_usr_name", input, 1));
	ring_back_timeout = strdup(web_get("ring_back_timeout", input, 1));
	
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_ENABLE", sip_select);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_SIP_LOCAL_PORT", sip_port);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_SIP_DISPNAME", d_name);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_SIP_AUTHNAME", auth_name);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_SIP_PASSWORD", s_password);
	
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_MEDIA_1ST_CODEC", codec1st);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_MEDIA_2ND_CODEC", codec2nd);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_MEDIA_3RD_CODEC", codec3rd);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_MEDIA_4TH_CODEC", codec4th);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_MEDIA_5TH_CODEC", codec5th);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_MEDIA_6TH_CODEC", codec6th);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_MEDIA_7TH_CODEC", codec7th);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_MEDIA_8TH_CODEC", codec8th);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_MEDIA_9TH_CODEC", codec9th);
		
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_MEDIA_G723_RATE", g723_1_rates);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_MEDIA_ILBC_RATE", ilbc_rates);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_SIP_SESSION_FLAG", setimer_select);
	//nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_SIP_SESSION_REFRESHER", outboundIp);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_SIP_SESSION_METHOD", setimer_refresh);
	
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_SIP_SESSION_MIN_EXP", min_se_timer);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_SIP_SESSION_TIMER", se_timer);
	
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_SIP_USER_AGENT", s_usr_name);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_TIMER_RINGING", ring_back_timeout);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_SIP_USERNAME", subscriber_number);

	nvram_commit(VOIP_NVRAM );
		free_all(21, sip_select, sip_port, d_name, auth_name, s_password, codec1st, codec2nd, codec3rd, codec4th, codec5th, codec6th, codec7th, codec8th, codec9th, g723_1_rates
	     ,ilbc_rates, setimer_select, setimer_refresh, min_se_timer, se_timer, s_usr_name, ring_back_timeout, subscriber_number);	
}
void voip_user2(char *input)
{
	char *sip_select2, *sip_port2, *d_name2, *auth_name2, *s_password2, *codec1st2, *codec2nd2, *codec3rd2, *codec4th2, *codec5th2, *codec6th2, *codec7th2, *codec8th2, *codec9th2, *g723_1_rates2
	     ,*ilbc_rates2, *setimer_select2, *setimer_refresh2, *min_se_timer2, *se_timer2, *s_usr_name2, *ring_back_timeout2, *subscriber_number2;
	
	web_debug_header();
	sip_select2 = strdup(web_get("sip_select2", input, 1));
	sip_port2 = strdup(web_get("sip_port2", input, 1));
	d_name2 = strdup(web_get("d_name2", input, 1));

	auth_name2 = strdup(web_get("auth_name2", input, 1));
	s_password2 = strdup(web_get("s_password2", input, 1));
	codec1st2 = strdup(web_get("codec1st2", input, 1));
	codec2nd2 = strdup(web_get("codec2nd2", input, 1));
	codec3rd2 = strdup(web_get("codec3rd2", input, 1));
	codec4th2 = strdup(web_get("codec4th2", input, 1));
	codec5th2 = strdup(web_get("codec5th2", input, 1));
	codec6th2 = strdup(web_get("codec6th2", input, 1));
	codec7th2 = strdup(web_get("codec7th2", input, 1));
	codec8th2 = strdup(web_get("codec8th2", input, 1));
	codec9th2 = strdup(web_get("codec9th2", input, 1));
		subscriber_number2 = strdup(web_get("subscriber_number2", input, 1));
	g723_1_rates2 = strdup(web_get("g723_1_rates2", input, 1));
	ilbc_rates2 = strdup(web_get("ilbc_rates2", input, 1));
	setimer_select2 = strdup(web_get("setimer_select2", input, 1));	
	
	setimer_refresh2 = strdup(web_get("setimer_refresh2", input, 1));
	min_se_timer2 = strdup(web_get("min_se_timer2", input, 1));
	se_timer2 = strdup(web_get("se_timer2", input, 1));
	
	s_usr_name2 = strdup(web_get("s_usr_name2", input, 1));
	ring_back_timeout2 = strdup(web_get("ring_back_timeout2", input, 1));
	
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_ENABLE", sip_select2);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_SIP_LOCAL_PORT", sip_port2);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_SIP_DISPNAME", d_name2);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_SIP_AUTHNAME", auth_name2);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_SIP_PASSWORD", s_password2);
	
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_MEDIA_1ST_CODEC", codec1st2);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_MEDIA_2ND_CODEC", codec2nd2);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_MEDIA_3RD_CODEC", codec3rd2);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_MEDIA_4TH_CODEC", codec4th2);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_MEDIA_5TH_CODEC", codec5th2);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_MEDIA_6TH_CODEC", codec6th2);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_MEDIA_7TH_CODEC", codec7th2);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_MEDIA_8TH_CODEC", codec8th2);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_MEDIA_9TH_CODEC", codec9th2);
		
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_MEDIA_G723_RATE", g723_1_rates2);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_MEDIA_ILBC_RATE", ilbc_rates2);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_SIP_SESSION_FLAG", setimer_select2);
	//nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_SIP_SESSION_REFRESHER", outboundIp);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_SIP_SESSION_METHOD", setimer_refresh2);
	
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_SIP_SESSION_MIN_EXP", min_se_timer2);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_SIP_SESSION_TIMER", se_timer2);
	
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_SIP_USER_AGENT", s_usr_name2);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_TIMER_RINGING", ring_back_timeout2);
  	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_SIP_USERNAME", subscriber_number2);
	nvram_commit(VOIP_NVRAM );
		free_all(21, sip_select2, sip_port2, d_name2, auth_name2, s_password2, codec1st2, codec2nd2, codec3rd2, codec4th2, codec5th2, codec6th2, codec7th2, codec8th2, codec9th2, g723_1_rates2
	     ,ilbc_rates2, setimer_select2, setimer_refresh2, min_se_timer2, se_timer2, s_usr_name2, ring_back_timeout2, subscriber_number2);	
}
void voip_feature(char *input)
{
	char *a_d_anonymous, *h_u_id, *mwi, *mwi_interval, *mwi_event, *holdmethod, *dtmfoption, *sipinfo, *allcallforward, *unconditionalcf, *unconditionalcf_target
	    ,*busycf, *busycf_target, *noanswer_cf, *noanswer_cf_target, *call_waiting, *hotline, *hotline_target, *hotline_period_time, *dnd ;
	
	web_debug_header();
	a_d_anonymous = strdup(web_get("a_d_anonymous", input, 1));
	h_u_id = strdup(web_get("h_u_id", input, 1));
	mwi = strdup(web_get("mwi", input, 1));
	mwi_interval = strdup(web_get("mwi_interval", input, 1));
	mwi_event = strdup(web_get("mwi_event", input, 1));
	holdmethod = strdup(web_get("holdmethod", input, 1));	
	dtmfoption = strdup(web_get("dtmfoption", input, 1));

	sipinfo = strdup(web_get("sipinfo", input, 1));
	allcallforward = strdup(web_get("allcallforward", input, 1));
	unconditionalcf = strdup(web_get("unconditionalcf", input, 1));
	unconditionalcf_target = strdup(web_get("unconditionalcf_target", input, 1));
	busycf = strdup(web_get("busycf", input, 1));
	busycf_target = strdup(web_get("busycf_target", input, 1));
	noanswer_cf = strdup(web_get("noanswer_cf", input, 1));
	noanswer_cf_target = strdup(web_get("noanswer_cf_target", input, 1));		
	call_waiting = strdup(web_get("call_waiting", input, 1));
	hotline = strdup(web_get("hotline", input, 1));
	hotline_target = strdup(web_get("hotline_target", input, 1));
	hotline_period_time = strdup(web_get("hotline_period_time", input, 1));
	dnd = strdup(web_get("dnd", input, 1));
	
	
	
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_ADV_AUTO_DECLINE", a_d_anonymous);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_ADV_HIDE_USER_ID", h_u_id);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_SIP_MWI", mwi);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_SIP_MWI_INTERVAL", mwi_interval);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_SIP_MWI_EVENT", mwi_event);
	
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_ADV_HOLD_METHOD", holdmethod);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_ADV_DTMF_TYPE", dtmfoption);

	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_SIP_INFO_ENABLE", sipinfo);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_ADV_ALL_CF", allcallforward);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_ADV_CFU", unconditionalcf);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_ADV_CFU_TARGET", unconditionalcf_target);
	
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_ADV_CFB", busycf);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_ADV_CFB_TARGET", busycf_target);
		
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_ADV_CFNA", noanswer_cf);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_ADV_CFNA_TARGET", noanswer_cf_target);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_ADV_CALL_WAITING", call_waiting);
	
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_ADV_HOTLINE", hotline);
	
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_ADV_HOTLINE_TARGET", hotline_target);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_ADV_HOTLINE_TIMER", hotline_period_time);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_ADV_DND", dnd);

	nvram_commit(VOIP_NVRAM );
	
	free_all(20, a_d_anonymous, h_u_id, mwi, mwi_interval, mwi_event, holdmethod, dtmfoption, sipinfo, allcallforward, unconditionalcf, unconditionalcf_target
	    ,busycf, busycf_target, noanswer_cf, noanswer_cf_target, call_waiting, hotline, hotline_target, hotline_period_time, dnd);	
	
	
}

void voip_feature2(char *input)
{
	char *a_d_anonymous2, *h_u_id2, *mwi2, *mwi_interval2, *mwi_event2, *holdmethod2, *dtmfoption2, *sipinfo2, *allcallforward2, *unconditionalcf2, *unconditionalcf_target2
	    ,*busycf2, *busycf_target2, *noanswer_cf2, *noanswer_cf_target2, *call_waiting2, *hotline2, *hotline_target2, *hotline_period_time2, *dnd2;
	
	web_debug_header();
	a_d_anonymous2 = strdup(web_get("a_d_anonymous2", input, 1));
	h_u_id2 = strdup(web_get("h_u_id2", input, 1));
	mwi2 = strdup(web_get("mwi2", input, 1));
	mwi_interval2 = strdup(web_get("mwi_interval2", input, 1));
	mwi_event2 = strdup(web_get("mwi_event2", input, 1));
	holdmethod2 = strdup(web_get("holdmethod2", input, 1));	
	dtmfoption2 = strdup(web_get("dtmfoption2", input, 1));

	sipinfo2 = strdup(web_get("sipinfo2", input, 1));
	allcallforward2 = strdup(web_get("allcallforward2", input, 1));
	unconditionalcf2 = strdup(web_get("unconditionalcf2", input, 1));
	unconditionalcf_target2 = strdup(web_get("unconditionalcf_target2", input, 1));
	busycf2 = strdup(web_get("busycf2", input, 1));
	busycf_target2 = strdup(web_get("busycf_target2", input, 1));
	noanswer_cf2 = strdup(web_get("noanswer_cf2", input, 1));
	noanswer_cf_target2 = strdup(web_get("noanswer_cf_target2", input, 1));		
	call_waiting2 = strdup(web_get("call_waiting2", input, 1));
	hotline2 = strdup(web_get("hotline2", input, 1));
	hotline_target2 = strdup(web_get("hotline_target2", input, 1));
	hotline_period_time2 = strdup(web_get("hotline_period_time2", input, 1));
	dnd2 = strdup(web_get("dnd2", input, 1));
	
	
	
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_ADV_AUTO_DECLINE", a_d_anonymous2);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_ADV_HIDE_USER_ID", h_u_id2);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_SIP_MWI", mwi2);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_SIP_MWI_INTERVAL", mwi_interval2);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_SIP_MWI_EVENT", mwi_event2);
	
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_ADV_HOLD_METHOD", holdmethod2);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_ADV_DTMF_TYPE", dtmfoption2);

	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_SIP_INFO_ENABLE", sipinfo2);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_ADV_ALL_CF", allcallforward2);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_ADV_CFU", unconditionalcf2);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_ADV_CFU_TARGET", unconditionalcf_target2);
	
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_ADV_CFB", busycf2);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_ADV_CFB_TARGET", busycf_target2);
		
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_ADV_CFNA", noanswer_cf2);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_ADV_CFNA_TARGET", noanswer_cf_target2);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_ADV_CALL_WAITING", call_waiting2);
	
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_ADV_HOTLINE", hotline2);
	
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_ADV_HOTLINE_TARGET", hotline_target2);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_ADV_HOTLINE_TIMER", hotline_period_time2);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_ADV_DND", dnd2);

	nvram_commit(VOIP_NVRAM );
	
	free_all(20, a_d_anonymous2, h_u_id2, mwi2, mwi_interval2, mwi_event2, holdmethod2, dtmfoption2, sipinfo2, allcallforward2, unconditionalcf2, unconditionalcf_target2
	    ,busycf2, busycf_target2, noanswer_cf2, noanswer_cf_target2, call_waiting2, hotline2, hotline_target2, hotline_period_time2, dnd2);	
	
	
}
void voip_dialing(char *input)
{
	char *timer_digit, *timer_first_digit;
	web_debug_header();
	timer_digit = strdup(web_get("timer_digit", input, 1));
	timer_first_digit = strdup(web_get("timer_first_digit", input, 1));
	
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_TIMER_INTER_DIGIT", timer_digit);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_TIMER_FIRST_DIGIT", timer_first_digit);
	
	nvram_commit(VOIP_NVRAM );
	free_all(2, timer_digit, timer_first_digit);
}

void voip_dialing2(char *input)
{
	char *timer_digit2, *timer_first_digit2;
	web_debug_header();
	timer_digit2 = strdup(web_get("timer_digit2", input, 1));
	timer_first_digit2 = strdup(web_get("timer_first_digit2", input, 1));
	
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_TIMER_INTER_DIGIT", timer_digit2);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_TIMER_FIRST_DIGIT", timer_first_digit2);
	
	nvram_commit(VOIP_NVRAM );
	free_all(2, timer_digit2, timer_first_digit2);
}

inline int getRuleNums(char *rules)
{
	return get_nums(rules, ';');
}
void voip_speed_dial(char *input)
{
	char *speedEnable, *dial_activeEnabled,*dialShortNumber, *dialrealNumber, *dialNote;
	int  dialShortNumber_int, dialrealNumber_int, dial_activeEnabled_int;
	char *VoipDialRules, *SC_ACCT_1_SD_STR;
	char rule[8192], dialrule[8192];
	
	speedEnable = strdup(web_get("speedEnable", input, 1));
	
	dial_activeEnabled = strdup(web_get("dial_activeEnabled", input, 1));
	
	dialShortNumber = strdup(web_get("dialShortNumber", input, 1));

	dialrealNumber = strdup(web_get("dialrealNumber", input, 1));
	
	dialNote = strdup(web_get("dialNote", input, 1));
		
	
	dialShortNumber_int = atoi(dialShortNumber);
	dialrealNumber_int = atoi(dialrealNumber);
	dial_activeEnabled_int = atoi(dial_activeEnabled);
	
	
	if(( VoipDialRules = (char *)nvram_bufget(VOIP_NVRAM , "VoipDialRules")) && strlen( VoipDialRules) ){
		if(dial_activeEnabled_int==0)
			snprintf(rule, sizeof(rule), "%s;0,%d,%d,%s",  VoipDialRules, dialShortNumber_int, dialrealNumber_int,dialNote);
		else
			snprintf(rule, sizeof(rule), "%s;1,%d,%d,%s",  VoipDialRules, dialShortNumber_int, dialrealNumber_int,dialNote);
	}
	else{
		if(dial_activeEnabled_int==0)
			snprintf(rule, sizeof(rule), "0,%d,%d,%s", dialShortNumber_int, dialrealNumber_int, dialNote);
		else
			snprintf(rule, sizeof(rule), "1,%d,%d,%s", dialShortNumber_int, dialrealNumber_int, dialNote);
  }

	if(( SC_ACCT_1_SD_STR = (char *)nvram_bufget(VOIP_NVRAM , "SC_ACCT_1_SD_STR")) && strlen( SC_ACCT_1_SD_STR) ){
		if(dial_activeEnabled_int==0)
			snprintf(dialrule, sizeof(dialrule), "%s;0,N,%d,%d,%s",  SC_ACCT_1_SD_STR, dialShortNumber_int, dialrealNumber_int,dialNote);
		else
			snprintf(dialrule, sizeof(dialrule), "%s;0,Y,%d,%d,%s",  SC_ACCT_1_SD_STR, dialShortNumber_int, dialrealNumber_int,dialNote);
	}
	else{
		if(dial_activeEnabled_int==0)
    	snprintf(dialrule, sizeof(dialrule), ";0,N,%d,%d,%s", dialShortNumber_int, dialrealNumber_int, dialNote);
    else
    	snprintf(dialrule, sizeof(dialrule), ";0,Y,%d,%d,%s", dialShortNumber_int, dialrealNumber_int, dialNote);
  }
		
		

  
  nvram_set(VOIP_NVRAM , "SC_ACCT_1_SD_AC_ENABLE", dial_activeEnabled);
   nvram_set(VOIP_NVRAM , "SC_ACCT_1_SD_ENABLE", speedEnable);
	nvram_set(VOIP_NVRAM , "VoipDialRules", rule);
	nvram_set(VOIP_NVRAM , "SC_ACCT_1_SD_STR", dialrule);
	nvram_commit(VOIP_NVRAM );
	
	free_all(5, speedEnable, dial_activeEnabled, dialShortNumber, dialrealNumber, dialNote);
}
void voip_speed_dial2(char *input)
{
	char *speedEnable2, *dial_activeEnabled2,*dialShortNumber2, *dialrealNumber2, *dialNote2;
	int  dialShortNumber_int2, dialrealNumber_int2, dial_activeEnabled_int2;
	char *VoipDialRules2, *SC_ACCT_2_SD_STR;
	char rule2[8192], dialrule2[8192];
	
	speedEnable2 = strdup(web_get("speedEnable2", input, 1));
	
	dial_activeEnabled2 = strdup(web_get("dial_activeEnabled2", input, 1));
	
	dialShortNumber2 = strdup(web_get("dialShortNumber2", input, 1));

	dialrealNumber2 = strdup(web_get("dialrealNumber2", input, 1));
	
	dialNote2 = strdup(web_get("dialNote2", input, 1));
		
	
	dialShortNumber_int2 = atoi(dialShortNumber2);
	dialrealNumber_int2 = atoi(dialrealNumber2);
	dial_activeEnabled_int2 = atoi(dial_activeEnabled2);
	
	
	if(( VoipDialRules2 = (char *)nvram_bufget(VOIP_NVRAM , "VoipDialRules2")) && strlen( VoipDialRules2) ){
		if(dial_activeEnabled_int2==0)
			snprintf(rule2, sizeof(rule2), "%s;0,%d,%d,%s",  VoipDialRules2, dialShortNumber_int2, dialrealNumber_int2,dialNote2);
		else
			snprintf(rule2, sizeof(rule2), "%s;1,%d,%d,%s",  VoipDialRules2, dialShortNumber_int2, dialrealNumber_int2,dialNote2);
	}
	else{
		if(dial_activeEnabled_int2==0)
			snprintf(rule2, sizeof(rule2), "0,%d,%d,%s", dialShortNumber_int2, dialrealNumber_int2, dialNote2);
		else
			snprintf(rule2, sizeof(rule2), "1,%d,%d,%s", dialShortNumber_int2, dialrealNumber_int2, dialNote2);
  }

	if(( SC_ACCT_2_SD_STR = (char *)nvram_bufget(VOIP_NVRAM , "SC_ACCT_2_SD_STR")) && strlen( SC_ACCT_2_SD_STR) ){
		if(dial_activeEnabled_int2==0)
			snprintf(dialrule2, sizeof(dialrule2), "%s;0,N,%d,%d,%s",  SC_ACCT_2_SD_STR, dialShortNumber_int2, dialrealNumber_int2,dialNote2);
		else
			snprintf(dialrule2, sizeof(dialrule2), "%s;0,Y,%d,%d,%s",  SC_ACCT_2_SD_STR, dialShortNumber_int2, dialrealNumber_int2,dialNote2);
	}
	else{
		if(dial_activeEnabled_int2==0)
    	snprintf(dialrule2, sizeof(dialrule2), ";0,N,%d,%d,%s", dialShortNumber_int2, dialrealNumber_int2, dialNote2);
    else
    	snprintf(dialrule2, sizeof(dialrule2), ";0,Y,%d,%d,%s", dialShortNumber_int2, dialrealNumber_int2, dialNote2);
  }
		
		

  
  nvram_set(VOIP_NVRAM , "SC_ACCT_2_SD_AC_ENABLE", dial_activeEnabled2);
   nvram_set(VOIP_NVRAM , "SC_ACCT_2_SD_ENABLE", speedEnable2);
	nvram_set(VOIP_NVRAM , "VoipDialRules2", rule2);
	nvram_set(VOIP_NVRAM , "SC_ACCT_2_SD_STR", dialrule2);
	nvram_commit(VOIP_NVRAM );
	
	free_all(5, speedEnable2, dial_activeEnabled2, dialShortNumber2, dialrealNumber2, dialNote2);
}
static void dialRuleDelete(char *input)
{
	int i, j, rule_count;
	char name_buf[16];
	//char *value;
	char *value;
	int *deleArray;
	char *firewall_enable;

	char *new_rules, *new_rules1;
	
  const char *rules = (char *)nvram_bufget(VOIP_NVRAM , "VoipDialRules");
  const char *rules1 = (char *)nvram_bufget(VOIP_NVRAM , "SC_ACCT_1_SD_STR");
    if(!rules || !strlen(rules) )
        return;

	rule_count = getRuleNums((char *)rules);
	if(!rule_count)
		return;

	deleArray = (int *)malloc(rule_count * sizeof(int));
	if(!deleArray)
		return;

	new_rules = strdup(rules);
	new_rules1 = strdup(rules1);
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

        return;
    }

	delete_nth_value(deleArray, rule_count, new_rules, ';');
	voip_delete_nth_value(deleArray, rule_count, new_rules1, ';');
	free(deleArray);

	nvram_set(VOIP_NVRAM , "VoipDialRules", new_rules);
	nvram_set(VOIP_NVRAM , "SC_ACCT_1_SD_STR", new_rules1);
	nvram_commit(VOIP_NVRAM );
	free(new_rules);

	return;
}  
static void dialRuleDelete2(char *input)
{
	int i, j, rule_count2;
	char name_buf2[16];
	//char *value;
	char *value2;
	int *deleArray2;
	char *firewall_enable2;

	char *new_rules2, *new_rules12;
	
  const char *rules2 = (char *)nvram_bufget(VOIP_NVRAM , "VoipDialRules2");
  const char *rules12 = (char *)nvram_bufget(VOIP_NVRAM , "SC_ACCT_2_SD_STR");
    if(!rules2 || !strlen(rules2) )
        return;

	rule_count2 = getRuleNums((char *)rules2);
	if(!rule_count2)
		return;

	deleArray2 = (int *)malloc(rule_count2 * sizeof(int));
	if(!deleArray2)
		return;

	new_rules2 = strdup(rules2);
	new_rules12 = strdup(rules12);
	if(!new_rules2){
		free(deleArray2);
		return;
	}

	web_debug_header();
	for(i=0, j=0; i< rule_count2; i++){
		snprintf(name_buf2, 16, "delRule%d", i);
		value2 = web_get(name_buf2, input, 1);
		if(strcmp(value2, "")!=0){
			deleArray2[j++] = i;
	
		}
	}

    if(!j){
		free(deleArray2);
		free(new_rules2);
		printf("You didn't select any rules to delete.\n");

        return;
    }

	delete_nth_value(deleArray2, rule_count2, new_rules2, ';');
	voip_delete_nth_value(deleArray2, rule_count2, new_rules12, ';');
	free(deleArray2);

	nvram_set(VOIP_NVRAM , "VoipDialRules2", new_rules2);
	nvram_set(VOIP_NVRAM , "SC_ACCT_2_SD_STR", new_rules12);
	nvram_commit(VOIP_NVRAM );
	free(new_rules2);

	return;
}  
void voip_fax(char *input)
{
	char *fax_option;
	fax_option = strdup(web_get("fax_option", input, 1));
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_FAX_TYPE", fax_option);

	nvram_commit(VOIP_NVRAM );
	
	free_all(1, fax_option);	
}
void voip_fax2(char *input)
{
	char *fax_option2;
	fax_option2 = strdup(web_get("fax_option2", input, 1));
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_FAX_TYPE", fax_option2);

	nvram_commit(VOIP_NVRAM );
	
	free_all(1, fax_option2);	
}
void voip_rtp(char *input)
{
	char *rtpEnable, *rtptimeout, *rtppacketloss;
	rtpEnable = strdup(web_get("rtpEnable", input, 1));
	rtptimeout = strdup(web_get("rtptimeout", input, 1));	
	rtppacketloss = strdup(web_get("rtppacketloss", input, 1));		
	
	
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_MEDIA_RTP_DETECT", rtpEnable);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_MEDIA_RTP_DETECT_INTERVAL", rtptimeout);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_1_MEDIA_RTP_DETECT_PKT_LOSS", rtppacketloss);
	nvram_commit(VOIP_NVRAM );
	
	free_all(3, rtpEnable, rtptimeout, rtppacketloss);	
}
void voip_rtp2(char *input)
{
	char *rtpEnable2, *rtptimeout2, *rtppacketloss2;
	rtpEnable2 = strdup(web_get("rtpEnable2", input, 1));
	rtptimeout2 = strdup(web_get("rtptimeout2", input, 1));	
	rtppacketloss2 = strdup(web_get("rtppacketloss2", input, 1));		
	
	
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_MEDIA_RTP_DETECT", rtpEnable2);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_MEDIA_RTP_DETECT_INTERVAL", rtptimeout2);
	nvram_bufset(VOIP_NVRAM , "SC_ACCT_2_MEDIA_RTP_DETECT_PKT_LOSS", rtppacketloss2);
	nvram_commit(VOIP_NVRAM );
	
	free_all(3, rtpEnable2, rtptimeout2, rtppacketloss2);	
}
void voip_system(char *input)
{
	char *speedEnable, *speedup_string, *emergencyEnable,*emergencyRegistration, *wimax_connect_timer, *number_generic, *number_police, *number_medical, *number_fire, *priority_urgent, *priority_normal, *priority_non_urgent, *sip_ti_interval;
	
	
	
	speedEnable = strdup(web_get("speedEnable", input, 1));
	speedup_string = strdup(web_get("speedup_string", input, 1));	
	emergencyEnable = strdup(web_get("emergencyEnable", input, 1));		
	emergencyRegistration = strdup(web_get("emergencyRegistration", input, 1));
	wimax_connect_timer = strdup(web_get("wimax_connect_timer", input, 1));	
	number_generic = strdup(web_get("number_generic", input, 1));		

	number_police = strdup(web_get("number_police", input, 1));
	number_medical = strdup(web_get("number_medical", input, 1));	
	number_fire = strdup(web_get("number_fire", input, 1));		
	priority_urgent = strdup(web_get("priority_urgent", input, 1));
	priority_normal = strdup(web_get("priority_normal", input, 1));	
	priority_non_urgent = strdup(web_get("priority_non_urgent", input, 1));	
	sip_ti_interval = strdup(web_get("sip_ti_interval", input, 1));	
	
	
	
	
	nvram_bufset(VOIP_NVRAM , "SC_SYS_SPEED_UP_DIALING", speedEnable);
	nvram_bufset(VOIP_NVRAM , "SC_SYS_SPEED_UP_DIALING_STR", speedup_string);
	nvram_bufset(VOIP_NVRAM , "SC_EMERG_ENABLE", emergencyEnable);
	nvram_bufset(VOIP_NVRAM , "SC_EMERG_REGISTRATION", emergencyRegistration);

	nvram_bufset(VOIP_NVRAM , "SC_EMERG_TIMER_WMX_CONNECT", wimax_connect_timer);
	nvram_bufset(VOIP_NVRAM , "SC_EMERG_NUM_GENERIC", number_generic);
	nvram_bufset(VOIP_NVRAM , "SC_EMERG_NUM_POLICE", number_police);
	nvram_bufset(VOIP_NVRAM , "SC_EMERG_NUM_MEDICAL", number_medical);
	
	nvram_bufset(VOIP_NVRAM , "SC_EMERG_NUM_FIRE", number_fire);
	nvram_bufset(VOIP_NVRAM , "SC_SYS_SIP_PRIORITY_URGENT_NUMS", priority_urgent);
  nvram_bufset(VOIP_NVRAM , "SC_SYS_SIP_PRIORITY_NORMAL_NUMS", priority_normal);
	nvram_bufset(VOIP_NVRAM , "SC_SYS_SIP_PRIORITY_NON_URGENT_NUMS", priority_non_urgent);
	nvram_bufset(VOIP_NVRAM , "SC_SYS_SIP_T1_INTERVAL", sip_ti_interval);
	

	
	nvram_commit(VOIP_NVRAM );
	
	free_all(13, speedEnable, speedup_string, emergencyEnable,emergencyRegistration, wimax_connect_timer, number_generic, number_police, number_medical, number_fire, priority_urgent, priority_normal, priority_non_urgent, sip_ti_interval);
}

void voip_media(char *input)
{
	char *rtcp_interval, *media_port_start, *media_port_end, *g_726_16k, *g_726_24k, *g_726_32k, *g_726_40k, *ilbc_pt, *telephone_event,*g_711_codec, *g_723_codec, *g_726_codec, *g_729_codec, *ilbc_codec ,*voice_jitter_buffer_type,*voice_jitter_buffer_len,*packet_loss_concealment,*dvcc_enable,*static_jitter_len ;
	rtcp_interval = strdup(web_get("rtcp_interval", input, 1));
	media_port_start = strdup(web_get("media_port_start", input, 1));
	media_port_end = strdup(web_get("media_port_end", input, 1));	
	g_726_16k = strdup(web_get("g_726_16k", input, 1));		
	g_726_24k = strdup(web_get("g_726_24k", input, 1));
	g_726_32k = strdup(web_get("g_726_32k", input, 1));	
	g_726_40k = strdup(web_get("g_726_40k", input, 1));	
	ilbc_pt = strdup(web_get("ilbc_pt", input, 1));	
	telephone_event = strdup(web_get("telephone_event", input, 1));
	
	g_711_codec = strdup(web_get("g_711_codec", input, 1));	
	g_723_codec = strdup(web_get("g_723_codec", input, 1));	
	g_726_codec = strdup(web_get("g_726_codec", input, 1));
	g_729_codec = strdup(web_get("g_729_codec", input, 1));	
	ilbc_codec = strdup(web_get("ilbc_codec", input, 1));			
	voice_jitter_buffer_type = strdup(web_get("voice_jitter_buffer_type", input, 1));
	voice_jitter_buffer_len = strdup(web_get("voice_jitter_buffer_len", input, 1));	
	packet_loss_concealment = strdup(web_get("packet_loss_concealment", input, 1));	
	dvcc_enable = strdup(web_get("dvcc_enable", input, 1));	
	static_jitter_len = strdup(web_get("static_jitter_len", input, 1));	
	
	nvram_bufset(VOIP_NVRAM , "SC_SYS_RTCP_SEND_INTERVAL", rtcp_interval);
	nvram_bufset(VOIP_NVRAM , "SC_MEDIA_PORT_START", media_port_start);
  nvram_bufset(VOIP_NVRAM , "SC_MEDIA_PORT_END", media_port_end);
	nvram_bufset(VOIP_NVRAM , "SC_MEDIA_CODEC_G726_16_PT", g_726_16k);
	nvram_bufset(VOIP_NVRAM , "SC_MEDIA_CODEC_G726_24_PT", g_726_24k);
	nvram_bufset(VOIP_NVRAM , "SC_MEDIA_CODEC_G726_32_PT", g_726_32k);
	nvram_bufset(VOIP_NVRAM , "SC_MEDIA_CODEC_G726_40_PT", g_726_40k);
	nvram_bufset(VOIP_NVRAM , "SC_MEDIA_CODEC_ILBC_PT", ilbc_pt);
	nvram_bufset(VOIP_NVRAM , "SC_MEDIA_CODEC_TELEVT_PT", telephone_event);

	nvram_bufset(VOIP_NVRAM , "SC_MEDIA_CODEC_G711_PTIME", g_711_codec);
	nvram_bufset(VOIP_NVRAM , "SC_MEDIA_CODEC_G723_PTIME", g_723_codec);
	nvram_bufset(VOIP_NVRAM , "SC_MEDIA_CODEC_G726_PTIME", g_726_codec);
  nvram_bufset(VOIP_NVRAM , "SC_MEDIA_CODEC_G729_PTIME", g_729_codec);
	nvram_bufset(VOIP_NVRAM , "SC_MEDIA_CODEC_ILBC_PTIME", ilbc_codec);
	nvram_bufset(VOIP_NVRAM , "SC_SYS_VOICE_JB_TYPE", voice_jitter_buffer_type);
	nvram_bufset(VOIP_NVRAM , "SC_SYS_VOICE_PLC_ENABLE", packet_loss_concealment);
	nvram_bufset(VOIP_NVRAM , "SC_SYS_FEATURE_DVCC", dvcc_enable);
	nvram_bufset(VOIP_NVRAM , "SC_SYS_VOICE_JB_LEN", voice_jitter_buffer_len);
	nvram_bufset(VOIP_NVRAM , "SC_SYS_T38_STATIC_JB_LEN", static_jitter_len);
					
	nvram_commit(VOIP_NVRAM );
	free_all(19, rtcp_interval, media_port_start, media_port_end, g_726_16k, g_726_24k, g_726_32k, g_726_40k, ilbc_pt, telephone_event,g_711_codec, g_723_codec, g_726_codec, g_729_codec, ilbc_codec, voice_jitter_buffer_type,voice_jitter_buffer_len,packet_loss_concealment,dvcc_enable,static_jitter_len);
	
}
void voip_qos(char *input){
	char *sip_tos_diffserv, *rtp_tos_diffserv;
	sip_tos_diffserv = strdup(web_get("sip_tos_diffserv", input, 1));		
	rtp_tos_diffserv = strdup(web_get("rtp_tos_diffserv", input, 1));
	nvram_bufset(VOIP_NVRAM , "SC_SYS_QOS_SIP_TOS", sip_tos_diffserv);
	nvram_bufset(VOIP_NVRAM , "SC_SYS_QOS_RTP_TOS", rtp_tos_diffserv);
	nvram_commit(VOIP_NVRAM );
	free_all(2, sip_tos_diffserv,rtp_tos_diffserv);

}
void voip_provision(char *input){
	char *provision_enable, *ftp_server, *file_path, *login_user_name, *login_password, *connection_timeout, *retry_count;
	provision_enable = strdup(web_get("provision_enable", input, 1));		
	ftp_server = strdup(web_get("ftp_server", input, 1));
	file_path = strdup(web_get("file_path", input, 1));
	login_user_name = strdup(web_get("login_user_name", input, 1));		
	login_password = strdup(web_get("login_password", input, 1));	
	connection_timeout = strdup(web_get("connection_timeout", input, 1));		
	retry_count = strdup(web_get("retry_count", input, 1));		
	
	
	nvram_bufset(VOIP_NVRAM , "SC_AUTO_PROV_ENABLE", provision_enable);
	nvram_bufset(VOIP_NVRAM , "SC_AUTO_PROV_FTP_ADDR", ftp_server);
	nvram_bufset(VOIP_NVRAM , "SC_AUTO_PROV_FTP_FILE_NAME", file_path);
	nvram_bufset(VOIP_NVRAM , "SC_AUTO_PROV_FTP_LOGIN_NAME", login_user_name);
	nvram_bufset(VOIP_NVRAM , "SC_AUTO_PROV_FTP_LOGIN_PASSWD", login_password);
	nvram_bufset(VOIP_NVRAM , "SC_AUTO_PROV_FTP_TIMEOUT", connection_timeout);
	nvram_bufset(VOIP_NVRAM , "SC_AUTO_PROV_FTP_RETRY", retry_count);
	nvram_commit(VOIP_NVRAM );
	free_all(7, provision_enable, ftp_server, file_path, login_user_name, login_password, connection_timeout, retry_count);

}
void voip_phone(char *input){
	char *flash_detect_up, *flash_detect_lv, *voice_tx_level, *ring_impedance, *voice_rx_level, *caller_id_type, *caller_id_display, *caller_id_pwr;
		flash_detect_up = strdup(web_get("flash_detect_up", input, 1));	
		flash_detect_lv = strdup(web_get("flash_detect_lv", input, 1));
		voice_tx_level = strdup(web_get("voice_tx_level", input, 1));
		ring_impedance = strdup(web_get("ring_impedance", input, 1));	
		voice_rx_level = strdup(web_get("voice_rx_level", input, 1));	
		caller_id_type = strdup(web_get("caller_id_type", input, 1));		
		caller_id_display = strdup(web_get("caller_id_display", input, 1));		
		caller_id_pwr = strdup(web_get("caller_id_pwr", input, 1));		
		
		nvram_bufset(VOIP_NVRAM , "SC_LINE_1_FLASH_DETECT_UP_LV", flash_detect_up);
		nvram_bufset(VOIP_NVRAM , "SC_LINE_1_FLASH_DETECT_DN_LV", flash_detect_lv);
		
		nvram_bufset(VOIP_NVRAM , "SC_LINE_1_VOGAIN_TX_LV", voice_tx_level);
		nvram_bufset(VOIP_NVRAM , "SC_LINE_1_VOGAIN_RX_LV", voice_rx_level);
		
		nvram_bufset(VOIP_NVRAM , "SC_LINE_1_RING_IMPEDENCE", ring_impedance);
		nvram_bufset(VOIP_NVRAM , "SC_LINE_1_CALLER_ID_TYPE", caller_id_type);
		
		nvram_bufset(VOIP_NVRAM , "SC_LINE_1_CALLER_ID_DISP", caller_id_display);
		nvram_bufset(VOIP_NVRAM , "SC_LINE_1_CALLER_ID_PWR", caller_id_pwr);
    nvram_commit(VOIP_NVRAM );
	  free_all(7, flash_detect_up, flash_detect_lv, voice_tx_level, ring_impedance, voice_rx_level, caller_id_type, caller_id_display, caller_id_pwr);		
										
}
void voip_phone2(char *input){
	char *flash_detect_up2, *flash_detect_lv2, *voice_tx_level2, *ring_impedance2, *voice_rx_level2, *caller_id_type2, *caller_id_display2, *caller_id_pwr2;
		flash_detect_up2 = strdup(web_get("flash_detect_up2", input, 1));	
		flash_detect_lv2 = strdup(web_get("flash_detect_lv2", input, 1));
		voice_tx_level2 = strdup(web_get("voice_tx_level2", input, 1));
		ring_impedance2 = strdup(web_get("ring_impedance2", input, 1));	
		voice_rx_level2 = strdup(web_get("voice_rx_level2", input, 1));	
		caller_id_type2 = strdup(web_get("caller_id_type2", input, 1));		
		caller_id_display2 = strdup(web_get("caller_id_display2", input, 1));		
		caller_id_pwr2 = strdup(web_get("caller_id_pwr2", input, 1));		
		
		nvram_bufset(VOIP_NVRAM , "SC_LINE_2_FLASH_DETECT_UP_LV", flash_detect_up2);
		nvram_bufset(VOIP_NVRAM , "SC_LINE_2_FLASH_DETECT_DN_LV", flash_detect_lv2);
		
		nvram_bufset(VOIP_NVRAM , "SC_LINE_2_VOGAIN_TX_LV", voice_tx_level2);
		nvram_bufset(VOIP_NVRAM , "SC_LINE_2_VOGAIN_RX_LV", voice_rx_level2);
		
		nvram_bufset(VOIP_NVRAM , "SC_LINE_2_RING_IMPEDENCE", ring_impedance2);
		nvram_bufset(VOIP_NVRAM , "SC_LINE_2_CALLER_ID_TYPE", caller_id_type2);
		
		nvram_bufset(VOIP_NVRAM , "SC_LINE_2_CALLER_ID_DISP", caller_id_display2);
		nvram_bufset(VOIP_NVRAM , "SC_LINE_2_CALLER_ID_PWR", caller_id_pwr2);
    nvram_commit(VOIP_NVRAM );
	  free_all(7, flash_detect_up2, flash_detect_lv2, voice_tx_level2, ring_impedance2, voice_rx_level2, caller_id_type2, caller_id_display2, caller_id_pwr2);		
										
}
void voip_voice(char *input){
	char *voice_active_detector, *line_echo_cancell, *drc;
		voice_active_detector = strdup(web_get("voice_active_detector", input, 1));	
		line_echo_cancell = strdup(web_get("line_echo_cancell", input, 1));
		drc = strdup(web_get("drc", input, 1));	
		nvram_bufset(VOIP_NVRAM , "SC_LINE_1_VAD", voice_active_detector);
		nvram_bufset(VOIP_NVRAM , "SC_LINE_1_LEC", line_echo_cancell);
		
		nvram_bufset(VOIP_NVRAM , "SC_LINE_1_DRC", drc);
    nvram_commit(VOIP_NVRAM );
	  free_all(3, voice_active_detector, line_echo_cancell, drc);	
	
}

void voip_voice2(char *input){
	char *voice_active_detector2, *line_echo_cancell2, *drc2;
		voice_active_detector2 = strdup(web_get("voice_active_detector2", input, 1));	
		line_echo_cancell2 = strdup(web_get("line_echo_cancell2", input, 1));
		drc2 = strdup(web_get("drc2", input, 1));	
		nvram_bufset(VOIP_NVRAM , "SC_LINE_2_VAD", voice_active_detector2);
		nvram_bufset(VOIP_NVRAM , "SC_LINE_2_LEC", line_echo_cancell2);
		
		nvram_bufset(VOIP_NVRAM , "SC_LINE_2_DRC", drc2);
    nvram_commit(VOIP_NVRAM );
	  free_all(3, voice_active_detector2, line_echo_cancell2, drc2);	
	
}

void voip_profile(char *input){
	char *country_profile;
		country_profile = strdup(web_get("country_profile", input, 1));	

		
		nvram_bufset(VOIP_NVRAM , "SC_LINE_1_REGION", country_profile);
    nvram_commit(VOIP_NVRAM );
	  free_all(1, country_profile);	
	
}
void voip_profile2(char *input){
	char *country_profile2;
		country_profile2 = strdup(web_get("country_profile2", input, 1));	

		
		nvram_bufset(VOIP_NVRAM , "SC_LINE_2_REGION", country_profile2);
    nvram_commit(VOIP_NVRAM );
	  free_all(2, country_profile2);	
	
}
void tr069(char *input){
	char *tr069Enable, *fixed_client_port, *acs_server_url, *bootstrapEnable, *acs_usemame, *acs_password, *periodicalEnable, *periodicalInterval, *connectionRequsetUse, *connectionPassword, *caCf, *cCif;
	tr069Enable = strdup(web_get("tr069Enable", input, 1));	
  fixed_client_port = strdup(web_get("fixed_client_port", input, 1));	 
  acs_server_url = strdup(web_get("acs_server_url", input, 1));	
  bootstrapEnable = strdup(web_get("bootstrapEnable", input, 1));	
  acs_usemame = strdup(web_get("acs_usemame", input, 1));	
  acs_password = strdup(web_get("acs_password", input, 1));	
  periodicalEnable = strdup(web_get("periodicalEnable", input, 1));	
  periodicalInterval = strdup(web_get("periodicalInterval", input, 1));
  connectionRequsetUse = strdup(web_get("connectionRequsetUse", input, 1));
  connectionPassword = strdup(web_get("connectionPassword", input, 1));
    //caCf = strdup(web_get("caCf", input, 1));		
    //cCif = strdup(web_get("cCif", input, 1));	
    
		nvram_bufset(VOIP_NVRAM , "TR_ENABLE", tr069Enable);
		nvram_bufset(VOIP_NVRAM , "TR_CLIENT_PORT", fixed_client_port);
		nvram_bufset(VOIP_NVRAM , "TR_ACS_URL", acs_server_url);
		nvram_bufset(VOIP_NVRAM , "TR_BOOTSTRAP", bootstrapEnable);
		nvram_bufset(VOIP_NVRAM , "TR_ACS_USER", acs_usemame);
		nvram_bufset(VOIP_NVRAM , "TR_ACS_PASS", acs_password);
		nvram_bufset(VOIP_NVRAM , "TR_INFORM_ENABLE", periodicalEnable);
		nvram_bufset(VOIP_NVRAM , "TR_INFORM_INTERVAL", periodicalInterval);
		nvram_bufset(VOIP_NVRAM , "TR_CR_USER", connectionRequsetUse);
		nvram_bufset(VOIP_NVRAM , "TR_CR_PASS", connectionPassword);
		//nvram_bufset(VOIP_NVRAM , "TR_CR_PATH", caCf);
		//nvram_bufset(VOIP_NVRAM , "TR_CR_PASS", cCif);
				
    nvram_commit(VOIP_NVRAM );
	  free_all(12, tr069Enable, fixed_client_port, acs_server_url, bootstrapEnable, acs_usemame, acs_password, periodicalEnable, periodicalInterval, connectionRequsetUse, connectionPassword, caCf, cCif);	
	
}
int main(int argc, char *argv[]) 
{
	char *form, *inStr;
	long inLen;

	nvram_init(VOIP_NVRAM );

	inLen = strtol(getenv("CONTENT_LENGTH"), NULL, 10) + 1;
	if (inLen <= 1) {
		fprintf(stderr, "%s: get no data!\n", __func__);
		return -1;
	}
	inStr = malloc(inLen);
	memset(inStr, 0, inLen);
	fgets(inStr, inLen, stdin);
	//DBG_MSG("%s\n", inStr);
	form = web_get("page", inStr, 0);
	if (!strcmp(form, "voip_status")) {
		voip_status(inStr);
	}else if(!strcmp(form, "voip_status2")){
	  voip_status2(inStr);		
	} else if (!strcmp(form, "set_voip")) {
 	  set_voip(inStr);
	}else if (!strcmp(form, "set_voip2")) {
 	  set_voip2(inStr);
	}else if (!strcmp(form, "voip_user")) {
 	  voip_user(inStr);
	}else if (!strcmp(form, "voip_user2")) {
 	  voip_user2(inStr);
	}else if (!strcmp(form, "voip_feature")) {
 	  voip_feature(inStr);
	}	else if (!strcmp(form, "voip_feature2")) {
 	  voip_feature2(inStr);
	}	else if (!strcmp(form, "voip_dialing")) {
 	  voip_dialing(inStr);
	} else if (!strcmp(form, "voip_dialing2")) {
 	  voip_dialing2(inStr);
	}else if (!strcmp(form, "voip_speed_dial")) {
 	  voip_speed_dial(inStr);
	} else if (!strcmp(form, "voip_speed_dial2")) {
 	  voip_speed_dial2(inStr);
	}else if (!strcmp(form, "dialRuleDelete")) {
 	  dialRuleDelete(inStr);
	}else if (!strcmp(form, "dialRuleDelete2")) {
 	  dialRuleDelete2(inStr);
	}else if (!strcmp(form, "voip_fax")) {
 	  voip_fax(inStr);
	}else if (!strcmp(form, "voip_fax2")) {
 	  voip_fax2(inStr);
	}else if (!strcmp(form, "voip_rtp")) {
 	  voip_rtp(inStr);
	}else if (!strcmp(form, "voip_rtp2")) {
 	  voip_rtp2(inStr);
	}else if (!strcmp(form, "voip_system")) {
 	  voip_system(inStr);
	}else if (!strcmp(form, "voip_media")) {
 	  voip_media(inStr);
	}else if (!strcmp(form, "voip_media")) {
 	  voip_media(inStr);
	}else if (!strcmp(form, "voip_qos")) {
 	  voip_qos(inStr);
	}else if (!strcmp(form, "voip_provision")) {
 	  voip_provision(inStr);
	}else if (!strcmp(form, "voip_phone")) {
 	  voip_phone(inStr);
	}else if (!strcmp(form, "voip_phone2")) {
 	  voip_phone2(inStr);
	}else if (!strcmp(form, "voip_voice")) {
 	  voip_voice(inStr);
	}else if (!strcmp(form, "voip_voice2")) {
 	  voip_voice2(inStr);
	}else if (!strcmp(form, "voip_profile")) {
 	  voip_profile(inStr);
	}else if (!strcmp(form, "voip_profile2")) {
 	  voip_profile2(inStr);
	}else if (!strcmp(form, "tr069")) {
 	  tr069(inStr);
	}




	free(inStr);
leave:	
	nvram_close(VOIP_NVRAM );

	return 0;
}