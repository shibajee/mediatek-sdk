/*
 *  This implementation is for RT3052 Switch.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include "linux/autoconf.h"

#include "ralink.h"

#include "defs.h"

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

#ifdef CONFIG_RAETH_SPECIAL_TAG
#define LAN_VLAN_IDX              6
#define WAN_VLAN_IDX              7
#else
#define LAN_VLAN_IDX              0
#define WAN_VLAN_IDX              1
#endif

typedef struct rt3052_esw_reg {
	unsigned int off;
	unsigned int val;
} esw_reg;

struct mac_table{
	unsigned int		mac1;		// 4 byte mac address
	unsigned short int	mac2;		// 2 byte mac address
	unsigned char		vidx;
	unsigned char		port_map;
};

// function prototype
void updateMacTable(struct group *entry, int delay_deleted);
void updateMacTable_specialtag(struct group *entry, unsigned char new_portmap, int delay_delete);
static inline int reg_read(int offset, int *value);
static inline int reg_write(int offset, int value);
static void ZeroEntriesBarrier(struct group *entry, int mode);

// global variables.
static struct mac_table internal_mac_table[1024];
static int 			esw_fd = -1;

static struct ifreq		ifr;
static esw_reg			reg;

void dump_table(void)
{
	int i=0;
	unsigned int mac1;
	char show_buf[128];
	mac1 = internal_mac_table[i].mac1;
	while( i< 1024 && mac1 != END_OF_MAC_TABLE){
		sprintf(show_buf, "%08x%04x, %d, %08x\n", internal_mac_table[i].mac1, internal_mac_table[i].mac2, internal_mac_table[i].vidx, internal_mac_table[i].port_map);
		printf("%s\n", show_buf);
		i++;
		mac1 = internal_mac_table[i].mac1;
	}
}


/*
 *	The cpu overhead of this function is low.
 */
static void sync_internal_mac_table(void *argu)
{
	unsigned int value, mac1, mac2, i = 0;

	reg_write(REG_ESW_TABLE_SEARCH, 0x1);
	while( i < 0x3fe) {
		reg_read(REG_ESW_TABLE_STATUS0, &value);
		if (value & 0x1) { //search_rdy
			if ((value & 0x70) == 0) {
				log(LOG_WARNING, 0, "*** RT3052: found an unused entry (age = 3'b000), please check!");
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
				log(LOG_WARNING, 0, "*** RT3052: end of table. %d", i);
				internal_mac_table[i+1].mac1 = END_OF_MAC_TABLE;
				return;
			}
			reg_write(REG_ESW_TABLE_SEARCH, 0x2); //search for next address
			i++;
		}else if (value & 0x2) { //at_table_end
			//log(LOG_WARNING, 0, "*** RT3052: found the last entry (not ready). %d", i);
			internal_mac_table[i].mac1 = END_OF_MAC_TABLE;
			return;
		}else
			usleep(2000);
	}
	internal_mac_table[i].mac1 = END_OF_MAC_TABLE;
	return;
}

void rt_switch_fini(void)
{
	/*
	 *  handle RT3052 registers
	 */
	/* 1011009c */
	unsigned int value;

	/* 10110014 */
	value = rareg(READMODE, 0x10110014, 0);
	value = value & 0xFF7FFFFF;
	rareg(WRITEMODE, 0x10110014, value);

	if(esw_fd >= 0)
		close(esw_fd);
}

void rt_switch_init(int se)
{
	unsigned int value;

	/* 1011009c */
	value = rareg(READMODE, 0x1011009c, 0);
	//value = value | 0x08000000;
	value = value & 0xE7FFFFFF;
	rareg(WRITEMODE, 0x1011009c, value);

	/* 10110014 */
	value = rareg(READMODE, 0x10110014, 0);
	value = value | 0x00800000;
	rareg(WRITEMODE, 0x10110014, value);

	esw_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (esw_fd < 0) {
		perror("socket");
		exit(0);
	}

	strncpy(ifr.ifr_name, "eth2", 5);
	ifr.ifr_data = (char *)&reg;

	sync_internal_mac_table(NULL);
}

static inline int reg_read(int offset, int *value)
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
		reg_read(REG_ESW_WT_MAC_AD0, &value);
		if (value & 0x2) {	//w_mac_done
			//printf("done.\n");
			break;
		}
		usleep(1000);
	}
	if (i == 20)
		log(LOG_WARNING, 0, "*** RT3052: timeout.");
}

static void ZeroEntriesBarrier(struct group *entry, int mode)
{
	unsigned char lanport[] = LANPORT_RANGE;

	int i, value;
	char wholestr[13];
	char tmpstr[9];

	sprintf(wholestr, "%s%02x%02x%02x", "01005e", entry->a1, entry->a2, entry->a3);

	strncpy(tmpstr, wholestr, 8);
	tmpstr[8] = '\0';

	value = strtoul(tmpstr, NULL, 16);
	reg_write(REG_ESW_WT_MAC_AD2, value);
	strncpy(tmpstr, &wholestr[8], 4);
	tmpstr[4] = '\0';
	value = strtoul(tmpstr, NULL, 16);
	reg_write(REG_ESW_WT_MAC_AD1, value);

	for(i=0; i < sizeof(lanport)/sizeof(unsigned char); i++){
		value = 0x1;			//w_mac_cmd
		if(mode == ADDENTRY)
			value |= (7 << 4);	//w_age_field
		value |= (lanport[i] << 7);	//w_index
		reg_write(REG_ESW_WT_MAC_AD0, value);
		wait_switch_done();
	}
}

static void ZeroEntry(struct group *entry, int port, int mode)
{
	int i, value;
	char wholestr[13];
	char tmpstr[9];

	sprintf(wholestr, "%s%02x%02x%02x", "01005e", entry->a1, entry->a2, entry->a3);

	strncpy(tmpstr, wholestr, 8);
	tmpstr[8] = '\0';

	value = strtoul(tmpstr, NULL, 16);
	reg_write(REG_ESW_WT_MAC_AD2, value);
	strncpy(tmpstr, &wholestr[8], 4);
	tmpstr[4] = '\0';
	value = strtoul(tmpstr, NULL, 16);
	reg_write(REG_ESW_WT_MAC_AD1, value);


	/* update the zero entry */
	value = 0x1;				//w_mac_cmd
	if(mode == ADDENTRY)
		value |= (7 << 4);		//w_age_field
	value |= (port << 7);			//w_index
	reg_write(REG_ESW_WT_MAC_AD0, value);
	wait_switch_done();
	return;
}

void updateMacTable_specialtag(struct group *entry, unsigned char new_portmap, int delay_delete)
{
	int i;
	unsigned char lanport[] = LANPORT_RANGE;

	for(i=0; i < sizeof(lanport)/sizeof(unsigned char); i++){
		unsigned char old_bit, new_bit;

		old_bit = entry->port_map & (0x1 << lanport[i]);
		new_bit = new_portmap  & (0x1 << lanport[i]);

		if(old_bit == new_bit)
			continue;
		
		if(old_bit == 0 /* new_bit == 0x1 */){
			ZeroEntry(entry, lanport[i], DELENTRY);
		}else{
			ZeroEntry(entry, lanport[i], ADDENTRY);
		}
	}
}

/*
 * ripped from user/rt2880/switch/switch.c
 */
void updateMacTable(struct group *entry, int delay_delete)
{
	int i, value;
	char wholestr[13];
	char tmpstr[9];

	sprintf(wholestr, "%s%02x%02x%02x", "01005e", entry->a1, entry->a2, entry->a3);

	strncpy(tmpstr, wholestr, 8);
	tmpstr[8] = '\0';

	value = strtoul(tmpstr, NULL, 16);
	reg_write(REG_ESW_WT_MAC_AD2, value);
	strncpy(tmpstr, &wholestr[8], 4);
	tmpstr[4] = '\0';
	value = strtoul(tmpstr, NULL, 16);
	reg_write(REG_ESW_WT_MAC_AD1, value);

	value = 0;
	if(entry->port_map){
		/*
		 * force all mulicast addresses to bind with CPU.
		 */
		value = value | (0x1 << 18);

		/*
		 * fill the port map
		 */
		value = value | (entry->port_map & (0x7f)) << 12;
		value += (7 << 4); //w_age_field
		value += (LAN_VLAN_IDX << 7); //w_age_field
		value += 1;				//w_mac_cmd
		reg_write(REG_ESW_WT_MAC_AD0, value);
		wait_switch_done();

		/*
		 * new an additional entry for IGMP Inquery/Report on WAN.
		 */
		if(WANPORT){
			value = (WANPORT << 12);
			value |= (1 << 18);
			value |= (7 << 4);		//w_age_field
			value |= (WAN_VLAN_IDX << 7);		//w_index
			value |= 1;				//w_mac_cmd
			reg_write(REG_ESW_WT_MAC_AD0, value);
			wait_switch_done();
		}
	}else{
		if(delay_delete == ZEROED){
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
			value |= (7 << 4);		//w_age_field, keep it alive
			value |= 1;				//w_mac_cmd
			reg_write(REG_ESW_WT_MAC_AD0, value);
			wait_switch_done();
		}else if (delay_delete == DELETED){
			/*
			 * delete the entry
			 */
			value |= 1;				//w_mac_cmd
			value |= (LAN_VLAN_IDX << 7); //w_age_field
			reg_write(REG_ESW_WT_MAC_AD0, value);
			wait_switch_done();

			/*
			 * delete the additional entry on WAN.
			 */
			value = 0;
			value |= (WAN_VLAN_IDX  << 7);		//w_index
			value |= 1;				//w_mac_cmd
			reg_write(REG_ESW_WT_MAC_AD0, value);
			wait_switch_done();
		}
	}
}

int portLookUpByMac(char *mac)
{
	unsigned long long int  mac1, mac2;
	unsigned int i = 0, mac_iter;
	char mac_entry1[16], mac_entry2[16];

	sync_internal_mac_table(NULL);

	memset(mac_entry1, 0, sizeof(mac_entry1));
	memset(mac_entry2, 0, sizeof(mac_entry2));

	strncpy(mac_entry1, mac, 8);
	strncpy(mac_entry2, &mac[8], 4);

	mac1 = strtoll(mac_entry1, 0, 16);
	mac2 = strtol(mac_entry2, 0, 16);

	mac_iter = internal_mac_table[i].mac1;
	while(i < 0x3fe && mac_iter != END_OF_MAC_TABLE) {
		//log(LOG_WARNING, 0, "look for [%s] (%d)%08x %04x, %08x %04x\n", mac, i, internal_mac_table[i].mac1, internal_mac_table[i].mac2, mac1, mac2);

		if(internal_mac_table[i].vidx != LAN_VLAN_IDX)
			goto next_entry;

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
			case 0x40:	/* CPU Only */
				break;
			default:
				log(LOG_WARNING, 0, "No/Multi ports found:%x", internal_mac_table[i].port_map);
				return -1;
			}
		}
next_entry:
		i++;
		mac_iter = internal_mac_table[i].mac1;
	}

	return -1;
}

#ifdef CONFIG_RAETH_SPECIAL_TAG
void ZeroEntriesBarrier(struct group *entry, int mode)
{
	unsigned char lanport[] = LANPORT_RANGE;

	int i, value;
	char wholestr[13];
	char tmpstr[9];

	sprintf(wholestr, "%s%02x%02x%02x", "01005e", entry->a1, entry->a2, entry->a3);

	strncpy(tmpstr, wholestr, 8);
	tmpstr[8] = '\0';

	value = strtoul(tmpstr, NULL, 16);
	reg_write(REG_ESW_WT_MAC_AD2, value);
	strncpy(tmpstr, &wholestr[8], 4);
	tmpstr[4] = '\0';
	value = strtoul(tmpstr, NULL, 16);
	reg_write(REG_ESW_WT_MAC_AD1, value);

	for(i=0; i < sizeof(lanport)/sizeof(unsigned char); i++){
		value = 0x1;			//w_mac_cmd
		if(mode == ADDENTRY)
			value |= (7 << 4);	//w_age_field
		value |= (lanport[i] << 7);	//w_index
		reg_write(REG_ESW_WT_MAC_AD0, value);
		wait_switch_done();
	}
}

static void ZeroEntry(struct group *entry, int port, int mode)
{
	int i, value;
	char wholestr[13];
	char tmpstr[9];

	sprintf(wholestr, "%s%02x%02x%02x", "01005e", entry->a1, entry->a2, entry->a3);

	strncpy(tmpstr, wholestr, 8);
	tmpstr[8] = '\0';

	value = strtoul(tmpstr, NULL, 16);
	reg_write(REG_ESW_WT_MAC_AD2, value);
	strncpy(tmpstr, &wholestr[8], 4);
	tmpstr[4] = '\0';
	value = strtoul(tmpstr, NULL, 16);
	reg_write(REG_ESW_WT_MAC_AD1, value);


	/* update the zero entry */
	value = 0x1;				//w_mac_cmd
	if(mode == ADDENTRY)
		value |= (7 << 4);		//w_age_field
	value |= (port << 7);			//w_index
	reg_write(REG_ESW_WT_MAC_AD0, value);
	wait_switch_done();
	return;
}

void updateMacTable_specialtag(struct group *entry, unsigned char new_portmap, int delay_delete)
{
	int i;
	unsigned char lanport[] = LANPORT_RANGE;

	for(i=0; i < sizeof(lanport)/sizeof(unsigned char); i++){
		unsigned char old_bit, new_bit;

		old_bit = entry->port_map & (0x1 << lanport[i]);
		new_bit = new_portmap  & (0x1 << lanport[i]);

		if(old_bit == new_bit)
			continue;
		
		if(old_bit == 0 /* new_bit == 0x1 */){
			ZeroEntry(entry, lanport[i], DELENTRY);
		}else{
			ZeroEntry(entry, lanport[i], ADDENTRY);
		}
	}
}
#endif
