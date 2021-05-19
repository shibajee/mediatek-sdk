/************************************************************************
 *
 *	Copyright (C) 2006 Trendchip Technologies, Corp.
 *	All Rights Reserved.
 *
 * Trendchip Confidential; Need to Know only.
 * Protected as an unpublished work.
 *
 * The computer program listings, specifications and documentation
 * herein are the property of Trendchip Technologies, Co. and shall
 * not be reproduced, copied, disclosed, or used in whole or in part
 * for any reason without the prior express written permission of
 * Trendchip Technologeis, Co.
 *
 *************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
//-#include "../../../version/tcversion.h"   
//-#include "../lib/libtcapi.h"

#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_C1_OBM)
#include "../../../tools/trx/trx.h"
#endif/*TCSUPPORT_COMPILE*/
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_C2_TRUE) || defined(TCSUPPORT_C1_OBM)
#include	<sys/stat.h>
#endif/*TCSUPPORT_COMPILE*/

#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_BOOTLOADER_MODIFY_PVNAME)|| defined(TCSUPPORT_C1_OBM)
#define PRODUCTNAME_ADDR_BOOTLOADER 0x0000ff24L	
#define VENDORNAME_ADDR_BOOTLOADER 0x0000ff04L
#define PRODUCTNAMELEN 32
#define VENDORNAMELEN 32
#endif/*TCSUPPORT_COMPILE*/

#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_C2_TRUE) || defined(TCSUPPORT_C1_OBM)
#define DEFAULT_ROMFILE_PATH "/userfs/romfile.cfg"
#endif/*TCSUPPORT_COMPILE*/

#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_C1_OBM)
typedef unsigned char uint8;
#define DEVICEINFO	"DeviceInfo"
#define FWVER 		"FwVer"
#define O_RDONLY 	"0x0000"
#define MAX_FWVER_LEN	32
#define BOOLOADER_VERSION		"1.0"
#define FW_PATH		"/dev/mtd2"
#define BOOT_PATH 	"/dev/mtd0"
#define ETHER_ADDR_BOOTLOADER 0x0000ff48L	
#define SNMP_SYSOBJ_BOOTLOADER 0x0000ff4eL
#define COUNTYR_CODE_BOOTLOADER 0x0000ff64L
#define ETHERLEN 6
#define SNMPSYSOBJLEN 22
#define COUNTRYCODELEN 1
#endif/*TCSUPPORT_COMPILE*/
typedef int (*func_t)(int argc,char *argv[]);

static int doSysMac(int argc, char *argv[]);
static int dummy(int argc, char *argv[]);
#ifdef TC2031_DEBUG
static int tcipaddr(int argc, char *argv[]);
static int tcsetip(int argc, char *argv[]);
static int tcping(int argc, char *argv[]);
#endif
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_BOOTLOADER_MODIFY_PVNAME)
static int doSysAtsp(int argc, char *argv[]);
static int doSysAtsv(int argc, char *argv[]);
static int doSysAtspv(int argc, char *argv[]);
#endif/*TCSUPPORT_COMPILE*/
#if defined(TCSUPPORT_LED_BTN_CHECK) || defined(TCSUPPORT_TEST_LED_ALL) 
#if defined(TCSUPPORT_LED_CHECK) || defined(TCSUPPORT_TEST_LED_ALL) 
static int 	doLedCheck(int argc, char *argv[]);
#endif
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_BTN_CHECK)
static int 	doButtonCheck(int argc, char *argv[]);
#endif/*TCSUPPORT_COMPILE*/
#endif
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_C1_NEW_GUI)
static int 
doRomReset(int argc, char *argv[]);
#endif/*TCSUPPORT_COMPILE*/
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_C2_TRUE) || defined(TCSUPPORT_C1_OBM)
unsigned short Calculate_Checksum(unsigned short *start, unsigned long int len);
static int 
doRomCheck(int argc, char *argv[]);
#endif/*TCSUPPORT_COMPILE*/

#if defined(TCSUPPORT_CMDPROMPT)
static int 
doSysSubcmdsPrompt(int argc, char *argv[]);
static int 
doWan2lanPrompt(int argc, char *argv[]);
#endif

#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_C1_OBM)
static int doSysAtsh(int argc, char *argv[]);
static int doCheckImage(void);
static int doCheckBootbase(void);
#endif/*TCSUPPORT_COMPILE*/

func_t ci_func[] ={
    dummy,
    doSysMac,
#ifdef TC2031_DEBUG
    tcipaddr,
    tcsetip,
    tcping,
#else/* To ensure index */
    NULL,
    NULL,
    NULL,
#endif    
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_C1_NEW_GUI)
    doRomReset,
#else/*TCSUPPORT_COMPILE*/
    NULL,
#endif/*TCSUPPORT_COMPILE*/
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_BOOTLOADER_MODIFY_PVNAME)
	doSysAtsp,
	doSysAtsv,
	doSysAtspv,  
#else/*TCSUPPORT_COMPILE*/
	NULL,
	NULL,
	NULL,
#endif/*TCSUPPORT_COMPILE*/    
#if defined(TCSUPPORT_LED_BTN_CHECK) || defined(TCSUPPORT_TEST_LED_ALL) 
#if defined(TCSUPPORT_LED_CHECK) || defined(TCSUPPORT_TEST_LED_ALL) 
	doLedCheck,
#else
	NULL,
#endif
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_BTN_CHECK)
	doButtonCheck,
#else/*TCSUPPORT_COMPILE*/
	NULL,
#endif/*TCSUPPORT_COMPILE*/
#else
	NULL,
	NULL,
#endif
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_C2_TRUE)
	doRomCheck,
#else/*TCSUPPORT_COMPILE*/
	NULL,
#endif/*TCSUPPORT_COMPILE*/
#if defined(TCSUPPORT_CMDPROMPT)
	doSysSubcmdsPrompt,
	doWan2lanPrompt,
#else
	NULL,
	NULL,
#endif
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_C1_OBM)
	doSysAtsh,
#else/*TCSUPPORT_COMPILE*/
	NULL,
#endif/*TCSUPPORT_COMPILE*/
    NULL  
};

static int dummy(int argc, char *argv[]){
    printf("I am dummy!!\n");
    return 0;
}
#ifdef TC2031_DEBUG
/*_____________________________________________________________________________
**      function name: tcipaddr
**      descriptions:
**           show lan ip addr 
**             
**      parameters:
**            argc : argument number
**            argv : argument point 
**		 p     :  no use
**      global:
**            None
**             
**      return:
**            none
**	     
**      call:
**      	
**      revision:
**      
**____________________________________________________________________________
*/
static int tcipaddr(int argc, char *argv[]){
	
	FILE *fp;
	char buf[256];
	char *point;
	char x=0;
	
	system("ifconfig br0 > /tmp/var/ifconfig.tmp");
	fp = fopen("/tmp/var/ifconfig.tmp","r");
	if(fp != NULL){
		while(fgets(buf,256,fp)){
			point=strstr(buf,"inet addr:");
			if(point != NULL){
				printf("%s(set)\n",strtok(point+strlen("inet addr:")," "));
				break;
			}else{
				continue;
			}
			x++;
			if(x>10){
				break;
			}
		}
		fclose(fp);
		unlink("/tmp/var/ifconfig.tmp");
	}else{
		printf("Get ip addr fail!!\n");
	}

	return 0;
}

/*_____________________________________________________________________________
**      function name: tcsetip
**      descriptions:
**           set lan ip addr 
**             
**      parameters:
**            argc : argument number
**            argv : argument point 
**		 p     :  no use
**      global:
**            None
**             
**      return:
**            none
**	     
**      call:
**      	
**      revision:
**      
**____________________________________________________________________________
*/
static int tcsetip(int argc, char *argv[]){
	char buf[256];

	if(argc > 4){
		/*
		sprintf(buf,"ifconfig br0 %s netmask %s",argv[3],argv[4]);
		*/
		sprintf(buf,"ifconfig br0 %s netmask %s;tcapi set Lan_Entry0 IP %s;tcapi save;", \
				argv[3], argv[4],argv[3]);
	}else{
		sprintf(buf,"ifconfig br0 %s ",argv[3]);
	}

	system(buf);
	return 0;
}
/*_____________________________________________________________________________
**      function name: tcping
**      descriptions:
**           do ping action
**             
**      parameters:
**            argc : argument number
**            argv : argument point 
**		 p     :  no use
**      global:
**            None
**             
**      return:
**            none
**	     
**      call:
**      	
**      revision:
**      
**____________________________________________________________________________
*/
static int tcping(int argc, char *argv[]){
	char buf[256];
	int interval;
	
	if(argc >= 5){	
		interval = atoi(argv[4]);
		if(interval < 0){
			interval = 0;
		}
	}else{
		interval = 0;
	}
	
	sprintf(buf,"/userfs/bin/sendicmp %s %s %s %d",argv[1],argv[2],argv[3],interval);
	system(buf);
	return 0;
}

#endif

#if defined(TCSUPPORT_FON)
int hextoInt(char ch)
{
	int ret = 0;
	if (ch >= '0' && ch <= '9')
		ret = ch - '0';
	if (ch >= 'a' && ch <= 'f')
		ret = 10 + ch - 'a';
	if (ch >= 'A'  && ch <= 'F')
		ret = 10 + ch - 'A';
	return ret;
}

char intToHex(int i)
{
	char ret = 0;
	if(i>=0 && i <=9)
		ret = i + '0';
	if(i>=10 && i <=15)
		ret = i+'a'-10;
	return ret;
}
#endif
static int doSysMac(int argc, char *argv[]){
    unsigned char len, tmp[3], i;
    unsigned char mac_addr[6] = {0};
    FILE *fp;    
#if defined(TCSUPPORT_FON)
    unsigned char fon_tmp[80],syscmd[128],j;
    unsigned int seed = 1;
#endif

//    printf("MAC address=%s\n", argv[1]);    
    /* check MAC address is valid or not */
    len = strlen(argv[1]);
	if (len != 12) {
		printf("mac address must be 12 digits\n");
		return -1;
	}
	   
#if defined(TCSUPPORT_FON)
	memset(fon_tmp,0,sizeof(fon_tmp));
	memset(syscmd,0,sizeof(syscmd));
	for(i=1,j=0;i<18;i++)
	{
		if (i%3==0)
		{
			fon_tmp[i-1] = '-';
		}
		else
		{
			fon_tmp[i-1] = argv[1][j];
			j++;
		}
	}
	//printf("\r\nfon_mac=%s",fon_tmp);
	sprintf(syscmd,"/usr/bin/prolinecmd fonMac set %s",fon_tmp);
	system(syscmd);

	memset(syscmd,0,sizeof(syscmd));
	memset(fon_tmp,0,sizeof(fon_tmp));
	seed = hextoInt(argv[1][8])*4096+hextoInt(argv[1][9])*256+hextoInt(argv[1][10])*16+hextoInt(argv[1][11]);
	seed += (unsigned) time(NULL) + getpid();
	srand(seed);
	for(i=0;i<64; i++)
	{
		fon_tmp[i] = intToHex(rand()%16);
	}
	//printf("\r\nfon_keyword=%s",fon_tmp);
	sprintf(syscmd,"/usr/bin/prolinecmd fonKeyword set %s",fon_tmp);
	system(syscmd);
#endif
    
    /* Read bootloader from flash */
    system("cat /dev/mtd0 > /tmp/boot.bin");
    
    /* Modify the MAC address in the bootloader */  
	tmp[2] = 0;
    for(i = 0; i < 6; i++){
		tmp[0] = argv[1][2*i];
		tmp[1] = argv[1][2*i+1];
		mac_addr[i] = (unsigned char)strtoul(tmp, NULL, 16);
	}
	
	printf("new mac addr = ");
    for(i = 0; i < 5; i++)    
        printf("%x:", mac_addr[i]);
    printf("%x\n", mac_addr[5]);
    
    fp = fopen("/tmp/boot.bin", "r+b");
    if( fp == NULL ){
        printf("open file error!!\n");
        system("rm /tmp/boot.bin");        
        return -1;
    }
    
    fseek(fp, 0x0000ff48L, SEEK_SET);
    fwrite(mac_addr, 6, 1, fp );
    fflush(fp);
    fclose(fp);
        
    /* Write the bootloader back to flash and reboot*/
    system("/userfs/bin/mtd -rf write /tmp/boot.bin bootloader");
   
    return 0;
}
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_BOOTLOADER_MODIFY_PVNAME)
//this compile option is used by multi customers.
static int doSysAtsp(int argc, char *argv[])
{
	int ret=0;
	
	if(argc == 2)
	{
		if(strlen(argv[1]) >= PRODUCTNAMELEN)
		{
			printf("ERR: ProductName len must be less lan 32\r\n");
			return -1;
		}
		else
		{			
			ret = tcapi_set("SysInfo_Entry", "ProductName", argv[1]);
		}
		
		if(ret != 0)
		{
			return -1;
		}

		tcapi_save();
		printf("set ProductName success!\r\n");
		
	}
	return 0;
	
}

static int doSysAtsv(int argc, char *argv[])
{
	int ret=0;

	if(argc == 2)
	{
		if(strlen(argv[1]) >= VENDORNAMELEN)
		{
			printf("ERR: VendorName len must be less lan 32\r\n");
			return -1;
		}
		else
		{
			ret = tcapi_set("SysInfo_Entry", "Vendor", argv[1]);
		}
		
		if(ret != 0)
		{
			return -1;
		}

		tcapi_save();
		printf("set VendorName success\r\n");
	}

	return 0;
}


static int doSysAtspv(int argc, char *argv[])
{
	char ProductName[PRODUCTNAMELEN]={0};
	char VendorName[VENDORNAMELEN]= {0};
	int ret = 0;
	FILE *fp = NULL;  
	
	if(argc == 2)
	{
		if(strcmp(argv[1], "show") == 0)
		{
		
			system("cat /dev/mtd0 > /tmp/boot.bin");

			fp = fopen("/tmp/boot.bin", "r+b");
			if( fp == NULL ){
				printf("open file error!!\n");    
				return -1;
			}
    
			fseek(fp, PRODUCTNAME_ADDR_BOOTLOADER, SEEK_SET);
			fread(ProductName, sizeof(ProductName), 1, fp );
    
			fseek(fp, VENDORNAME_ADDR_BOOTLOADER, SEEK_SET);
			fread(VendorName, sizeof(VendorName), 1, fp );
			fclose(fp);
			
			printf("ProductName: %s\r\n",ProductName);
			printf("VendorName: %s\r\n",VendorName);
			
			return 0;			
		}		
	}
	if(0 != (ret = tcapi_get("SysInfo_Entry","ProductName",ProductName)))
	{
		return -1;
	}
	
	if(0 != (ret = tcapi_get("SysInfo_Entry","Vendor",VendorName)))
	{
		return -1;
	}
	
	system("cat /dev/mtd0 > /tmp/boot.bin");

	fp = fopen("/tmp/boot.bin", "r+b");
	if( fp == NULL ){
		printf("open file error!!\n");    
		return -1;
	}
    
	fseek(fp, PRODUCTNAME_ADDR_BOOTLOADER, SEEK_SET);
	fwrite(ProductName, sizeof(ProductName), 1, fp );
    
	fseek(fp, VENDORNAME_ADDR_BOOTLOADER, SEEK_SET);
	fwrite(VendorName, sizeof(VendorName), 1, fp );
    
	fflush(fp);
	fclose(fp);
  
	/* Write the bootloader back to flash and reboot*/
	system("/userfs/bin/mtd -rf write /tmp/boot.bin bootloader");
	return 0;
}
#endif/*TCSUPPORT_COMPILE*/
#if defined(TCSUPPORT_LED_BTN_CHECK) || defined(TCSUPPORT_TEST_LED_ALL) 
#if defined(TCSUPPORT_LED_CHECK) || defined(TCSUPPORT_TEST_LED_ALL) 
static int doLedCheck(int argc, char *argv[])
{
	if((argc == 2) && ((strcmp(argv[1], "on") == 0) || (strcmp(argv[1], "off") == 0))){
		if(strcmp(argv[1], "off") == 0){
			printf("All led is turned off! \r\n");	
		 }	
		else{
			printf("All led is turned on! \r\n");
		}
	}
	else{
		printf("Usage: sys led [on||off] \r\n");
		return -1;
	}
	return 0;
}	
#endif
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_BTN_CHECK)
static int doButtonCheck(int argc, char *argv[])
{
	if(argc != 2 || (strcmp(argv[1], "enable") && strcmp(argv[1], "disable"))){
		printf("Usage: sys button [enable||disable]\r\n");
		return -1;
	}
	if (strcmp(argv[1], "disable") == 0){
		printf("All buttons are disabled! \r\n");
	}	
	else{
		printf("All buttons are enabled! \r\n");	
	 }

	return 0;
}	
#endif/*TCSUPPORT_COMPILE*/
#endif
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_C1_NEW_GUI)
/*_____________________________________________________________________________
**      function name: doRomReset
**      descriptions:
**           reset  romfile to default
**             
**      parameters:
**            argc : argument number
**            argv : argument point 
**      global:
**            None
**             
**      return:
**            none
**	     
**      call:
**      	
**      revision:
**	   1. Jianhua 2010/8/9
**      
**____________________________________________________________________________
*/
static int 
doRomReset(int argc, char *argv[]) 
{
	int reboot = 0;
	int ret = 0;

	if(argc < 2 || strcmp(argv[0], "romreset") != 0 || !isdigit(argv[1][0])){
		ret = -1;
		goto err;
	}
	
	reboot =  atoi(argv[1]);
	
	if(reboot == 1){
		system("/userfs/bin/mtd -r write /userfs/romfile.cfg romfile");
	}
	else if(reboot == 0){
		system("/userfs/bin/mtd write /userfs/romfile.cfg romfile");
		//tcapi_readAll();
	}
	else{
		ret = -1;
	}

err:
	if(ret == -1){
		printf("Bad command!\nUsage: sys romreset 0|1\n");
	}
	
	return ret;
}
#endif/*TCSUPPORT_COMPILE*/

#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_C2_TRUE) || defined(TCSUPPORT_C1_OBM)
unsigned short Calculate_Checksum(unsigned short *start, unsigned long int len)
{
    unsigned short *ptr;
    unsigned short *endptr;
    unsigned long int checksum = 0;

    ptr = start;
    endptr = start + (len/sizeof(unsigned short)) + (len%sizeof(unsigned short));

    while (ptr < endptr)
    {
        checksum += *ptr;

        if (checksum > 0xFFFF)
            checksum = (checksum + 1) & 0xFFFF;
        ptr++;
    }
    if ((len & 0x01) != 0)
    {
        checksum += (*ptr & 0xFF00);

        if (checksum > 0xFFFF)
            checksum = (checksum + 1) & 0xFFFF;
    }
    return((unsigned short)checksum);
}

static int doRomCheck(int argc, char *argv[])
{
	char *str;
	FILE *fp = NULL;
	struct stat fs;
	int  fd=0, ret=0;
	
	if(stat(DEFAULT_ROMFILE_PATH, &fs) != 0)
		return -1;

	str = (char *)malloc(fs.st_size+1);
	if(str == NULL)
		return -1;

	fd = open(DEFAULT_ROMFILE_PATH, 0);
	if(fd == -1){
		return -1;
	}

	memset(str,0, fs.st_size+1);
	ret = read(fd, str, fs.st_size);
	if(ret <= 0){
		close(fd);
		return -1;
	}
	close(fd);
	
	printf("RomFile Checksum	: %04x\n\r", Calculate_Checksum((unsigned short*)str, fs.st_size));
	
	if(str != NULL)
		free(str);

	return 0;
}
#endif/*TCSUPPORT_COMPILE*/
#if defined(TCSUPPORT_CMDPROMPT)
static int doSysSubcmdsPrompt(int argc, char *argv[]){
	system("cat proc/kprofile/sys_subcmds");
	return 0;
}

static int doWan2lanPrompt(int argc, char *argv[]){
	system("cat proc/kprofile/sys_wan2lan");
	return 0;
}
#endif

#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_C1_OBM)

static int doCheckImage(void)
{
	int fd = 0;
	int buflen = 0;
	char img_buf[sizeof(struct trx_header)];
	struct trx_header *trx = (struct trx_header *) img_buf;
	
	fd = open(FW_PATH,O_RDONLY);
	if( fd < 0){
		return -1;
	}
	buflen = read(fd, img_buf, sizeof(struct trx_header));
	if (buflen < sizeof(struct trx_header)) {
		return -1;
	}
	printf("fw size			: %d\n",trx->header_len + trx->kernel_len + trx->rootfs_len);
	printf("fw crc32		: %04x\n",trx->crc32 );

	close(FW_PATH);

	return 0;
}

static int doCheckBootbase(void)
{
	FILE *fp = NULL;  
	char buf[64] = {0};
	int i = 0;
	
	fp = fopen(BOOT_PATH, "r+b");
	if( fp == NULL ){
		printf("open file error!!\n");
		return -1;
	}

	fseek(fp, VENDORNAME_ADDR_BOOTLOADER, SEEK_SET);//vendor name
	fread(buf,VENDORNAMELEN,1, fp);
	printf("Vendor Name		: %s\n", buf);

	memset(buf,0,sizeof(buf));
	fseek(fp, PRODUCTNAME_ADDR_BOOTLOADER, SEEK_SET);//product name 
	fread(buf,PRODUCTNAMELEN, 1, fp);
	printf("Product Name		: %s\n", buf);

	memset(buf,0,sizeof(buf));
	fseek(fp, ETHER_ADDR_BOOTLOADER, SEEK_SET);//mac addr
	fread(buf, ETHERLEN, 1,fp);
	printf("Mac Address		: ");
	for(i=0; i<6; i++){
		printf("%02x",(uint8)buf[i]);
	}
	printf("\n");

	memset(buf,0,sizeof(buf));
	fseek(fp, SNMP_SYSOBJ_BOOTLOADER, SEEK_SET);//snmpObjID
	fread(buf, SNMPSYSOBJLEN,1, fp);
	printf("SNMP MIB level & OID	: ");
	for(i=0;i<21;i++){
		printf("%02x",(uint8)buf[i]);
	}
	printf("\n");

	memset(buf,0,sizeof(buf));
	fseek(fp, COUNTYR_CODE_BOOTLOADER, SEEK_SET);//country id
	fread(buf, COUNTRYCODELEN, 1,fp);
	printf("Default Country Code	: %x\n",(uint8)buf[0]);

	fclose(fp);
	return 0;
}

static int doSysAtsh(int argc, char **argv)
{
	char fwVersion[MAX_FWVER_LEN] ={0};
	int ret = 0;

	if((ret = tcapi_get(DEVICEINFO, FWVER, fwVersion)) != 0){
		return -1;
	}
	printf("Fw version		: %s",fwVersion);
	printf("bootbase version	: %s\n",BOOLOADER_VERSION);
	doRomCheck(1,NULL);
	doCheckImage();
	doCheckBootbase();
	
	return 0;
}
#endif/*TCSUPPORT_COMPILE*/
int main(int argc, char **argv) 
{
	FILE *proc_file;
	char cmd[160];
	const char *s;
	const char *applet_name;
	int i;
	char str[160];
	int func_num=0;

	if(argc == 2){
		if(!strcmp(argv[1],"version")){
//--			printf("\r\n tcci version: %s\n",MODULE_VERSION_TCCI);
			return 0;
		}
	}
	
	applet_name = argv[0];

	for (s = applet_name; *s != '\0';) {
		if (*s++ == '/')
			applet_name = s;
	}

	strcpy(cmd, applet_name);
	for (i = 1; i < argc; i++) {
		strcat(cmd, " ");
		strcat(cmd, argv[i]);
	}

    proc_file = fopen("/proc/kprofile/tcci_cmd", "w");
	if (!proc_file) {
		printf("open /proc/kprofile/tcci_cmd fail\n");
		return 0;
	}

	fprintf(proc_file, "%s", cmd);
	fclose(proc_file);

    proc_file = fopen("/proc/kprofile/tcci_cmd", "r");
	if (!proc_file) {
		printf("open /proc/kprofile/tcci_cmd fail\n");
		return 0;
	}
	fgets(str, 160, proc_file);
	func_num = atoi(str);
//	printf("data=%d\n", func_num);
	fclose(proc_file);
	
	if(func_num == 0 )
	   return 0;
	   
	if( ci_func[func_num] != NULL )	
        ci_func[func_num](argc-1, &argv[1]);
	return 0;
}
