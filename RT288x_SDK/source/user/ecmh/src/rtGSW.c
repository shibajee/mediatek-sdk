#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>

#include "linux/autoconf.h"
#include "ecmh.h"

#if 1
/*new switch address define*/
#include "ra_ioctl.h"
#endif


#define END_OF_MAC_TABLE			0xFFFFFFFF

/* ioctl commands */
#define RAETH_ESW_REG_READ			0x89F1
#define RAETH_ESW_REG_WRITE			0x89F2

/* rt3052 embedded ethernet switch registers */
#define REG_ESW_VLAN_ID_BASE		0x50
#define REG_ESW_VLAN_MEMB_BASE		0x70
#define REG_ESW_TABLE_SEARCH		0x24
#define REG_ESW_TABLE_STATUS0		0x28
#define REG_ESW_TABLE_STATUS1		0x2C
#define REG_ESW_TABLE_STATUS2		0x30
#define REG_ESW_WT_MAC_AD0			0x34
#define REG_ESW_WT_MAC_AD1			0x38
#define REG_ESW_WT_MAC_AD2			0x3C
#define REG_ESW_MAX					0xFC


#define DD printf("%s %d\n", __FUNCTION__, __LINE__);

/* global variables */
static struct mac_table internal_mac_table[1024];
static int	esw_fd = -1;
static esw_reg                  reg;
static struct ifreq             ifr;

void dump_mactable(void)
{
	int i=0;
	unsigned int mac1;
	char show_buf[128];
	mac1 = internal_mac_table[i].mac1;
	dolog(LOG_DEBUG, "Begin dump Mac table\n");
	while( i< 1024 && mac1 != END_OF_MAC_TABLE){
		sprintf(show_buf, "%08x%04x, %d, %08x\n", internal_mac_table[i].mac1, internal_mac_table[i].mac2, internal_mac_table[i].vidx, internal_mac_table[i].port_map);
		dolog(LOG_DEBUG, "%s\n", show_buf);
		i++;
		mac1 = internal_mac_table[i].mac1;
	}
}

/* ========================== Switch Register Read/Write =========================== */
static inline int reg_read(int offset, unsigned int *value)
{
	reg.off = offset;
	if (-1 == ioctl(esw_fd, RAETH_ESW_REG_READ, &ifr)) {
		perror("ioctl");
		close(esw_fd);
		exit(0);
	}
	*value = reg.val;
	return 0;
}

static inline int reg_write(int offset, int value)
{
    reg.off = offset;
    reg.val = value;
    if (-1 == ioctl(esw_fd, RAETH_ESW_REG_WRITE, &ifr)) {
        perror("ioctl");
        close(esw_fd);
        exit(0);
    }
    return 0;
}



static inline wait_switch_done(void)
{
	int i, value;

	for (i = 0; i < 20; i++) {
	    reg_read(REG_ESW_WT_MAC_ATC, &value);
	    if ((value & 0x8000) == 0 ){ //mac address busy
		printf("mac table IO done.\n");
		break;
	    }
	    usleep(1000);
	}
	if (i == 20)
		dolog(LOG_WARNING, 0, "*** rtGSW: timeout.");



}


/* ========================== General Register Read/Write =========================== */
#define PAGE_SIZE		0x1000 	/* 4096 */
#define READMODE	0x0
#define WRITEMODE	0x1
#define WRITE_DELAY	100			/* ms */
static unsigned int rareg(int mode, unsigned int addr, long long int new_value)
{
	int fd; 
	unsigned int round_tmp;
	void *start;
	volatile unsigned int *v_addr;
	unsigned int rc;

	fd = open("/dev/mem", O_RDWR | O_SYNC );
	if ( fd < 0 ) { 
		printf("open file /dev/mem error. %s\n", strerror(errno)); 
		exit(-1);
	} 

	// round addr to PAGE_SIZE
	round_tmp = addr;								// keep old value
	addr = (addr / PAGE_SIZE) * PAGE_SIZE;
	round_tmp = round_tmp - addr;

	start = mmap(0, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, addr);
	if(	(int)start == -1 ){
		printf("mmap() failed at phsical address:%d %s\n", addr, strerror(errno)); 
		close(fd);
		exit(-1);
	}
	//printf("mmap() starts at 0x%08x successfuly\n", (unsigned int) start);

	v_addr = (void *)start + round_tmp;
	addr = addr + round_tmp;
	printf("0x%08x: 0x%08x\n", addr, *v_addr);

	if(mode == WRITEMODE){
		*v_addr = new_value;
		usleep(WRITE_DELAY * 1000);
		printf("0x%08x: 0x%08x\n", addr, *v_addr);
	}

	rc = *v_addr;
	munmap(start, PAGE_SIZE);
	close(fd);
	return rc;
}


/*
 *	The cpu overhead of this function is low.
 */
void sync_internal_mac_table(void *argu)
{
	unsigned int value, /*mac1,*/ mac2, i = 0;
	unsigned int value1 = 0;
	dolog(LOG_DEBUG, "*** Ralink SYNC mac tables ***\n");

	//timer_setTimer(INTERNAL_SYNC_TIMEOUT, sync_internal_mac_table, NULL);
#if 0	
	
	reg_write(REG_ESW_TABLE_SEARCH, 0x1);
	while( i < 0x3fe) {
		reg_read(REG_ESW_TABLE_STATUS0, &value);
		if (value & 0x1) { //search_rdy
			if ((value & 0x70) == 0) {
				dolog(LOG_DEBUG, "*** RT Switch: found an unused entry (age = 3'b000), please check!\n");
				reg_write(REG_ESW_TABLE_SEARCH, 0x2); //search for next address
				continue;
			}

			internal_mac_table[i].vidx = (value >> 7) & 0xf;

			// read mac1
			reg_read(REG_ESW_TABLE_STATUS2, &(internal_mac_table[i].mac1));

			// read mac2
			reg_read(REG_ESW_TABLE_STATUS1, &mac2);
			internal_mac_table[i].mac2 = (mac2 & 0xffff);

			internal_mac_table[i].port_map = (value & 0x0007f000) >> 12 ;

			if (value & 0x2) {
				log(LOG_WARNING, 0, "*** rtGSW: end of table. %d", i);
				internal_mac_table[i+1].mac1 = END_OF_MAC_TABLE;
				return;
			}
			reg_write(REG_ESW_TABLE_SEARCH, 0x2); //search for next address
			i++;
		}else if (value & 0x2) { //at_table_end
			//log(LOG_WARNING, 0, "*** rtGSW: found the last entry (not ready). %d", i);
			internal_mac_table[i].mac1 = END_OF_MAC_TABLE;
			return;
		}else
			usleep(2000);
	}
#else
	printf("sync table\n\r");
	reg_write(REG_ESW_WT_MAC_ATC, 0x8004);
	while( i < 0x3fe) {
	//while( i < 0x3fe) {
		reg_read(REG_ESW_WT_MAC_ATC, &value);
		if (value & (0x1 << 13)) { //search_rdy
			reg_read(REG_ESW_TABLE_ATRD, &value1);
			if ((value1 & 0xff000000) == 0) {
				dolog(LOG_WARNING, 0, "*** rtGSW: found an unused entry (age = 3'b000), please check!");
				reg_write(REG_ESW_TABLE_SEARCH, 0x2); //search for next address
				continue;
			}

			internal_mac_table[i].vidx = (value >> 7) & 0xf;
			// read mac1
			reg_read(REG_ESW_TABLE_TSRA1, &(internal_mac_table[i].mac1));

			// read mac2
			reg_read(REG_ESW_TABLE_TSRA2, &mac2);
			internal_mac_table[i].mac2 = (mac2 >> 16);

			//reg_read(REG_ESW_TABLE_ATRD, &value1);
			internal_mac_table[i].port_map = (value1 & 0x0007f0) >> 4 ;

			if (value & 0x4000) {
				dolog(LOG_WARNING, 0, "*** rtGSW: end of table. %d", i);
				printf("sync table at_table_end 1\n\r");
				internal_mac_table[i+1].mac1 = END_OF_MAC_TABLE;
				return;
			}
			reg_write(REG_ESW_WT_MAC_ATC, 0x8005); //search for next address
			i++;
		}else if (value & 0x4000) { //at_table_end
			//log(LOG_WARNING, 0, "*** rtGSW: found the last entry (not ready). %d", i);
			printf("sync table at_table_end\n\r");
			internal_mac_table[i].mac1 = END_OF_MAC_TABLE;
			return;
		}else
			//usleep(2000);
			usleep(1000);
	}

#endif
	printf("sync table OK\n\r");
	internal_mac_table[i].mac1 = END_OF_MAC_TABLE;
	return;
}

int portLookUpByMac(unsigned char *mac);
int portLookUpByMac(unsigned char *mac)
{
	unsigned int  mac1, mac2, i = 0, mac_iter;

	mac1 = ntohl(*(unsigned int *)mac);
	mac2 = ntohl(*((unsigned int *)mac+1)) >> 16;

	mac_iter = internal_mac_table[i].mac1;
	while(i < 0x3fe && mac_iter != END_OF_MAC_TABLE) {
		if(internal_mac_table[i].vidx != LAN_VLAN_IDX)
			goto next_entry;

		/* dolog(LOG_DEBUG, "look for [%08x %04x] %08x %04x\n", mac1, mac2 ,internal_mac_table[i].mac1, internal_mac_table[i].mac2); */
		if(	internal_mac_table[i].mac1 == mac1 &&
			internal_mac_table[i].mac2 == mac2){
			switch( internal_mac_table[i].port_map ){
			case 0x1:
				return 0;
			case 0x2:
				return 1;
			case 0x4:
				return 2;
			case 0x8:
				return 3;
			case 0x10:
				return 4;
			case 0x20:
				return 5;
			case 0x40:	/* CPU Only */
				break;
			default:
				dolog(LOG_DEBUG, "No/Multi ports found:%x\n", internal_mac_table[i].port_map);
				return -1;
			}
		}
next_entry:
		i++;
		mac_iter = internal_mac_table[i].mac1;
	}

#ifdef SEARCH_CLIENT_IN_WIFI_MACTABLE
	if(WiFiSTALookUPByMac(mac1, mac2)){
		return OTHER_INTERFACE;
	}else
#endif
		return -1;
}
static unsigned char getMacTablePortNum(struct in6_addr *mca)
{
	unsigned int  mac1 = 0, mac2 = 0, i = 0, mac_iter;

	mac1 = 0x33330000 | IN6_GET_ADDR_SINGLE_BYTE(mca, 12) << 8 | IN6_GET_ADDR_SINGLE_BYTE(mca, 13);
	mac2 = IN6_GET_ADDR_SINGLE_BYTE(mca, 14) << 8 | IN6_GET_ADDR_SINGLE_BYTE(mca, 15);

	mac_iter = internal_mac_table[i].mac1;
	while(i < 0x3fe && mac_iter != END_OF_MAC_TABLE) {
		if(internal_mac_table[i].vidx != LAN_VLAN_IDX)
			goto next_entry;

		/* dolog(LOG_DEBUG, "look for [%08x %04x] %08x %04x\n", mac1, mac2 ,internal_mac_table[i].mac1, internal_mac_table[i].mac2); */
		if(	internal_mac_table[i].mac1 == mac1 &&
			internal_mac_table[i].mac2 == mac2){
			return internal_mac_table[i].port_map;
		}
next_entry:
		i++;
		mac_iter = internal_mac_table[i].mac1;
	}

#ifdef SEARCH_CLIENT_IN_WIFI_MACTABLE
	if(WiFiSTALookUPByGroup(mac1, mac2)){
		return OTHER_INTERFACE;
	}else
#endif
		return -1;

}

/*
 * don't export this func..
 */
static void updateMacTable(struct in6_addr *mca, unsigned char port_map, int delay_delete)
{
	int i, value;
	char wholestr[13];
	char tmpstr[9];
	unsigned int value1 = 0;
	sprintf(wholestr, "%s%02x%02x%02x%02x", "3333", IN6_GET_ADDR_SINGLE_BYTE(mca, 12), IN6_GET_ADDR_SINGLE_BYTE(mca, 13), IN6_GET_ADDR_SINGLE_BYTE(mca, 14), IN6_GET_ADDR_SINGLE_BYTE(mca, 15));

#if 0
	strncpy(tmpstr, wholestr, 8);
	tmpstr[8] = '\0';

	value = strtoul(tmpstr, NULL, 16);
	reg_write(REG_ESW_WT_MAC_AD2, value);
	strncpy(tmpstr, &wholestr[8], 4);
	tmpstr[4] = '\0';
	value = strtoul(tmpstr, NULL, 16);
	reg_write(REG_ESW_WT_MAC_AD1, value);

#else
        dolog(LOG_DEBUG,"updateMacTable!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	strncpy(tmpstr, wholestr, 8);
	tmpstr[8] = '\0';

	value = strtoul(tmpstr, NULL, 16);
	
	reg_write(REG_ESW_WT_MAC_ATA1, value);
        dolog(LOG_DEBUG,"REG_ESW_WT_MAC_ATA1 is 0x%x\n", value);
	//printf("REG_ESW_WT_MAC_ATA1 is 0x%x\n\r",value);


	strncpy(tmpstr, &wholestr[8], 4);
	tmpstr[4] = '\0';

	value = strtoul(tmpstr, NULL, 16);
	value = (value << 16);
	value |= (1 << 15);//IVL=1

	value |= (1 << 0); //LAN ID ==1
	
	reg_write(REG_ESW_WT_MAC_ATA2, value);
        dolog(LOG_DEBUG,"REG_ESW_WT_MAC_ATA2 is 0x%x\n", value);
	//printf("REG_ESW_WT_MAC_ATA2 is 0x%x\n\r",value);
	
	value1 = value; //save for later usage        

#endif

	value = 0;
	if(port_map){
		/*
		 * force all mulicast addresses to bind with CPU.
		 */
		value |= (0x1 << 10);//port 6 cpu port
		/*
		 * fill the port map
		 */
		//printf("port_map is 0x%x\n\r", port_map);
		dolog(LOG_DEBUG,"port_map = 0x%x\n", port_map);
		value |= (port_map & (0x7f)) << 4;

                value |= (0xff << 24); //w_age_field
		value |= (0x3<< 2); //static



		reg_write(REG_ESW_WT_MAC_ATWD, value);
		value = 0x8001;  //w_mac_cmd
	        reg_write(REG_ESW_WT_MAC_ATC, value);


		wait_switch_done();

		/*
		 * new an additional entry for IGMP Inquery/Report on WAN.
		 */
		if(WANPORT){
		        value = value1;
			value = (value & 0xffffff00);
			value |= (2 << 0); //WAN ID ==2
	
			reg_write(REG_ESW_WT_MAC_ATA2, value);
			printf("WAN REG_ESW_WT_MAC_ATA2 is 0x%x\n\r",value);

			value1 = (WANPORT << 4);
			value1 |= (0x1 << 10);//port 6 cpu port

			value1 |= (0xff << 24); //w_age_field
			value1 |= (0x3<< 2); //static

			reg_write(REG_ESW_WT_MAC_ATWD, value1);

			value1 = 0x8001;  //w_mac_cmd
			reg_write(REG_ESW_WT_MAC_ATC, value1);

			wait_switch_done();
			printf("for wan port is done\n\r");
		}
	}else{
		if(delay_delete == MACTABLE_ENTRY_ZEROED){
			/*
			 * Can't delete this entry too early.
			 *
			 * Because multicast packets from WAN may still come even receiver on LAN has left, and
			 * at the same time the kernel routing rule is not yet deleted by igmpproxy.
			 *
			 * If we delete mac entry earier than deleting routing rule (by igmpproxy),
			 * these packets would be forwarded to "br0" and then flood on eth2.1(vlan1) due to our 
			 * default policy -- "Broadcast if not found". So we may see flooding packets on
			 * LAN until the kernel routing rule is deleted.
			 *
			 * So we keep the mac entry alive to avoid the our default policy until the igmp group
			 * is actually eliminated.
			 */

			/*
			 * zero the entry
			 */
#if 0
		    value |= (7 << 4);		//w_age_field, keep it alive
		    value |= 1;				//w_mac_cmd
		    reg_write(REG_ESW_WT_MAC_AD0, value);
		    wait_switch_done();
#else

		    value = (0xff << 24); //w_age_field
		    value |= (0x3<< 2); //static
		    reg_write(REG_ESW_WT_MAC_ATWD, value);
		    //printf("delay delete = zer0 REG_ESW_WT_MAC_ATWD is 0x%x\n\r",value);
		    dolog(LOG_DEBUG,"delay delete = zer0 REG_ESW_WT_MAC_ATWD is 0x%x\n", value);
		  
		    value = 0x8001;  //w_mac_cmd
		    reg_write(REG_ESW_WT_MAC_ATC, value);
		    wait_switch_done();

#endif

		}else if (delay_delete == MACTABLE_ENTRY_DELETED){
			/*
			 * delete the entry
			 */
#if 0
			value |= 1;				//w_mac_cmd
			reg_write(REG_ESW_WT_MAC_AD0, value);
			wait_switch_done();

			/*
			 * delete the additional entry on WAN.
			 */
			value = 0;
			value |= (1 << 7);		//w_index
			value |= 1;				//w_mac_cmd
			reg_write(REG_ESW_WT_MAC_AD0, value);
			wait_switch_done();
#else
			value = 0; //STATUS=0, delete mac
			reg_write(REG_ESW_WT_MAC_ATWD, value);
			value = 0x8001;  //w_mac_cmd
			reg_write(REG_ESW_WT_MAC_ATC, value);
			wait_switch_done();



#endif
		}else{
			dolog(LOG_DEBUG, "!!!! Fatal warning, empty mac entry with dontcare flag.");	
		}

	}
	sync_internal_mac_table(NULL);
}

void update_group_port_map(struct in6_addr *mca, int delete_flag)
{
	struct intnode          *intn;
	struct groupnode        *groupn;
	struct listnode         *ln;
	struct grpintnode       *grpintn;
	struct listnode         *gn;
	struct subscrnode       *subscrn;
	struct listnode         *ssn;

	unsigned char new_portmap = 0x0;
	unsigned int grpnum = GetGroup(mca);

	dolog(LOG_DEBUG, "!!!! update_group_port_mapg.\n!!!!!!!!!!!!!!");	
	
	LIST_LOOP(g_conf->groups, groupn, ln)
	{
		if( grpnum != GetGroup(&groupn->mca))
			continue;
		LIST_LOOP(groupn->interfaces, grpintn, gn)
		{
			intn = int_find(grpintn->ifindex);
			if (!intn)
				continue;

			if(intn->upstream)
				continue;
			LIST_LOOP(grpintn->subscriptions, subscrn, ssn)
			{
				if(subscrn->portnum == -1){
					// can't find which port it's in, so opens all ports for it.
					dolog(LOG_DEBUG, "!!!!! subscrn->portnum==-1\n" );
					new_portmap =  (0x5f & ~(WANPORT)); // All Lan ports
				}else{
					new_portmap = new_portmap | (0x1 << subscrn->portnum);
				}
/*
				dolog(LOG_DEBUG, "subscrn->portmap:%08x\n", subscrn->portmap);
				new_portmap |= subscrn->portmap;
*/
			}
		}
	}

	if(delete_flag == MACTABLE_ENTRY_DELETED && new_portmap == 0x0){
		updateMacTable(mca, 0x0, MACTABLE_ENTRY_DELETED);
	}else if(new_portmap != getMacTablePortNum(mca)){
		updateMacTable(mca, new_portmap, delete_flag);
	}
}

int ralink_timeout(void)
{
	sync_internal_mac_table(NULL);
	return 0;
}

static void create_all_hosts_rule(void)
{
	struct in6_addr mca;
	memset(&mca, 0, sizeof(mca));
	/*
	IN6_SET_ADDR_SINGLE_BYTE(&mca, 12, 0);
	IN6_SET_ADDR_SINGLE_BYTE(&mca, 13, 0);
	IN6_SET_ADDR_SINGLE_BYTE(&mca, 14, 0);
	*/
	IN6_SET_ADDR_SINGLE_BYTE(&mca, 15, 1);

	updateMacTable(&mca, 0x5f & ~(WANPORT), MACTABLE_ENTRY_DONT_CARE);
}

static void destory_all_hosts_rule()
{
	struct in6_addr mca;
	memset(&mca, 0, sizeof(mca));
	/*
	IN6_SET_ADDR_SINGLE_BYTE(&mca, 12, 0);
	IN6_SET_ADDR_SINGLE_BYTE(&mca, 13, 0);
	IN6_SET_ADDR_SINGLE_BYTE(&mca, 14, 0);
	*/
	IN6_SET_ADDR_SINGLE_BYTE(&mca, 15, 1);

	updateMacTable(&mca, 0x0, MACTABLE_ENTRY_DELETED);
}


static void remove_all_groups(void)
{
	struct groupnode        *groupn;
	struct listnode         *ln;

	LIST_LOOP(g_conf->groups, groupn, ln){
		updateMacTable(&groupn->mca, 0x0, MACTABLE_ENTRY_DELETED);
	}
}

void ralink_fini(void)
{
	unsigned int value;

	if(!g_conf->es)
		return;

	remove_all_groups();

	destory_all_hosts_rule(); 
	if(esw_fd >= 0)
		close(esw_fd);

	/* MLD packets to flooding */
	value = rareg(READMODE, 0x10110098, 0);
	value = value & 0xFDFFFFFF;
	rareg(WRITEMODE, 0x10110098, value);
}

int ralink_init(void)
{
	/*
	 *  handle registers
	 */
	unsigned int value, value2;

	if(!g_conf->es)
		return 0;
        /* New GSW support snooping"
        /* MLD packets to cpu only */
        value = rareg(READMODE, RALINK_ETH_SW_BASE + 0x1c, 0);
        value = value & 0x8FFFFFFF;
        value = value | 0x60000000;
        rareg(WRITEMODE, RALINK_ETH_SW_BASE + 0x1c, value);

	esw_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (esw_fd < 0) {
		perror("socket");
		ralink_fini();
		return -1;
	}

	strncpy(ifr.ifr_name, "eth2", 5);
	ifr.ifr_data = (char *)&reg;

	create_all_hosts_rule();

	sync_internal_mac_table(NULL);

	return 0;
}
