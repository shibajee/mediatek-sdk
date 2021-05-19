/* $Id: //WIFI_SOC/MP/SDK_4_3_0_0/RT288x_SDK/source/user/miniupnpd-1.6/upnpevents.h#1 $ */
/* MiniUPnP project
 * http://miniupnp.free.fr/ or http://miniupnp.tuxfamily.org/
 * (c) 2008-2011 Thomas Bernard
 * This software is subject to the conditions detailed
 * in the LICENCE file provided within the distribution */

#ifndef __UPNPEVENTS_H__
#define __UPNPEVENTS_H__

#include "config.h"

#ifdef ENABLE_EVENTS
enum subscriber_service_enum {
 EWanCFG = 1,
 EWanIPC,
#ifdef ENABLE_L3F_SERVICE
 EL3F,
#endif
#ifdef ENABLE_6FC_SERVICE
 E6FC,
#endif
#ifdef ENABLE_DP_SERVICE
 EDP,
#endif
#ifdef ENABLE_WSC_SERVICE
 EWSC,
#endif
};

//Ralink : move from to upnpevents.c
#if 1//def ENABLE_WSC_SERVICE
/* stuctures definitions */
struct subscriber {
	LIST_ENTRY(subscriber) entries;
	struct upnp_event_notify * notify;
	time_t timeout;
	uint32_t seq;
	enum subscriber_service_enum service;
	char uuid[42];
	char callback[];
};
#endif

void
upnp_event_var_change_notify(enum subscriber_service_enum service);

const char *
upnpevents_addSubscriber(const char * eventurl,
                         const char * callback, int callbacklen,
                         int timeout);

int
upnpevents_removeSubscriber(const char * sid, int sidlen);

int
renewSubscription(const char * sid, int sidlen, int timeout);

void upnpevents_selectfds(fd_set *readset, fd_set *writeset, int * max_fd);
void upnpevents_processfds(fd_set *readset, fd_set *writeset);

#ifdef USE_MINIUPNPDCTL
void write_events_details(int s);
#endif

#endif
#endif
