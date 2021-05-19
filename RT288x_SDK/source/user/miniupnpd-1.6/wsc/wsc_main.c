/***************************************
	wsc main function
****************************************/
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h> 
#include <sys/ioctl.h> 
	   
//#include "config.h"
#include "upnpglobalvars.h"
#include "wsc_common.h"
#include "wsc_msg.h"
#include "wsc_upnp.h"

#include "upnpreplyparse.h"

int wsc_debug_level = RT_DBG_OFF;

int ioctl_sock = -1;
unsigned char UUID[16]= {0};

int enableDaemon = 0;
	
static int wscUPnPDevInit = FALSE;
static int wscK2UMInit = FALSE;
static int wscU2KMInit = FALSE;
static int wscMsgQInit = FALSE;


int WscUPnPOpMode;
//peter : 0523
//char HostDescURL[WSC_UPNP_DESC_URL_LEN] = {0};			// Used to save the DescriptionURL of local host.
unsigned char HostMacAddr[MAC_ADDR_LEN]={0};			// Used to save the MAC address of local host.
unsigned int HostIPAddr;								// Used to save the binded IP address of local host.
char WSC_IOCTL_IF[IFNAMSIZ];
unsigned short WPS_PORT;
#if 1 //def MULTIPLE_CARD_SUPPORT
#define FILE_PATH_LEN	256
char pid_file_path[FILE_PATH_LEN];
#endif // MULTIPLE_CARD_SUPPORT //

void usage(void)
{
	printf("Usage: miniupnpd -G [-I infName] [-A ipaddress] [-n port] [-F descDoc] [-w webRootDir] -m UPnPOpMode -D [-l debugLevel] -h\n");
	printf("\t-G:      Enable IGD Device service in miniupnpd\n");
	printf("\t-I:	   Interface name this daemon will run wsc protocol(if not set, will use the default interface name - ra0)\n");
	printf("\t\t\te.g.: ra0\n");
	printf("\t-A:       IP address of the device (if not set, will auto assigned)\n");
	printf("\t\t\t e.g.: 192.168.0.1\n");
	printf("\t-n:       Port number for receiving UPnP messages (if not set, will auto assigned)\n");
	printf("\t\t\t e.g.: 54312\n");
	printf("\t-F:       Name of device description document (If not set, will used default value)\n");
	printf("\t\t\t e.g.: WFADeviceDesc.xml\n");
	printf("\t-w:       Filesystem path where descDoc and web files related to the device are stored\n");
	printf("\t\t\t e.g.: /etc/xml/\n");
	printf("\t-m:       UPnP system operation mode\n");
	printf("\t\t\t 1: Enable UPnP Device service(Support Enrolle or Proxy functions)\n");
	printf("\t\t\t 2: Enable UPnP Control Point service(Support Registratr function)\n");
	printf("\t\t\t 3: Enable both UPnP device service and Control Point services.\n");
	printf("\t-D:       Run program in daemon mode.\n");
	printf("\t-l:       Debug level of WPS module.\n");
	printf("\t\t\t 0: debug printing off\n");
	printf("\t\t\t 1: DEBUG_ERROR\n");
	printf("\t\t\t 2: DEBUG_PKT\n");
	printf("\t\t\t 3: DEBUG_INFO\n");
	
	exit(1);

}


void sig_handler(int sigNum)
{
	sig_atomic_t status;

	DBGPRINTF(RT_DBG_INFO, "Get a signal=%d!\n", sigNum);
	switch(sigNum)
	{
		case SIGCHLD:
			wait3(&status, WNOHANG, NULL);
			break;
		default:
			break;
	}

}

int wscSystemInit(void)
{
	int retVal;
	
	struct sigaction sig;
	memset(&sig, 0, sizeof(struct sigaction));
	sig.sa_handler= &sig_handler;
	sigaction(SIGCHLD, &sig, NULL);
	
	retVal = wscMsgSubSystemInit();
	
	return retVal;
}


int wscSystemStop(void)
{
	/* Stop the K2U and U2K msg subsystem */
	if (wscU2KMInit && ioctl_sock >= 0) 
	{
		// Close the ioctl socket.
		close(ioctl_sock);
		ioctl_sock = -1;
	}
	
	if (wscK2UMInit)
	{

	}

	if(wscUPnPDevInit)
	{
		WscUPnPDevStop();
	}
/*
	if(wscUPnPCPInit)
		WscUPnPCPStop();
*/
#if 0	
	if(wscUPnPSDKInit)
		UpnpFinish();
#endif

	if( wscMsgQInit)
	{
		wscMsgSubSystemStop();
	}
	return 1;
}

/******************************************************************************
 * WscDeviceCommandLoop
 *
 * Description: 
 *       Function that receives commands from the user at the command prompt
 *       during the lifetime of the device, and calls the appropriate
 *       functions for those commands. Only one command, exit, is currently
 *       defined.
 *
 * Parameters:
 *    None
 *
 *****************************************************************************/
void *
WscDeviceCommandLoop( void *args )
{
    int stoploop = 0;
    char cmdline[100];
    char cmd[100];

    while( !stoploop ) {
        sprintf( cmdline, " " );
        sprintf( cmd, " " );

		printf( "\n>> " );

        // Get a command line
        fgets( cmdline, 100, stdin );

        sscanf( cmdline, "%s", cmd );

        if( strcasecmp( cmd, "exit" ) == 0 ) {
			printf( "Shutting down...\n" );
			wscSystemStop();
        } else {
			printf("\n   Unknown command: %s\n\n", cmd);
			printf("   Valid Commands:\n" );
			printf("     Exit\n\n" );
        }

    }

    return NULL;
}



/******************************************************************************
 * main of WPS module
 *
 * Description: 
 *       Main entry point for WSC device application.
 *       Initializes and registers with the sdk.
 *       Initializes the state stables of the service.
 *       Starts the command loop.
 *
 * Parameters:
 *    int argc  - count of arguments
 *    char ** argv -arguments. The application 
 *                  accepts the following optional arguments:
 *
 *          -ip 	ipAddress
 *          -port 	port
 *		    -desc 	descDoc
 *	        -webdir webRootDir"
 *		    -help 
 *          -i      ioctl binding interface.
 *
 *****************************************************************************/
int WPSInit(int argc, char **argv)
{
	unsigned int portTemp = 0;
	char *ipAddr = NULL, *descDoc = NULL, *webRootDir = NULL, *infName = NULL;
//	unsigned short port = 0;
	FILE *fp;
	int retVal;
	int opt;

	if (argc < 2)
		usage();

	/* default op mode is device */
	WscUPnPOpMode = WSC_UPNP_OPMODE_DEV;
	/* default op mode is device */
	memset(&WSC_IOCTL_IF[0], 0, IFNAMSIZ);
	strcpy(&WSC_IOCTL_IF[0], "ra0");
	
	/* first, parsing the input options */
	while((opt = getopt(argc, argv, "A:I:n:F:w:m:l:DGdh"))!= -1)
	{
		switch (opt)
		{
			case 'A':
				ipAddr = optarg;
				break;
			case 'n':
				sscanf(optarg, "%u", &portTemp);
				break;
			case 'F':
				descDoc = optarg;
				break;
			case 'I':
				infName = optarg;
				memset(&WSC_IOCTL_IF[0], 0, IFNAMSIZ);
				if (strlen(infName))
					strncpy(&WSC_IOCTL_IF[0], infName, IFNAMSIZ);
				else
					strcpy(&WSC_IOCTL_IF[0], "ra0");
				break;
			case 'w':
				webRootDir = optarg;
				break;
			case 'm':
				WscUPnPOpMode = strtol(optarg, NULL, 10);
				if (WscUPnPOpMode < WSC_UPNP_OPMODE_DEV || WscUPnPOpMode > WSC_UPNP_OPMODE_BOTH)
					usage();
				break;
			case 'D':
				enableDaemon = 1;
				break;
			case 'l':
				wsc_debug_level = strtol(optarg, NULL, 10);
				break;
			case 'G':
				/* dummy option - In fact : be handled in miniupnpd */
				break;
			case 'd':
				/* dummy option - In fact : be handled in miniupnpd */
				break;	
			case 'h':
				usage();
				break;
        }
    }

	if ((WscUPnPOpMode < WSC_UPNP_OPMODE_DEV) || (WscUPnPOpMode > WSC_UPNP_OPMODE_BOTH))
	{
		fprintf(stderr, "Wrong UPnP Operation Mode: %d\n", WscUPnPOpMode);
		usage();
	}
	if ((wsc_debug_level > RT_DBG_ALL)  || (wsc_debug_level < RT_DBG_OFF))
	{
		fprintf(stderr, "Wrong Debug Level: %d\n", wsc_debug_level);
		usage();
	}				

	if (enableDaemon)
	{
		pid_t childPid;

		childPid = fork();
		if(childPid < 0)
		{
			fprintf(stderr, "Run in daemon mode failed --ErrMsg=%s!\n", strerror(errno));
			exit(0);
		} 
		else if (childPid > 0)
			exit(0);
		else 
		{
			/* normal programming style to make a program work as a daemon */
			close(0); /* close the stdin fd */
			close(1); /* close the stdout fd */
		}	
	}

	/* Write the pid file */
#if 1 // def MULTIPLE_CARD_SUPPORT
	memset(&pid_file_path[0], 0, FILE_PATH_LEN);
	sprintf(pid_file_path, "%s.%s", DEFAULT_PID_FILE_PATH, WSC_IOCTL_IF);
	DBGPRINTF(RT_DBG_INFO, "The pid file is: %s!\n", pid_file_path);
	if ((fp = fopen(pid_file_path, "w")) != NULL)
#else
	if((fp = fopen(DEFAULT_PID_FILE_PATH, "w"))!= NULL)
#endif // MULTIPLE_CARD_SUPPORT //
	{
		fprintf(fp, "%d", getpid());
		fclose(fp);
	}
	
	/* System paramters initialization */
	if (wscSystemInit() == WSC_SYS_ERROR)
	{
		fprintf(stderr, "wsc MsgQ System Initialization failed!\n");
		goto STOP;
	}
	wscMsgQInit = TRUE;
	
	/* Initialize the netlink interface from kernel space */
	if(wscK2UModuleInit() != WSC_SYS_SUCCESS)
	{	
		fprintf(stderr, "creat netlink socket failed!\n");
		goto STOP;
	}
	else 
	{
		DBGPRINTF(RT_DBG_INFO, "Create netlink socket success!\n");
		wscK2UMInit = TRUE;
	}
	
	/* Initialize the ioctl interface for data path to kernel space */
	ioctl_sock = wscU2KModuleInit();
	if(ioctl_sock == -1)
	{
		fprintf(stderr, "creat ioctl socket failed!err=%d!\n", errno);
		goto STOP;
	}
	else
	{
		DBGPRINTF(RT_DBG_INFO, "Create ioctl socket(%d) success!\n", ioctl_sock);
		wscU2KMInit = TRUE;
	}

	/* Initialize the upnp related data structure and start upnp service */
	if(WscUPnPOpMode)
	{
		struct ifreq  ifr;
#if 0			
		// Initializing UPnP SDK
		if ((retVal = UpnpInit(ipAddr, port)) != 0)
		{
			DBGPRINTF(RT_DBG_ERROR, "Error with UpnpInit -- %d\n", retVal);
			UpnpFinish();
			goto STOP;
		}
		wscUPnPSDKInit = TRUE;
#endif
		// Get the IP/Port the UPnP services want to bind.

		if (ipAddr == NULL)
		{
//			ipAddr = UpnpGetServerIpAddress();
//			ipAddr = "192.168.15.43";
#if 1
#if 1 //Porting for miniupnpd 1.6
			struct lan_addr_s * tmp_addr;
			tmp_addr = lan_addrs.lh_first;
			if (tmp_addr!=NULL)
			{
				ipAddr =tmp_addr->str;
			}
#else
			ipAddr = lan_addr[0].str;
#endif
#else
			if ((lan_addr[n_lan_addr-1].str != NULL) && (GETFLAG(ENABLEWPSMASK)))
				ipAddr = lan_addr[n_lan_addr-1].str;
#endif
		}
		ASSERT(ipAddr != NULL);
		inet_aton(ipAddr, (struct in_addr *)&HostIPAddr);
		
		if (portTemp != 0)
		{
		    WPS_PORT = (unsigned short)portTemp;
		}
		else
		{
			/* set default port number for WPS service */
			WPS_PORT = (unsigned short)DEFAULT_WPS_PORT;
		}

		ASSERT(WPS_PORT > 0);

		// Get the Mac Address of wireless interface
		memset(&ifr, 0, sizeof(struct ifreq));
		strcpy(ifr.ifr_name, WSC_IOCTL_IF); 
		if (ioctl(ioctl_sock, SIOCGIFHWADDR, &ifr) > 0) 
        { 
			perror("ioctl to get Mac Address");
			goto STOP;
		}
		memcpy(HostMacAddr, ifr.ifr_hwaddr.sa_data, MAC_ADDR_LEN);

		DBGPRINTF(RT_DBG_INFO, "\t HW-Addr: %02x:%02x:%02x:%02x:%02x:%02x!\n", HostMacAddr[0], HostMacAddr[1], 
				HostMacAddr[2], HostMacAddr[3], HostMacAddr[4], HostMacAddr[5]);

		// Start UPnP Device Service.
		if (WscUPnPOpMode & WSC_UPNP_OPMODE_DEV)
		{	  
		    retVal = WscUPnPDevStart(ipAddr, WPS_PORT, descDoc, webRootDir);
			if (retVal != WSC_SYS_SUCCESS)
				goto STOP;
			wscUPnPDevInit = TRUE;
		}
#if 0 /* apply for control point only */
		// Start UPnP Control Point Service.
		if(WscUPnPOpMode & WSC_UPNP_OPMODE_CP)
		{
			retVal = WscUPnPCPStart(ipAddr, port);
			if (retVal != WSC_SYS_SUCCESS)
				goto STOP;
			wscUPnPCPInit = TRUE;
		}
#endif
	}

#if 0 /* handled in main() of miniupnpd */
    /* Catch Ctrl-C and properly shutdown */
    sigemptyset(&sigs_to_catch);
    sigaddset(&sigs_to_catch, SIGINT);
    sigwait(&sigs_to_catch, &sig);

    DBGPRINTF(RT_DBG_INFO, "Shutting down on signal %d...\n", sig);
#endif

	return 1;	

    DBGPRINTF(RT_DBG_INFO, "wscSystemStop()...\n");
    wscSystemStop();
STOP:
		/* Trigger other thread to stop the procedures. */
#if 1 //def MULTIPLE_CARD_SUPPORT
	unlink(pid_file_path);
#else
	unlink(DEFAULT_PID_FILE_PATH);
#endif // MULTIPLE_CARD_SUPPORT //
	return 0;
//    exit(0);
}

