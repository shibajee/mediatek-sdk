/* $Id: //WIFI_SOC/MP/SDK_4_3_0_0/RT288x_SDK/source/user/miniupnpd-1.6/linux/getifstats.c#1 $ */
/* MiniUPnP project
 * http://miniupnp.free.fr/ or http://miniupnp.tuxfamily.org/
 * (c) 2006-2011 Thomas Bernard
 * This software is subject to the conditions detailed
 * in the LICENCE file provided within the distribution */

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <time.h>

#include "../config.h"
#include "../getifstats.h"

#ifdef HW_NAT_EBL
#if !defined (CONFIG_RALINK_MT7620) && ! defined (CONFIG_RALINK_MT7621)
#include "ac_ioctl.h"
#include "ac_api.h"
#endif
#endif // HW_NAT_EBL //

int
getifstats(const char * ifname, struct ifdata * data)
{
	FILE *f;
	char line[512];
	char * p;
	int i;
	int r = -1;
	char fname[64];
#ifdef ENABLE_GETIFSTATS_CACHING
	static time_t cache_timestamp = 0;
	static struct ifdata cache_data;
	time_t current_time;
#endif
	if(!data)
		return -1;
	data->baudrate = 4200000;	/* that is the answer */
	data->opackets = 0;
	data->ipackets = 0;
	data->obytes = 0;
	data->ibytes = 0;
	if(!ifname || ifname[0]=='\0')
		return -1;
#ifdef ENABLE_GETIFSTATS_CACHING
	current_time = time(NULL);
	if(current_time == ((time_t)-1)) {
		syslog(LOG_ERR, "getifstats() : time() error : %m");
	} else {
		if(current_time < cache_timestamp + GETIFSTATS_CACHING_DURATION) {
			/* return cached data */
			memcpy(data, &cache_data, sizeof(struct ifdata));
			return 0;
		}
	}
#endif
	f = fopen("/proc/net/dev", "r");
	if(!f) {
		syslog(LOG_ERR, "getifstats() : cannot open /proc/net/dev : %m");
		return -1;
	}
	/* discard the two header lines */
	if(!fgets(line, sizeof(line), f) || !fgets(line, sizeof(line), f)) {
		syslog(LOG_ERR, "getifstats() : error reading /proc/net/dev : %m");
	}
	while(fgets(line, sizeof(line), f)) {
		p = line;
		while(*p==' ') p++;
		i = 0;
		while(ifname[i] == *p) {
			p++; i++;
		}
		/* TODO : how to handle aliases ? */
		if(ifname[i] || *p != ':')
			continue;
		p++;
		while(*p==' ') p++;
		data->ibytes = strtoull(p, &p, 0) % 0xffffffff;
		while(*p==' ') p++;
		data->ipackets = strtoull(p, &p, 0) % 0xffffffff;
		/* skip 6 columns */
		for(i=6; i>0 && *p!='\0'; i--) {
			while(*p==' ') p++;
			while(*p!=' ' && *p) p++;
		}
		while(*p==' ') p++;
		data->obytes = strtoull(p, &p, 0) % 0xffffffff;
		while(*p==' ') p++;
		data->opackets = strtoull(p, &p, 0) % 0xffffffff;
		r = 0;
		break;
	}
	fclose(f);

#ifdef HW_NAT_EBL
#if defined (CONFIG_RALINK_MT7620) && defined (CONFIG_RALINK_MT7621)
	  struct hwnat_qos_args
	  int vid = 0;
	  static int ipackets =0;
	  static int ibytes = 0;
	  static int opackets = 0;
	  static int obytes = 0;

	  for(vid=1;vid<=2;vid++) {
		  args.ag_index = vid;
		  GetAcEntry(&args, HW_NAT_GET_AC_CNT);
#ifdef CONFIG_RA_HW_NAT_ACCNT_MAINTAINER
		  ipackets = args.ag_pkt_cnt;
		  ibytes = args.ag_byte_cnt;
		  opackets = args.ag_pkt_cnt;
		  obytes = args.ag_byte_cnt;
#else
		  ipackets += args.ag_pkt_cnt;
		  ibytes += args.ag_byte_cnt;
		  opackets += args.ag_pkt_cnt;
		  obytes += args.ag_byte_cnt;
#endif
	  }
#else
	  struct ac_args args;
	  int vid = 0;
	  static int ipackets =0;
	  static int ibytes = 0;
	  static int opackets = 0;
	  static int obytes = 0;

	  for(vid=1;vid<=2;vid++) {
		  args.vid = vid;
		  GetAcEntry(&args, AC_GET_VLAN_UL_PKT_CNT);
		  ipackets += args.cnt;
		  GetAcEntry(&args, AC_GET_VLAN_UL_BYTE_CNT);
		  ibytes += args.cnt;

		  GetAcEntry(&args, AC_GET_VLAN_DL_PKT_CNT);
		  opackets += args.cnt;
		  GetAcEntry(&args, AC_GET_VLAN_DL_BYTE_CNT);
		  obytes += args.cnt;
	  }
#endif // HW_NAT_EBL //
		  
	  data->ipackets += ipackets;
	  data->ibytes += ibytes;
	  data->opackets += opackets;
	  data->obytes += obytes;
#endif

	/* get interface speed */
	snprintf(fname, sizeof(fname), "/sys/class/net/%s/speed", ifname);
	f = fopen(fname, "r");
	if(f) {
		if(fgets(line, sizeof(line), f)) {
			data->baudrate = 1000000*atoi(line);
		}
		fclose(f);
	} else {
		syslog(LOG_WARNING, "cannot read %s file : %m", fname);
	}
#ifdef ENABLE_GETIFSTATS_CACHING
	if(r==0 && current_time!=((time_t)-1)) {
		/* cache the new data */
		cache_timestamp = current_time;
		memcpy(&cache_data, data, sizeof(struct ifdata));
	}
#endif
	return r;
}

