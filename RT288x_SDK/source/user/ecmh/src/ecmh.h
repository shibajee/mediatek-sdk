/**************************************
 ecmh - Easy Cast du Multi Hub
 by Jeroen Massar <jeroen@unfix.org>
***************************************
 $Author: red.hung $
 $Id: //WIFI_SOC/MP/SDK_4_3_0_0/RT288x_SDK/source/user/ecmh/src/ecmh.h#1 $
 $Date: 2014/05/07 $
**************************************/

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <syslog.h>
#include <pwd.h>
#include <getopt.h>

#include <net/if.h>
#include <netinet/if_ether.h>
#include <sched.h>
#ifdef linux
#include <netpacket/packet.h>
#endif
#ifdef __FreeBSD__
#include <fcntl.h>
#include <sys/uio.h>
#include <netinet/in_systm.h>
#include <net/ethernet.h>
#include <net/bpf.h>
#define ETH_P_IPV6 ETHERTYPE_IPV6
#define ETH_P_IP ETHERTYPE_IP
#endif
#include "ifaddrs.h"
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/icmp6.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <sys/ioctl.h>
#include "mld.h"

//#ifdef MLD_SNOOPING
#include "ralink.h"
//#endif

#define PIDFILE "/var/run/ecmh.pid"
#define ECMH_DUMPFILE "/var/run/ecmh.dump"

#define ECMH_VERSION_STRING "Easy Cast du Multi Hub (ecmh) %s by Jeroen Massar <jeroen@unfix.org>\n"

#ifdef DEBUG
#define D(x) x
#else
#define D(x) {}
#endif

/* The timeout for queries */
/* as per RFC3810 MLDv2 "9.2.  Query Interval" */
#define ECMH_SUBSCRIPTION_TIMEOUT	125

/* Robustness Factor, per RFC3810 MLDv2 "9.1.  Robustness Variable" */
#define ECMH_ROBUSTNESS_FACTOR		2


#include "linklist.h"
#include "common.h"

/* Booleans */
#define false	0
#define true	(!false)
#define bool	int

#include "interfaces.h"
#include "groups.h"
#include "grpint.h"
#include "subscr.h"

/* Our configuration structure */
struct conf
{
	unsigned int		maxgroups;
	unsigned int		maxinterfaces;			/* The max number of interfaces the array can hold */
	struct intnode		*ints;				/* The interfaces we are watching */
	struct list		*groups;			/* The groups we are joined to */

	char			*upstream;			/* Upstream interface */
	unsigned int		upstream_id;			/* Interface ID of upstream interface */

	bool			daemonize;			/* To Daemonize or to not to Daemonize */
	bool			verbose;			/* Verbose Operation ? */
	bool			quit;				/* Global Quit signal */
#ifdef ECMH_SUPPORT_MLD2
	bool			mld1only;			/* Only MLDv1 ? */
	bool			mld2only;			/* Only MLDv2 ? */
#endif
	bool			promisc;			/* Make interfaces promisc? (To be sure to receive all MLD's) */
	
	uint8_t			*buffer;			/* Our buffer */
	unsigned int		bufferlen;			/* Length of the buffer */

#ifndef ECMH_BPF
	int			rawsocket;			/* Single RAW socket for sending and receiving everything */
#else
	bool			tunnelmode;			/* Intercept&handle proto-41 packets? */
	struct list		*locals;			/* Local devices that could have tunnels */
	fd_set			selectset;			/* Selectset */
	int			hifd;				/* Highest File Descriptor */
#endif
	bool			dynamic_intf;			/* Dynamic interfaces */
	bool			es;				/* enable mld snooping for Ralink Switch */
	bool			pi_flag;			/* enable mld snooping for Ralink Switch */
	char 			*positive_intf[8];

	FILE			*stat_file;			/* The file handle of ourdump file */
	time_t			stat_starttime;			/* When did we start */
	uint64_t		stat_packets_received;		/* Number of packets received */
	uint64_t		stat_packets_sent;		/* Number of packets forwarded */
	uint64_t		stat_bytes_received;		/* Number of bytes received */
	uint64_t		stat_bytes_sent;		/* Number of bytes forwarded */
	uint64_t		stat_icmp_received;		/* Number of ICMP's received */
	uint64_t		stat_icmp_sent;			/* Number of ICMP's sent */
	uint64_t		stat_hlim_exceeded;		/* Packets that where dropped due to hlim == 0 */
};

/* Global Stuff */
extern struct conf *g_conf;

/* Special option types for padding. */
#define IP6OPT_PAD1 0
#define IP6OPT_PADN 1


#if __GNUC_PREREQ(4,0)
/* nothing */
#elif __GNUC_PREREQ(3,4)
struct ip6_ext {
	unsigned char ip6e_nxt;
	unsigned char ip6e_len;
};
#endif


/*
   form RFC4291
   ...
   Nodes must not originate a packet to a multicast address whose scop
   field contains the reserved value 0; if such a packet is received, it
   must be silently dropped.  Nodes should not originate a packet to a
   multicast address whose scop field contains the reserved value F; if
   such a packet is sent or received, it must be treated the same as
   packets destined to a global (scop E) multicast address.
   ...
 */
 
#define IN6_IS_ADDR_MC_RESERVED_0(a) \
        (IN6_IS_ADDR_MULTICAST(a)                                             \
         && ((((__const uint8_t *) (a))[1] & 0xf) == 0x0))

#define IN6_IS_ADDR_MC_RESERVED_F(a) \
        (IN6_IS_ADDR_MULTICAST(a)                                             \
         && ((((__const uint8_t *) (a))[1] & 0xf) == 0xf))

 
#ifndef ICMP6_MEMBERSHIP_QUERY
#define ICMP6_MEMBERSHIP_QUERY MLD_LISTENER_QUERY
#endif
 
#ifndef ICMP6_MEMBERSHIP_REPORT
#define ICMP6_MEMBERSHIP_REPORT MLD_LISTENER_REPORT
#endif

#ifndef ICMP6_MEMBERSHIP_REDUCTION
#define ICMP6_MEMBERSHIP_REDUCTION  MLD_LISTENER_REDUCTION
#endif

#ifndef ICMP6_DST_UNREACH_NOTNEIGHBOR  
#define ICMP6_DST_UNREACH_NOTNEIGHBOR ICMP6_DST_UNREACH_BEYONDSCOPE 
#endif
