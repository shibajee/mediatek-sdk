/*
 *  We maintain an IGMP group list:
 *
 *				group[ 01:00:5E:00:00:02 ]
 *				group[ 01:00:5E:00:00:05 ]
 *			 	...
 *
 *  In every IGMP group entry, an IP address list keeps the record who has sent IGMP report.
 *
 *       group[ 01:00:5E:00:00:02 ] --> 10.10.10.100(port1) --> 10.10.10.102(port3) -> ... -> NULL
 *       group[ 01:00:5E:00:00:05 ] --> 10.10.10.103(port3) --> NULL
 *        ...
 *
 *  By steping the IP address list, we can know the ports which the IGMP group is interesting in.
 *
 *  A mirror of rt3052 internal mac table is created in memory to improve performance. The time 
 *  interval of sync is 10 secs.
 *
 */

/*
 *  History:
 *
 * 1) Add an entry for igmp query on WAN
 * 2) Broadcast the multicast packets which not matched.
 * 3) Delay mac table deletion.
 * 4) Don't follow igmpproxy to remove an invalid (unknown destination) group entry
 * 5) "no wan port" condition.
 * 6) WiFi Snooping support: a) search STA in WiFi tables  b) using WiFi(ra0)'s internal snooping
 * 7) multiple wifi interface
 * 8) move Switch code to other .c file
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include "defs.h"
#include "ralink.h"
#include <linux/autoconf.h>
#include <linux/wireless.h>

#define MAX_MULTICASTS_GROUP		256

#define IP_GET_LOST_MAPPING(mip)	((mip & 0x0F800000) >> 23) 
#define IP_MULTICAST_A0(a0)		((a0 >> 1) | 0xE0)
#define IP_MULTICAST_A1(a0, a1)		(((a0 & 0x1) << 7) | a1)

#ifdef WIFI_IGMPSNOOP_SUPPORT
#define RTPRIV_IOCTL_GET_MAC_TABLE          (SIOCIWFIRSTPRIV + 0x0F)
#define RTPRIV_IOCTL_GET_MAC_TABLE_STRUCT   (SIOCIWFIRSTPRIV + 0x1F)
#define RTWIFI_INTFS_MAX	8
int  rtwifi_intf_count = 0;
char rtwifi_intfs[RTWIFI_INTFS_MAX][IFNAMSIZ];

void rtwifi_remove_member(uint32 m_ip_addr, uint32 u_ip_addr);
void rtwifi_remove_multicast_ip(uint32 m_ip_addr);
void rtwifi_insert_multicast_ip(uint32 m_ip_addr);
void rtwifi_insert_member(uint32 m_ip_addr, uint32 u_ip_addr);
void rtwifi_fini(void);
void rtwifi_init(void);
#endif

#if defined (CONFIG_PPE_MCAST)
#if defined (CONFIG_RAETH_GMAC2)
#define HNAT_VID	0
#else
#define HNAT_VID	2 
#endif
#endif
// function prototype
static void update_group_port_map(struct group *entry);
void updateMacTable(struct group *entry, int delay_deleted);
static int	portLookUpByIP(char *ip);

// global variables.
static struct group 	*group_list = NULL;

static int		snooping_enabled = 0;

static struct group *find_entry(uint32 ip_addr)
{
	unsigned ua1, ua2, ua3;
	struct group *pos = group_list;

	ua1 = ( ip_addr & 0x007F0000 ) >> 16;
	ua2 = ( ip_addr & 0x0000FF00 ) >> 8;
	ua3 = ( ip_addr & 0x000000FF );

	while(pos){
		if(pos->a1 == ua1 && pos->a2 == ua2 && pos->a3 == ua3)
			return pos;
		pos = pos->next;
	}
	return NULL;
}


static int arpLookUp(char *ip, char *arp)
{
	char buf[256];
	FILE *fp = fopen("/proc/net/arp", "r");
	if(!fp){
		log(LOG_ERR, 0, "*** rtGSW: no proc fs!");
		return -1;
	}

	while(fgets(buf, sizeof(buf), fp)){
		char ip_entry[32], hw_type[8], flags[8], hw_address[32];
		sscanf(buf, "%31s %7s %7s %31s", ip_entry, hw_type, flags, hw_address);
		if(!strcmp(ip, ip_entry)){
			strcpy(arp, hw_address);
			fclose(fp);
			if(!strcmp("00:00:00:00:00:00", hw_address)){
				return -1;
			}
			return 0;
		}
	}

	fclose(fp);
	return -1;
}

static void dump_entry(void)
{
	unsigned char port_map, i;
	struct group *pos = group_list;
	printf("=== Dump group entries:\n");
	while(pos){
		struct group_member *member;

		printf("01:00:5e:%02x:%02x:%02x\t", pos->a1, pos->a2, pos->a3);
		port_map = pos->port_map;
		for(i=0; i<sizeof(unsigned char); i++){
			if(port_map & 0x1){
				printf("%d ",  i);
			}
			port_map = port_map >> 1;
		}

		printf("\n");
		member = pos->members;
		while(member){
			struct in_addr tmp;
			tmp.s_addr = htonl(member->ip_addr);
			printf("\t\t%d.%d.%d.%d\t%s(%d)\tport%d\t%s\n", IP_MULTICAST_A0(member->a0), IP_MULTICAST_A1(member->a0, pos->a1), pos->a2, pos->a3, inet_ntoa(tmp), member->a0, member->port_num, member->has_report ? "reported" : "not-reported");
			member = member->next;
		}

		pos = pos->next;
		printf("\n");
	}
	printf("===\n");
}

static struct group *build_entry(uint32 m_ip_addr, uint32 u_ip_addr)
{
	unsigned char 		a1, a2, a3;
	struct in_addr 		tmp;
	struct group 		*new_entry;

	static int group_count = 0;

	// crowd control
	if(group_count++ > MAX_MULTICASTS_GROUP)
		return NULL;

	a1 = ( m_ip_addr & 0x007F0000 ) >> 16;
	a2 = ( m_ip_addr & 0x0000FF00 ) >> 8;
	a3 = ( m_ip_addr & 0x000000FF );

	/* check if "all hosts" address*/
	if(a1 == 0 && a2 == 0 && a3 == 1)
		return NULL;

	new_entry = (struct group *)malloc(sizeof(struct group));
	if(!new_entry){
		log(LOG_WARNING, 0, "*** rtGSW: Out of memory.");
		return NULL;
	}
	printf("%s, %s\n", __FUNCTION__,  inetFmt(htonl(m_ip_addr), s1));
	
	/* set up address */
	new_entry->a1 = a1;
	new_entry->a2 = a2;
	new_entry->a3 = a3;

	/* set up ip address bitmap */
	new_entry->port_map = 0x0;
	new_entry->members = NULL;
	/* GND */
	new_entry->next = NULL;
	return new_entry;
}

static struct group_member *lookup_ip_group(struct group *entry, uint32 m_ip_addr)
{
	struct group_member *pos = entry->members;
	while(pos){
		unsigned char a0 = IP_GET_LOST_MAPPING(m_ip_addr);
		if(pos->a0 == a0)
			return pos;
		pos = pos->next;
	}
	return NULL;
} 

static struct group_member *lookup_member(struct group *entry, uint32 m_ip_addr, uint32 u_ip_addr)
{
	struct group_member *pos = entry->members;
	while(pos){
		unsigned char a0 = IP_GET_LOST_MAPPING(m_ip_addr);
		if(pos->ip_addr == u_ip_addr && pos->a0 == a0)
			return pos;
		pos = pos->next;
	}
	return NULL;
} 

void remove_member(uint32 m_ip_addr, uint32 u_ip_addr)
{
	unsigned char a0;
	struct group *entry;
	struct group_member *pos;
	struct group_member *del=NULL;
#if defined (CONFIG_PPE_MCAST)
	char cmd[128];
	unsigned char *a;
#endif
	if(!snooping_enabled)
		return;
#ifdef WIFI_IGMPSNOOP_SUPPORT
	rtwifi_remove_member(m_ip_addr, u_ip_addr);
#endif

	entry = find_entry(m_ip_addr);
	if(!entry){
		log(LOG_WARNING, 0, "*** rtGSW: can't find the group [%s].", inetFmt(htonl(m_ip_addr), s1));
		return;
	}

	pos = entry->members;
	if(!pos){
		log(LOG_WARNING, 0, "*** rtGSW: group [%s] member list is empty.", inetFmt(htonl(m_ip_addr), s1));
		return;
	}

	printf("%s, %s, %s\n", __FUNCTION__,  inetFmt(htonl(m_ip_addr), s1), inetFmt(htonl(u_ip_addr), s2));
	a0 = IP_GET_LOST_MAPPING(m_ip_addr);
	if(entry->members->ip_addr == u_ip_addr && entry->members->a0 == a0){
		del = pos;
		entry->members = entry->members->next;
	}else{
		while(pos->next){
			if(pos->next->ip_addr == u_ip_addr && pos->next->a0 == a0){
				del = pos->next;
				pos->next = del->next;
				break;
			}
			pos = pos->next;
		}
	}

	if(del){
		free(del);
	}else{
		log(LOG_WARNING, 0, "************************************************");
		log(LOG_WARNING, 0, "*** rtGSW: can't delete [%s] in the group [%s].", inetFmt(htonl(u_ip_addr), s1) , inetFmt(htonl(m_ip_addr), s2));
		log(LOG_WARNING, 0, "************************************************");
	}
	update_group_port_map(entry);
	
#if defined (CONFIG_PPE_MCAST)
// multicast port0 map to PDMA
// multicast port1 map to GMAC1
// multicast port2 map to GMAC2
// multicast port3 map to QDMA

	//printf("remove: group portmap is 0x%2x\n!!!!!!!!!!!",entry->port_map);
	a = (u_char *)&m_ip_addr;
	if((entry->port_map & 0x7f) == 0 ){
#if defined (CONFIG_RAETH_QDMA)
		sprintf(cmd, "hw_nat -C %2d 01:00:5e:%02x:%02x:%02x 2 2 0",HNAT_VID,a[2],a[1],a[0]);
#else
		sprintf(cmd, "hw_nat -C %2d 01:00:5e:%02x:%02x:%02x 2 0 0",HNAT_VID,a[2],a[1],a[0]);
#endif
		system(cmd);
		printf("%s\n", cmd);
	}

	if((entry->port_map & 0x80) == 0){
#if defined (CONFIG_RAETH_QDMA)
		sprintf(cmd, "hw_nat -C %2d 01:00:5e:%02x:%02x:%02x 8 8 0",HNAT_VID,a[2],a[1],a[0]);
#else
		sprintf(cmd, "hw_nat -C %2d 01:00:5e:%02x:%02x:%02x 1 0 0",HNAT_VID,a[2],a[1],a[0]);
#endif
		system(cmd);
		printf("%s\n", cmd);
	}
#endif
	return;
}

/*
 *  ret:
 *	1 == new member is found
 *  0 == existed member
 *	-1 = error occured.
 */
static struct group_member *insert_member(struct group *entry, uint32 m_ip_addr, uint32 u_ip_addr)
{
	int rc = 0;
	struct in_addr 		tmp;
	struct group_member *new_member;
	struct group_member *pos = entry->members;
	if(entry->members != NULL){
		struct group_member *member = lookup_member(entry, m_ip_addr, u_ip_addr);
		if(member){
			 log(LOG_DEBUG, 0, "*** rtGSW: find the same member [%s] in [%s]. ", inetFmt(u_ip_addr, s1), inetFmt(m_ip_addr, s2));

			/* check if port changed */
			unsigned char port_num;

			tmp.s_addr = htonl(u_ip_addr);
			port_num = portLookUpByIP(inet_ntoa(tmp));
			if(port_num != member->port_num)
				member->port_num = port_num;
			/* update its report flag */
			member->has_report = 1;
			return NULL;
		}
	}

	/* create a new member */
	new_member = (struct group_member *)malloc(sizeof(struct group_member));
	if(!new_member){
			log(LOG_WARNING, 0, "*** rtGSW: Out of memory.");
			return NULL;
	}
	tmp.s_addr				= htonl(u_ip_addr);
	new_member->ip_addr 	= u_ip_addr;
	new_member->a0		= IP_GET_LOST_MAPPING( m_ip_addr);
	new_member->port_num 	= portLookUpByIP(inet_ntoa(tmp));
	new_member->has_report	= 1;
	new_member->next	= entry->members;

	entry->members = new_member;

	return new_member;
}

void sweap_no_report_members(void)
{
	struct group *pos = group_list;

	if(!snooping_enabled)
		return;

	while(pos){
		struct group_member *member = pos->members;

		while(member){
			struct group_member *next_backup = NULL;
			if(!member->has_report){
				unsigned int craft_mip = 0x0;

				next_backup = member->next;

				/* craft a multicast ip for remove_member() */
				craft_mip |=  (unsigned long)( IP_MULTICAST_A0(member->a0)) ;
				craft_mip |= ((unsigned long)( IP_MULTICAST_A1(member->a0, pos->a1) ) << 8 );
				craft_mip |= ((unsigned long)(pos->a2) << 16) ;
				craft_mip |= ((unsigned long)(pos->a3) << 24) ;
				
				log(LOG_WARNING, 0, "*** rtGSW!: remove [%s] in the group [%s].", inetFmt(htonl(member->ip_addr), s1) , inetFmt(craft_mip, s2));
				remove_member( ntohl(craft_mip), member->ip_addr);
			}

			if(next_backup)
				member = next_backup;
			else
				member = member->next;
		}
		pos = pos->next;
	}
}

void clear_all_entries_report(void)
{
	struct group *pos = group_list;

	if(!snooping_enabled)
		return;

	while(pos){
		struct group_member *member = pos->members;
		while(member){
			member->has_report = 0;
			member = member->next;
		}
		pos = pos->next;
	}
}

void remove_all_members(struct group *entry)
{
	struct group_member *del, *pos = entry->members;

	if(!snooping_enabled)
		return;

	while(pos){
		del = pos;
		pos = pos->next;
		free(del);
	}
	entry->members = NULL;
}

void remove_multicast_ip(uint32 m_ip_addr)
{
	unsigned char a0;
	unsigned char new_portmap = 0;
	struct group *entry = find_entry(m_ip_addr);
	struct group *group_pos = group_list;

	int delete_found = 0;
	struct group_member *mem_pos, *tmp;

	if(!snooping_enabled)
		return;

#ifdef WIFI_IGMPSNOOP_SUPPORT
	rtwifi_remove_multicast_ip(m_ip_addr);
#endif

	if(!entry){
		// This entry isn't in the list.
		log(LOG_WARNING, 0, "*** rtGSW: can't find group entry [%s].", inetFmt(htonl(m_ip_addr), s1));
		return;
	}

	a0 = IP_GET_LOST_MAPPING(m_ip_addr);
	while(entry->members && entry->members->a0 == a0){
		tmp = entry->members->next;
		free(entry->members);
		entry->members = tmp;
		delete_found = 1;
	}

	mem_pos = entry->members;
	while(mem_pos){
		if(mem_pos->next && mem_pos->next->a0 == a0){
			mem_pos->next = mem_pos->next->next;
			free(mem_pos->next);
			delete_found = 1;
		}else
			new_portmap = new_portmap | (0x1 << mem_pos->port_num);
		mem_pos = mem_pos->next;
	}

	if(delete_found)
		log(LOG_WARNING, 0, "*** RT3052: group entry [%s] found undeleted member.", inetFmt(htonl(m_ip_addr), s1));

	if(entry->members == NULL || new_portmap == 0){
		entry->port_map = 0;

		/* remove from group_list */
		if(group_list == entry){
			group_list = entry->next;
		}else{
			int found = 0;
			while(group_pos->next){
				if(group_pos->next == entry){
					group_pos->next = entry->next;
					found = 1;
					break;
				}
				group_pos = group_pos->next;
			}
			if(!found){
				log(LOG_WARNING, 0, "*** rtGSW: can't find grou entry [%s].", inetFmt(htonl(m_ip_addr), s1));
				return;
			}
		}

#ifndef CONFIG_RAETH_SPECIAL_TAG
		updateMacTable(entry, DELETED);
#else
#if defined (CONFIG_RALINK_RT3052) || defined (CONFIG_RALINK_RT3352) || defined (CONFIG_RALINK_RT5350) || defined (CONFIG_RALINK_MT7628)
		ZeroEntriesBarrier(entry, DELENTRY);
#endif
#endif
		// free myself
		free(entry);
	}else if(entry->port_map != new_portmap){
		entry->port_map = new_portmap;
		update_group_port_map(entry);
	}

	return;
}

void remove_all_groups(void)
{
	struct group *del, *pos = group_list;	

	if(!snooping_enabled)
		return;

	/*  TODO: call remove_multicast_ip() instead. */
	while(pos){
		del = pos;
		pos = pos->next;
		del->port_map = 0x0;
#ifndef CONFIG_RAETH_SPECIAL_TAG
		updateMacTable(del, DELETED);
#else
#if defined (CONFIG_RALINK_RT3052) || defined (CONFIG_RALINK_RT3352) || defined (CONFIG_RALINK_RT5350) || defined (CONFIG_RALINK_MT7628)
		ZeroEntriesBarrier(del, DELENTRY);
#endif
#endif
		remove_all_members(del);
		free(del);
	}
	group_list =  NULL;	
}

static void update_group_port_map(struct group *entry)
{
	unsigned char new_portmap = 0x0;
	struct group_member *pos = entry->members;
	while(pos){
		if(pos->port_num == -1){
			// can't find which port it's in, so opens all ports for it.
			log(LOG_WARNING, 0, "****************************************");
			log(LOG_WARNING, 0, "*** rtGSW: can't find %s's port number.", inetFmt(htonl(pos->ip_addr), s1));
			log(LOG_WARNING, 0, "****************************************");
			new_portmap =  (0x5f & ~(WANPORT)); // All Lan ports
			break;
		}else{
			new_portmap = new_portmap | (0x1 << pos->port_num);
			pos = pos->next;
		}
	}
	if(entry->port_map != new_portmap){
#ifndef CONFIG_RAETH_SPECIAL_TAG
		entry->port_map = new_portmap;
		updateMacTable(entry, ZEROED);
#else
#if defined (CONFIG_RALINK_RT3052) || defined (CONFIG_RALINK_RT3352) || defined (CONFIG_RALINK_RT5350) || defined (CONFIG_RALINK_MT7628)
		updateMacTable_specialtag(entry, new_portmap, ZEROED);
		entry->port_map = new_portmap;
#endif
#endif
	}
}

#if 1
void insert_multicast_ip(uint32 m_ip_addr, uint32 u_ip_addr)
{
	char cmd[128];
	struct group_member *new_member;
	struct group *entry = find_entry(m_ip_addr);
#if defined (CONFIG_PPE_MCAST)
	unsigned char *a;
#endif
	if(!snooping_enabled)
		return;

	if(!entry){
		// This entry isn't in the list, create one.
		if( (entry = build_entry(m_ip_addr, u_ip_addr)) == NULL)
			return;
#ifdef WIFI_IGMPSNOOP_SUPPORT
		rtwifi_insert_multicast_ip(m_ip_addr);
#endif
#ifdef CONFIG_RAETH_SPECIAL_TAG
#if defined (CONFIG_RALINK_RT3052) || defined (CONFIG_RALINK_RT3352) || defined (CONFIG_RALINK_RT5350) || defined (CONFIG_RALINK_MT7628)
		/* build a zero entry barrier for this new MC group in mac tables */
		ZeroEntriesBarrier(entry, ADDENTRY);
#endif
#endif
		if(group_list)
			entry->next = group_list;
		group_list = entry;
	}
	new_member = insert_member(entry, m_ip_addr, u_ip_addr);
#ifdef WIFI_IGMPSNOOP_SUPPORT
	if(new_member && new_member->port_num == OTHER_INTERFACE)
		rtwifi_insert_member(m_ip_addr, u_ip_addr);
#endif
	update_group_port_map(entry);
#if defined (CONFIG_PPE_MCAST)
// multicast port0 map to PDMA
// multicast port1 map to GMAC1
// multicast port2 map to GMAC2
// multicast port3 map to QDMA

	//printf("insert: group portmap is 0x%2x\n!!!!!!!!!!!",entry->port_map);
	a = (u_char *)&m_ip_addr;
	if((a[2] == 0xff) && (a[1] == 0xff))
	    return;
	if((a[2] == 0x00) && (a[1] == 0x00))
	    return;
	if(entry->port_map & 0x7f){
#if defined (CONFIG_RAETH_QDMA)
		sprintf(cmd, "hw_nat -B %2d 01:00:5e:%02x:%02x:%02x 2 2 0",HNAT_VID,a[2],a[1],a[0]);
#else
		sprintf(cmd, "hw_nat -B %2d 01:00:5e:%02x:%02x:%02x 2 0 0",HNAT_VID,a[2],a[1],a[0]);
#endif
		system(cmd);
		printf("%s\n", cmd);
	}

#ifdef WIFI_IGMPSNOOP_SUPPORT
	if(entry->port_map & 0x80){
#if defined (CONFIG_RAETH_QDMA)
		sprintf(cmd, "hw_nat -B %2d 01:00:5e:%02x:%02x:%02x 8 8 0",HNAT_VID,a[2],a[1],a[0]);
#else
		sprintf(cmd, "hw_nat -B %2d 01:00:5e:%02x:%02x:%02x 1 0 0",HNAT_VID,a[2],a[1],a[0]);
#endif
		system(cmd);
		printf("%s\n", cmd);
	}
#endif /*WIFI_IGMPSNOOP_SUPPORT*/


#endif


	return;
}
#endif
static void create_all_hosts_rule(void)
{
	struct group entry	= {
		.a1 = 0x00,
		.a2 = 0x00,
		.a3 = 0x01,
		.port_map = (0x5f & ~(WANPORT)),	/* All LAN ports */
		.next 		= NULL
	};
	updateMacTable(&entry, ZEROED);
}

static void destory_all_hosts_rule()
{
	struct group entry	= {
		.a1 = 0x00,
		.a2 = 0x00,
		.a3 = 0x01,
		.port_map = 0x0,
		.next = NULL
	};
	updateMacTable(&entry, DELETED);
}

#define DD printf("%s %d\n",__FUNCTION__, __LINE__);

void rt_fini(void)
{
	if(!snooping_enabled)
		return;

	/* del 224.0.0.1( 01:00:5e:00:00:01) from mac table */
	destory_all_hosts_rule();

	remove_all_groups();

	rt_switch_fini();
#ifdef WIFI_IGMPSNOOP_SUPPORT
	rtwifi_fini();
#endif

}

void rt_init(int se)
{
	unsigned int value;

	snooping_enabled = se;

	if(!snooping_enabled)
		return;

	rt_switch_init();

	/* add 224.0.0.1( 01:00:5e:00:00:01) to mac table */
	create_all_hosts_rule();

#ifdef WIFI_IGMPSNOOP_SUPPORT
	rtwifi_init();
#endif

}

typedef union _MACHTTRANSMIT_SETTING {
	struct  {
		unsigned short  MCS:7;  // MCS
		unsigned short  BW:1;   //channel bandwidth 20MHz or 40 MHz
		unsigned short  ShortGI:1;
		unsigned short  STBC:2; //SPACE
		unsigned short  eTxBF:1;
		unsigned short  rsv:1;
		unsigned short  iTxBF:1;
		unsigned short  MODE:2; // Use definition MODE_xxx.
	} field;
	unsigned short      word;
} MACHTTRANSMIT_SETTING;

typedef struct _RT_802_11_MAC_ENTRY {
	unsigned char			ApIdx;
	unsigned char           Addr[6];
	unsigned char           Aid;
	unsigned char           Psm;     // 0:PWR_ACTIVE, 1:PWR_SAVE
	unsigned char           MimoPs;  // 0:MMPS_STATIC, 1:MMPS_DYNAMIC, 3:MMPS_Enabled
	char                    AvgRssi0;
	char                    AvgRssi1;
	char                    AvgRssi2;
	unsigned int            ConnectedTime;
	MACHTTRANSMIT_SETTING	TxRate;
	unsigned int			LastRxRate;
	int						StreamSnr[3];
	int						SoundingRespSnr[3];
#if 0
	short					TxPER;
	short					reserved;
#endif
} RT_802_11_MAC_ENTRY;

#if defined (CONFIG_RT2860V2_AP_WAPI) || defined (CONFIG_RT3090_AP_WAPI) || \
    defined (CONFIG_RT3572_AP_WAPI) || defined (CONFIG_RT5392_AP_WAPI) || \
    defined (CONFIG_RT5572_AP_WAPI) || defined (CONFIG_RT5592_AP_WAPI) || \
    defined (CONFIG_RT3593_AP_WAPI) || defined (CONFIG_RT3680_iNIC_AP_WAPI)
#define MAX_NUMBER_OF_MAC               96
#else
#define MAX_NUMBER_OF_MAC               32 // if MAX_MBSSID_NUM is 8, this value can't be larger than 211
#endif

typedef struct _RT_802_11_MAC_TABLE {
	unsigned long            Num;
	RT_802_11_MAC_ENTRY      Entry[MAX_NUMBER_OF_MAC]; //MAX_LEN_OF_MAC_TABLE = 32
} RT_802_11_MAC_TABLE;

#ifdef WIFI_IGMPSNOOP_SUPPORT
void rtwifi_init(void)
{
	int i;
	char cmd[128];
	for(i=0; i<rtwifi_intf_count ; i++){
		sprintf(cmd, "iwpriv %s set IgmpSnEnable=1", rtwifi_intfs[i]);
printf("%s\n", cmd);
		system(cmd);
	}
}

void rtwifi_fini(void)
{
	int i;
	char cmd[128];
	for(i=0; i<rtwifi_intf_count ; i++){
		sprintf(cmd, "iwpriv %s set IgmpSnEnable=0", rtwifi_intfs[i]);
printf("%s\n", cmd);
		system(cmd);
	}
}

void rtwifi_insert_member(uint32 m_ip_addr, uint32 u_ip_addr)
{
	int i;
	char mac[32], cmd[128];
	if( arpLookUp(inetFmt( htonl(u_ip_addr), mac), s1) != -1){
		for(i=0; i<rtwifi_intf_count ; i++){
			sprintf(cmd, "iwpriv %s set IgmpAdd=%s-%s", rtwifi_intfs[i], inetFmt(htonl(m_ip_addr), s1), mac);
printf("%s\n", cmd);
			system(cmd);
		}
	}else
		log(LOG_WARNING, 0, "Can't find Mac address(%s)", u_ip_addr);
}


void rtwifi_insert_multicast_ip(uint32 m_ip_addr)
{
	int i;
	char cmd[128];
	for(i=0; i<rtwifi_intf_count ; i++){
		sprintf(cmd, "iwpriv %s set IgmpAdd=%s", rtwifi_intfs[i], inetFmt(htonl(m_ip_addr), s1));
printf("%s\n", cmd);
		system(cmd);
	}
}

void rtwifi_remove_multicast_ip(uint32 m_ip_addr)
{
	int i;
	char cmd[128];
	for(i=0; i<rtwifi_intf_count ; i++){
		sprintf(cmd, "iwpriv %s set IgmpDel=%s", rtwifi_intfs[i], inetFmt(htonl(m_ip_addr), s1));
printf("%s\n", cmd);
		system(cmd);
	}
}

void rtwifi_remove_member(uint32 m_ip_addr, uint32 u_ip_addr)
{
	int i;
	char mac[32], cmd[128];
	if( arpLookUp(inetFmt(htonl(u_ip_addr), s1), mac) != -1){
		for(i=0; i<rtwifi_intf_count ; i++){
			sprintf(cmd, "iwpriv %s set IgmpDel=%s-%s", rtwifi_intfs[i], inetFmt(htonl(m_ip_addr), s1), mac);
printf("%s\n", cmd);
			system(cmd);
		}
	}else
		log(LOG_WARNING, 0, "Can't find Mac address(%s)", inetFmt(htonl(u_ip_addr), s1));
}


int addRTWiFiIntf(char *wifi)
{
	if(strlen(wifi) > IFNAMSIZ)
		return -1;
	if(rtwifi_intf_count >= RTWIFI_INTFS_MAX)
		return -1;
	strcpy(rtwifi_intfs[rtwifi_intf_count++], wifi);
	return 0;
}

int _WiFiSTALookUPByMac(char *wifi, unsigned int mac1, unsigned int mac2)
{
	int i, s;
	struct iwreq iwr;
	RT_802_11_MAC_TABLE table = {0};

	s = socket(AF_INET, SOCK_DGRAM, 0);
	strncpy(iwr.ifr_name, wifi, IFNAMSIZ);
	iwr.u.data.pointer = (caddr_t) &table;
	if (s < 0) {
		log(LOG_WARNING, 0, "ioctl sock failed!");
		return 0;
	}
#if 1 //def CONFIG_RT2860V2_AP_V24_DATA_STRUCTURE
	if (ioctl(s, RTPRIV_IOCTL_GET_MAC_TABLE_STRUCT, &iwr) < 0) {
		log(LOG_WARNING, 0, "ioctl -> RTPRIV_IOCTL_GET_MAC_TABLE_STRUCT failed!");
#else
	if (ioctl(s, RTPRIV_IOCTL_GET_MAC_TABLE, &iwr) < 0) {
		log(LOG_WARNING, 0, "ioctl -> RTPRIV_IOCTL_GET_MAC_TABLE failed!");
#endif
		close(s);
		return 0;
	}
	close(s);

	for (i = 0; i < table.Num; i++) {
		unsigned int c_mac1 = 0, c_mac2 = 0;
#ifdef CONFIG_RT2860V2_AP_TXBF
		RT_802_11_MAC_ENTRY *pe = &(table.Entry[i]);
		c_mac1 = (pe->Addr[3]) | (pe->Addr[2] << 8) | (pe->Addr[1] << 16) | (pe->Addr[0] << 24);
		c_mac2 = pe->Addr[5] | (pe->Addr[4] << 8);
#else
		c_mac1 = (table.Entry[i].Addr[3]) | (table.Entry[i].Addr[2] << 8) | (table.Entry[i].Addr[1] << 16) | (table.Entry[i].Addr[0] << 24);
		c_mac2 = (table.Entry[i].Addr[5]) | (table.Entry[i].Addr[4] << 8);
#endif
		if(c_mac1 == mac1 && c_mac2 == mac2)
 			return 1;
	}
	return 0;
}

int WiFiSTALookUPByMac(char *mac)
{
	int i;
	unsigned long long int  mac1, mac2;
	char mac_entry1[16], mac_entry2[16];

	mac1 = strtoll(mac_entry1, 0, 16);
	mac2 = strtol(mac_entry2, 0, 16);
	memset(mac_entry1, 0, sizeof(mac_entry1));
	memset(mac_entry2, 0, sizeof(mac_entry2));
	strncpy(mac_entry1, mac, 8);
	strncpy(mac_entry2, &mac[8], 4);

	mac1 = strtoll(mac_entry1, 0, 16);
	mac2 = strtol(mac_entry2, 0, 16);

	for(i=0; i<rtwifi_intf_count; i++){
		if(_WiFiSTALookUPByMac(rtwifi_intfs[i], mac1, mac2) == 1)
			return 1;
	}
	return 0;
}
#endif


static void strip_mac(char *mac)
{
	char *pos = mac, *strip = mac;
	while(*pos != '\0'){
		if(*pos == ':')
			pos++;
		else{
			*strip = *pos;
			strip++;
			pos++;
		}
	}
	*strip = '\0';
}

/*
 * send a udp packet to target if its mac address can't be found.
 */
static void sendUDP(char *ip)
{
	int socket_fd;
	struct sockaddr_in user_addr;
	char buf[16];

	memset(&user_addr, 0, sizeof(struct sockaddr_in));
	user_addr.sin_family	= AF_INET;
	user_addr.sin_port		= htons(53);
	user_addr.sin_addr.s_addr = inet_addr(ip);

	if((socket_fd = socket(AF_INET,SOCK_DGRAM, 0)) == -1) {
		log(LOG_WARNING, 0, "*** rtGSW: socket error");
		return;
	}
	strcpy(buf, "arp please");
	sendto(socket_fd, buf, strlen(buf), 0, (struct sockaddr *)&user_addr, sizeof(user_addr));
	close(socket_fd);

	return ;
}

/*
 * Unfortunately IGMP packets from linux raw socket layer don't have layer2 header, but we
 * can get the mac address from ARP table. If no matched entry found in the table, then we send a dummy
 * udp packet to target, and wait for its ARP reply.
 *
 * Hope we won't cause target's IDS alarm.
 */
static int portLookUpByIP(char *ip)
{
	char mac[32];
	int rc;
	if( arpLookUp(ip, mac) == -1){
		log(LOG_WARNING, 0, "*** rtGSW: Warning, Can't get mac address for %s", ip);
		// send an udp then wait.
		sendUDP(ip);
		usleep(20000);	
		if(arpLookUp(ip, mac) == -1){
			log(LOG_WARNING, 0, "*** rtGSW: Give up for %s", ip);
			// means flooding.
			return -1;
		}
	}
	else{
		log(LOG_WARNING, 0, "*** rtGSW: Get mac address for %s\n", ip);
		log(LOG_WARNING, 0, "*** rtGSW: mac address for %s\n", mac);
	}
	strip_mac(mac);
	rc = portLookUpByMac(mac);

#ifdef WIFI_IGMPSNOOP_SUPPORT
	if(rc == -1){
		if( (rc = WiFiSTALookUPByMac(mac)) != -1)
			return OTHER_INTERFACE;
	}else
#endif
	return rc;
}

void sigUSR1Handler(int signo)
{
	if(!snooping_enabled)
		return;

	dump_entry();
	dump_table();
}
