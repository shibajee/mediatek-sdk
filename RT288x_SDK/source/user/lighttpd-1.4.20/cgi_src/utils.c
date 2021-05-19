#include "utils.h"
#include <stdarg.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <linux/wireless.h>
#include "oid.h"


static char cmd[CMDLEN];
static FILE *log_fp;

/*
 * FOR CGI
 */
inline int get_one_port(void)
{
#if defined CONFIG_RAETH_ROUTER || defined CONFIG_MAC_TO_MAC_MODE || defined CONFIG_RT_3052_ESW
	return 0;
#elif defined CONFIG_ICPLUS_PHY
	return 1;
#else
	return 0;
#endif
	return 0;
}

int get_ifip(char *ifname, char *if_addr)
{	
	struct ifreq ifr;	
	int skfd = 0;	
	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {		
		printf("%s: open socket error\n", __FUNCTION__);		
		return -1;	
	}	
	strncpy(ifr.ifr_name, ifname, IF_NAMESIZE);	
	if (ioctl(skfd, SIOCGIFADDR, &ifr) < 0) {		
		close(skfd);
		return -1;	
	}	
	strcpy(if_addr, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));	
	close(skfd);	
	return 0;
}

int get_nth_value(int index, char *value, char delimit, char *result, int len)
{
	int i=0, result_len=0;
	char *begin, *end;

	if(!value || !result || !len)
		return -1;

	begin = value;
	end = strchr(begin, delimit);

	while(i<index && end){
		begin = end+1;
		end = strchr(begin, delimit);
		i++;
	}

	//no delimit
	if(!end){
		if(i == index){
			end = begin + strlen(begin);
			result_len = (len-1) < (end-begin) ? (len-1) : (end-begin);
		}else
			return -1;
	}else
		result_len = (len-1) < (end-begin)? (len-1) : (end-begin);

	memcpy(result, begin, result_len );
	*(result+ result_len ) = '\0';

	return 0;
}

char* get_field(char *a_line, char *delim, int count)
{
	int i=0;
	char *tok;
	tok = strtok(a_line, delim);
	while(tok){
		if(i == count)
			break;
		i++;
		tok = strtok(NULL, delim);
	}
	if(tok && isdigit(*tok))
		return tok;

	return NULL;
}

int get_nums(char *value, char delimit)
{
	char *pos = value;
	int count=1;

	if(!pos || !strlen(pos))
		return 0;
	while( (pos = strchr(pos, delimit))){
		pos = pos+1;
		count++;
	}
	return count;
}

int get_if_live(char *ifname)
{
	FILE *fp;
	char buf[256], *p;
	int i;

	if (NULL == (fp = fopen("/proc/net/dev", "r"))) {
		DBG_MSG("open /proc/net/dev error");
		return -1;
	}

	fgets(buf, 256, fp);
	fgets(buf, 256, fp);
	while (NULL != fgets(buf, 256, fp)) {
		i = 0;
		while (isspace(buf[i++]))
			;
		p = buf + i - 1;
		while (':' != buf[i++])
			;
		buf[i-1] = '\0';
		if (!strcmp(p, ifname)) {
			fclose(fp);
			return 1;
		}
	}
	fclose(fp);
	DBG_MSG("device %s not found", ifname);
	return 0;
}
int voip_delete_nth_value(int index[], int count, char *value, char delimit)
{
	char *begin, *end;
	int i=0,j=0;
	int need_check_flag=0;
	char *buf = strdup(value);

	begin = buf;

	end = strchr((begin+1), delimit);
	while(end){
		if(i == index[j]){
			memset(begin, 0, end - begin );
			if(index[j] == 0)
				need_check_flag = 1;
			j++;
			if(j >=count)
				break;
		}
		begin = end;

		end = strchr(begin+1, delimit);
		i++;
	}

	if(!end && index[j] == i)
		memset(begin, 0, strlen(begin));
/*
	if(need_check_flag){
		for(i=0; i<strlen(value); i++){
			if(buf[i] == '\0')
				continue;
			if(buf[i] == ';')
				buf[i] = '\0';
			break;
		}
	}
*/
	for(i=0, j=0; i<strlen(value); i++){
		if(buf[i] != '\0'){
			value[j++] = buf[i];
		}
	}
	value[j] = '\0';

	free(buf);
	return 0;
}


int delete_nth_value(int index[], int count, char *value, char delimit)
{
	char *begin, *end;
	int i=0,j=0;
	int need_check_flag=0;
	char *buf = strdup(value);

	begin = buf;

	end = strchr(begin, delimit);
	while(end){
		if(i == index[j]){
			memset(begin, 0, end - begin );
			if(index[j] == 0)
				need_check_flag = 1;
			j++;
			if(j >=count)
				break;
		}
		begin = end;

		end = strchr(begin+1, delimit);
		i++;
	}

	if(!end && index[j] == i)
		memset(begin, 0, strlen(begin));

	if(need_check_flag){
		for(i=0; i<strlen(value); i++){
			if(buf[i] == '\0')
				continue;
			if(buf[i] == ';')
				buf[i] = '\0';
			break;
		}
	}

	for(i=0, j=0; i<strlen(value); i++){
		if(buf[i] != '\0'){
			value[j++] = buf[i];
		}
	}
	value[j] = '\0';

	free(buf);
	return 0;
}

static char *set_nth_value(int index, char *old_values, char *new_value)
{
	int i;
	char *p, *q;
	static char ret[2048];
	char buf[8][256];

	memset(ret, 0, 2048);
	for (i = 0; i < 8; i++)
		memset(buf[i], 0, 256);

	//copy original values
	for ( i = 0, p = old_values, q = strchr(p, ';')  ;
			i < 8 && q != NULL                         ;
			i++, p = q + 1, q = strchr(p, ';')         )
	{
		strncpy(buf[i], p, q - p);
	}
	strcpy(buf[i], p); //the last one

	//replace buf[index] with new_value
	strncpy(buf[index], new_value, 256);

	//calculate maximum index
	index = (i > index)? i : index;

	//concatenate into a single string delimited by semicolons
	strcat(ret, buf[0]);
	for (i = 1; i <= index; i++) {
		strncat(ret, ";", 2);
		strncat(ret, buf[i], 256);
	}

	return ret;
}

void set_nth_value_flash(int nvram, int index, char *flash_key, char *value)
{
	char *result;
	char *tmp = (char *) nvram_bufget(nvram, flash_key);
	if(!tmp)
		tmp = "";
	result = set_nth_value(index, tmp, value);
	nvram_bufset(nvram, flash_key, result);
}

char *get_lanif_name(void)
{
	const char *mode;
	static char *if_name = "br0";

	mode = nvram_bufget(RT2860_NVRAM, "OperationMode");
	if (!strncmp(mode, "1", 2)) {
#if defined CONFIG_RAETH_ROUTER || defined CONFIG_MAC_TO_MAC_MODE || defined CONFIG_RT_3052_ESW
		if_name = "br0";
#elif defined  CONFIG_ICPLUS_PHY && CONFIG_RT2860V2_AP_MBSS
		char *num_s = nvram_bufget(RT2860_NVRAM, "BssidNum");
		if(strtol(num_s, NULL, 10) > 1) // multiple ssid
			if_name = "br0";
		else
			if_name = "ra0";
#elif defined CONFIG_GE1_RGMII_AN && defined CONFIG_GE2_RGMII_AN
		if_name = "br0";
#else
		if_name = "ra0";
#endif
	}

	return if_name;
}

char *get_wanif_name(void)
{
	const char *cm, *mode;
	static char *if_name = "br0"; 

	cm = nvram_bufget(RT2860_NVRAM, "wanConnectionMode");
	if (!strncmp(cm, "PPPOE", 6) || !strncmp(cm, "L2TP", 5) || !strncmp(cm, "PPTP", 5)
#ifdef CONFIG_USER_3G
			|| !strncmp(cm, "3G", 3)
#endif
	   ) {
		if_name = "ppp0";
	} else {
		mode = nvram_bufget(RT2860_NVRAM, "OperationMode");
		if (!strncmp(mode, "1", 2)) {
#if defined CONFIG_RAETH_ROUTER || defined CONFIG_MAC_TO_MAC_MODE || defined CONFIG_RT_3052_ESW
#if defined CONFIG_RAETH_SPECIAL_TAG
#if defined CONFIG_WAN_AT_P0
			if_name = "eth2.1";
#else
			if_name = "eth2.5";
#endif
#elif defined CONFIG_RAETH_GMAC2
			if_name = "eth3";
#else
			if_name = "eth2.2";
#endif
#elif defined CONFIG_GE1_RGMII_AN && defined CONFIG_GE2_RGMII_AN
			if_name = "eth3";
#else /* MARVELL & CONFIG_ICPLUS_PHY */
			if_name = "eth2";
#endif
		}
		else if (!strncmp(mode, "2", 2))
			if_name = "ra0";
		else if (!strncmp(mode, "3", 2))
			if_name = "apcli0";
	}

	return if_name;
}

char *get_macaddr(char *ifname)
{
	struct ifreq ifr;
	char *ptr;
	int skfd;
	static char if_hw[18] = {0};

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		fprintf(stderr, "%s: open socket error\n", __func__);
		return NULL;
	}
	strncpy(ifr.ifr_name, ifname, IF_NAMESIZE);
	if(ioctl(skfd, SIOCGIFHWADDR, &ifr) < 0) {
		close(skfd);
		fprintf(stderr, "%s: ioctl fail\n", __func__);
		return NULL;
	}

	ptr = (char *)&ifr.ifr_addr.sa_data;
	sprintf(if_hw, "%02X:%02X:%02X:%02X:%02X:%02X",
			(ptr[0] & 0377), (ptr[1] & 0377), (ptr[2] & 0377),
			(ptr[3] & 0377), (ptr[4] & 0377), (ptr[5] & 0377));
	close(skfd);

	return if_hw;
}

char *get_ipaddr(char *ifname)
{
	struct ifreq ifr;
	int skfd = 0;
	static char if_addr[16];

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		fprintf(stderr, "%s: open socket error\n", __func__);
		return "";
	}

	strncpy(ifr.ifr_name, ifname, IF_NAMESIZE);
	if (ioctl(skfd, SIOCGIFADDR, &ifr) < 0) {
		close(skfd);
		fprintf(stderr, "%s: ioctl SIOCGIFADDR error for %s\n", __func__, ifname);
		return "";
	}
	strcpy(if_addr, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
	close(skfd);

	return if_addr;
}

char *get_netmask(char *ifname)
{
	struct ifreq ifr;
	int skfd = 0;
	static char addr[16];

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		fprintf(stderr, "%s: open socket error\n", __func__);
		return "";
	}

	strncpy(ifr.ifr_name, ifname, IF_NAMESIZE);
	if (ioctl(skfd, SIOCGIFNETMASK, &ifr) < 0) {
		close(skfd);
		fprintf(stderr, "%s: ioctl SIOCGIFNETMASK error for %s\n", __func__, ifname);
		return "";
	}
	strcpy(addr, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
	close(skfd);

	return addr;
}

int get_index_routingrule(const char *rrs, char *dest, char *netmask, char *interface)
{
	int index=0;
	char one_rule[256];
	char dest_f[32], netmask_f[32], interface_f[32];

	if(!rrs || !strlen(rrs))
		return -1;

	while( get_nth_value(index, (char *)rrs, ';', one_rule, 256) != -1 ){
		if((get_nth_value(0, one_rule, ',', dest_f, sizeof(dest_f)) == -1)){
			index++;
			continue;
		}
		if((get_nth_value(1, one_rule, ',', netmask_f, sizeof(netmask_f)) == -1)){
			index++;
			continue;
		}
		if((get_nth_value(4, one_rule, ',', interface_f, sizeof(interface_f)) == -1)){
			index++;
			continue;
		}

		if( (!strcmp(dest, dest_f)) && (!strcmp(netmask, netmask_f)) && (!strcmp(interface, interface_f))){
			return index;
		}
		index++;
	}

	return -1;
}

void getCurrentWscProfile(char *interface, WSC_CONFIGURED_VALUE *data, int len)
{
	int socket_id;
	struct iwreq wrq;

	if ((socket_id = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		DBG_MSG("socket open Fail !");
	strcpy((char *)data, "get_wsc_profile");
	strcpy(wrq.ifr_name, interface);
	wrq.u.data.length = len;
	wrq.u.data.pointer = (caddr_t) data;
	wrq.u.data.flags = 0;
	if (ioctl(socket_id, RTPRIV_IOCTL_WSC_PROFILE, &wrq) < 0) {
		DBG_MSG("ioctl -> RTPRIV_IOCTL_WSC_PROFILE Fail !");
	}
	close(socket_id);
}

int getWscStatus(char *interface)
{
	int socket_id;
	struct iwreq wrq;
	int data = 0;
	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	strcpy(wrq.ifr_name, interface);
	wrq.u.data.length = sizeof(data);
	wrq.u.data.pointer = (caddr_t) &data;
	wrq.u.data.flags = RT_OID_WSC_QUERY_STATUS;
	if( ioctl(socket_id, RT_PRIV_IOCTL, &wrq) == -1)
		DBG_MSG("ioctl error");
	close(socket_id);

	return data;
}

void getWscProfile(char *interface, WSC_PROFILE *wsc_profile)
{
	int socket_id;
	struct iwreq wrq;

	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	strcpy(wrq.ifr_name, interface);
	wrq.u.data.length = sizeof(WSC_PROFILE);
	wrq.u.data.pointer = (caddr_t) wsc_profile;
	wrq.u.data.flags = RT_OID_802_11_WSC_QUERY_PROFILE;
	if( ioctl(socket_id, RT_PRIV_IOCTL, &wrq) == -1)
		DBG_MSG("ioctl error\n");
	close(socket_id);
}

#if defined CONFIG_USER_STORAGE
FILE *fetch_media_cfg(struct media_config *cfg, int write)
{
	FILE *mfp = fopen(MOUNT_INFO, "r");
	FILE *rfp = NULL;
	static FILE *wfp = NULL;
	char path[50];

	if (NULL == mfp) {
		DBG_MSG("fopen %s fail!", MOUNT_INFO);
		goto final;
	}
	while(EOF != fscanf(mfp, "%*s %s %*s %*s %*s %*s\n", path))
	{
		if (!strncmp(path, "/media/sd", 9) || 
		    !strncmp(path, "/media/mmc", 10)) {
			sprintf(path, "%s/.media_config", path);
			if (NULL != (rfp = fopen(path, "r"))) {
				//DBG_MSG(".media_config get it");
				break;
			}
		}
	}
	if (rfp != NULL)
		fread(cfg, 4*sizeof(struct media_config), 1, rfp);
	else
		goto final;
	fclose(mfp);
	fclose(rfp);
final:
	if (write) {
		wfp = fopen(path, "w");
		if (wfp == NULL)
			DBG_MSG("create %s fail", path);
	}

	return wfp;
}
#endif

static int is_allnum_slash(char *str)
{
	int i, len = strlen(str);
	for (i=0; i<len; i++) {
		if( (str[i] >= '0' && str[i] <= '9') || str[i] == '.' || str[i] == '/' )
			continue;
		return 0;
	}
	return 1;
}

static int is_num_only(char *str){
	int i, len = strlen(str);
	for (i=0; i<len; i++) {
		if((str[i] >= '0' && str[i] <= '9'))
			continue;
		return 0;
	}
	return 1;
}

static int is_one_slash(char *str)
{
	int i, count=0;
	int len = strlen(str);
	for(i=0; i<len; i++)
		if( str[i] == '/')
			count++;
	return count <= 1 ? 1 : 0;
}

int is_ipnetmask_valid(char *s)
{
	char str[32];
	char *slash;
	struct in_addr addr;    // for examination

	if(!s || !strlen(s)){
		return 0;
	}

	strncpy(str, s, sizeof(str));

	if( (!strcmp("any", str)) || (!strcmp("any/0", str)))
		return 1;

	if (!is_allnum_slash(str)){
		return 0;
	}

	if(!is_one_slash(str)){
		return 0;
	}

	slash = strchr(str, '/');
	if(slash){
		int mask;

		*slash = '\0';
		slash++;
		if(!strlen(slash)){
			return 0;
		}

		if(!is_num_only(slash)){
			return 0;
		}

		mask = strtol(slash, NULL, 10);
		if(mask < 0 || mask > 32){
			return 0;
		}
	}

	if(! (inet_aton(str, &addr))){
		fprintf(stderr, "%s: %s is not a valid IP address.\n", __func__, str);
		return 0;
	}
	return 1;
}

int is_ip_valid(char *str)
{
	struct in_addr addr;    // for examination
	if( (! strcmp("any", str)) || (! strcmp("any/0", str)))
		return 1;

	if(! (inet_aton(str, &addr))){
		fprintf(stderr, "%s: %s is not a valid IP address.\n", __func__, str);
		return 0;
	}
	return 1;
}

int is_mac_valid(char *str)
{
	int i, len = strlen(str);
	if(len != 17)
		return 0;

	for(i=0; i<5; i++){
		if( (!isxdigit( str[i*3])) || (!isxdigit( str[i*3+1])) || (str[i*3+2] != ':') )
			return 0;
	}
	return (isxdigit(str[15]) && isxdigit(str[16])) ? 1: 0;
}

int is_oneport_only(void)
{
#if defined CONFIG_RAETH_ROUTER || defined CONFIG_MAC_TO_MAC_MODE || defined CONFIG_RT_3052_ESW
	return 0;
#elif defined CONFIG_ICPLUS_PHY
	return 1;
#else
	return 0;
#endif
	return 0;
}

char *strip_space(char *str)
{
	while( *str == ' ')
		str++;
	return str;
}

char *racat(char *s, int i)
{
	static char str[32];
	snprintf(str, 32, "%s%1d", s, i);
	return str;
}

static void unencode(char *src, char *last, char *dest)
{
	for(; src != last; src++, dest++)
		if(*src == '+')
			*dest = ' ';
		else if(*src == '%') {
			int code;
			if(sscanf(src+1, "%2x", &code) != 1) code = '?';
			*dest = code;
			src +=2; 
		} else
			*dest = *src;
} 

int netmask_aton(const char *ip)
{
	int i, a[4], result = 0;
	sscanf(ip, "%d.%d.%d.%d", &a[0], &a[1], &a[2], &a[3]);
	for(i=0; i<4; i++){ //this is dirty
		if(a[i] == 255){
			result += 8;
			continue;
		}
		if(a[i] == 254)
			result += 7;
		if(a[i] == 252)
			result += 6;
		if(a[i] == 248)
			result += 5;
		if(a[i] == 240)
			result += 4;
		if(a[i] == 224)
			result += 3;
		if(a[i] == 192)
			result += 2;
		if(a[i] == 128)
			result += 1;
		//if(a[i] == 0)
		//  result += 0;
		break;
	}
	return result;
}

void convert_string_display(char *str)
{
	int  len, i;
	char buffer[193];
	char *pOut;

	memset(buffer,0,193);
	len = strlen(str);
	pOut = &buffer[0];

	for (i = 0; i < len; i++) {
		switch (str[i]) {
		case 38:
			strcpy (pOut, "&amp;");		// '&'
			pOut += 5;
			break;

		case 60:
			strcpy (pOut, "&lt;");		// '<'
			pOut += 4;
			break;

		case 62:
			strcpy (pOut, "&gt;");		// '>'
			pOut += 4;
			break;

		case 34:
			strcpy (pOut, "&#34;");		// '"'
			pOut += 5;
			break;

		case 39:
			strcpy (pOut, "&#39;");		// '''
			pOut += 5;
			break;
		case 32:
			strcpy (pOut, "&nbsp;");	// ' '
			pOut += 6;
			break;

		default:
			if ((str[i]>=0) && (str[i]<=31)) {
				//Device Control Characters
				sprintf(pOut, "&#%02d;", str[i]);
				pOut += 5;
			} else if ((str[i]==39) || (str[i]==47) || (str[i]==59) || (str[i]==92)) {
				// ' / ; (backslash)
				sprintf(pOut, "&#%02d;", str[i]);
				pOut += 5;
			} else if (str[i]>=127) {
				//Device Control Characters
				sprintf(pOut, "&#%03d;", str[i]);
				pOut += 6;
			} else {
				*pOut = str[i];
				pOut++;
			}
			break;
		}
	}
	*pOut = '\0';
	strcpy(str, buffer);
}

void do_system(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vsprintf(cmd, fmt, ap);
	va_end(ap);
	sprintf(cmd, "%s 1>%s 2>&1", cmd, LOGFILE);
	system(cmd);

	return;
}

int check_semicolon(char *str)
{
	char *c = strchr(str, ';');
	if (c)
		return 1;
	return 0;
}

void free_all(int num, char *fmt, ...)
{
	va_list ap;
	char *ptr;

	va_start(ap, fmt);
	ptr = fmt;
	do { 
		if (!ptr)
			free(ptr);
		ptr = va_arg(ap, char *);
	} while(--num > 0);
	va_end(ap);

	return;
}

/* 
 * FOR PROCESS
 */
#if 0
void dhcpc_handler(int signum)
{
	firewall_init();
	ripdRestart();
	do_system("/sbin/config-igmpproxy.sh");
#ifdef CONFIG_RALINKAPP_SWQOS
	qos_init();
#endif
#ifdef CONFIG_RALINKAPP_HWQOS
	qos_init();
#endif
#if defined (CONFIG_IPV6)
	ipv6Config(strtol(nvram_bufget(RT2860_NVRAM, "IPv6OpMode"), NULL, 10));
#endif
}
#endif

void update_flash_8021x(int nvram)
{
	char *lan_if_addr;
	const char *RADIUS_Server;
	const char *operation_mode;

	if(!(operation_mode = nvram_get(nvram, "OperationMode")))
		return;

	if(!(RADIUS_Server = nvram_bufget(nvram, "RADIUS_Server")))
		return;

	if(!strlen(RADIUS_Server))
		return;

	if ((*operation_mode == '0') || (*operation_mode == '1') || 
	    (*operation_mode == '2')) { 	// Bridge/Gateway/Wireless gateway
		char *if_name = get_lanif_name();
		if ((lan_if_addr = get_ipaddr(if_name)) == NULL)
			return;
		nvram_bufset(nvram, "own_ip_addr", lan_if_addr);
		nvram_bufset(nvram, "EAPifname", if_name);
		nvram_bufset(nvram, "PreAuthifname", if_name);
		nvram_commit(nvram);
	}
}



/* 
 * FOR WEB
 */
void web_redirect(const char *url)
{
	printf ("HTTP/1.0 %d %s\r\n", 302, "Redirect");
	printf ("Pragma: no-cache\r\nCache-Control: no-cache\r\n");
	printf ("Content-Type: text/html\r\n");
	printf ("Location: %s\r\n", url);
	printf ("\r\n"); /* end of header */
}

void web_redirect_wholepage(const char *url)
{
	printf("HTTP/1.1 200 OK\n");
	printf("Content-type: text/html\n");
	printf("Pragma: no-cache\n");
	printf("Cache-Control: no-cache\n");
	printf("\n");
	printf("<html><head><script language=\"JavaScript\">");
	printf("parent.location.replace(\"%s\");", url);
	printf("</script></head></html>");
}

void web_back_parentpage(void)
{
	printf("HTTP/1.0 200 OK\n");
	printf("Pragma: no-cache\n");
	printf("Content-Type: text/html\n");
	printf("Cache-control: no-cache\n");
	printf("\n");
	printf("<html>\n<head>\n");  
	printf("<title>My Title</title>");
	printf("<link rel=\"stylesheet\" href=\"/style/normal_ws.css\" type=\"text/css\">");
	printf("<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">");
	printf("</head>\n<body onload=\"opener.location.reload();window.close();\"></html>\n");
}

void web_debug_header(void)
{
	printf("Content-Type: text/plain;charset=utf-8\n\n");
}

char *web_get(char *tag, char *input, int dbg)	
{	
	char *e_begin, *v_begin, *v_end;
	static char ret[DATALEN];
	int v_len;

	sprintf(ret, "&%s=", tag);
	if (NULL == (e_begin = strstr(input, ret))) { 
		sprintf(ret, "%s=", tag);
		if (NULL == (e_begin = strstr(input, ret)) || e_begin != input)
			return "";
	}
	memset(ret, 0, DATALEN);
	v_begin = strchr(e_begin, '=') + 1;
	if ((NULL != (v_end = strchr(v_begin, '&')) || 
	     NULL != (v_end = strchr(v_begin, '\0'))) &&
	    (0 < (v_len = v_end - v_begin)))
		unencode(v_begin, v_end, ret);
	if (dbg == 1)
		printf("%s = %s\n", tag, ret);

	return ret;
}

