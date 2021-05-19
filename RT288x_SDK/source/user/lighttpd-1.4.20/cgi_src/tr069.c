#include "utils.h"
#include <unistd.h>	//for unlink
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <net/route.h>
#include "openssl/pem.h"
#include "openssl/x509.h"
#include "openssl/evp.h"
#include "user_conf.h"
#include "busybox_conf.h"
#define RFC_ERROR "RFC1867 ...."

#define REFRESH_TIMEOUT		"60000"		/* 40000 = 40 secs*/
#define UPLOAD_FILE "/var/tmpCERTtr069"

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
		//nvram_bufset(VOIP_NVRAM, "TR_CR_PATH", caCf);
		//nvram_bufset(VOIP_NVRAM, "TR_CR_PASS", cCif);		
				
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
   if (!strcmp(form, "tr069")) {
 	  tr069(inStr);
	}




	free(inStr);
leave:	
	nvram_close(VOIP_NVRAM );

	return 0;
}