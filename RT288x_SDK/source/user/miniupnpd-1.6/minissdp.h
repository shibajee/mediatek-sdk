/* $Id: //WIFI_SOC/MP/SDK_4_3_0_0/RT288x_SDK/source/user/miniupnpd-1.6/minissdp.h#1 $ */
/* MiniUPnP project
 * http://miniupnp.free.fr/ or http://miniupnp.tuxfamily.org/
 * (c) 2006-2007 Thomas Bernard
 * This software is subject to the conditions detailed
 * in the LICENCE file provided within the distribution */
#ifndef __MINISSDP_H__
#define __MINISSDP_H__

#include "miniupnpdtypes.h"

int
OpenAndConfSSDPReceiveSocket(int ipv6);

int
OpenAndConfSSDPNotifySockets(int * sockets);
/*OpenAndConfSSDPNotifySockets(int * sockets,
                             struct lan_addr_s * lan_addr, int n_lan_addr);*/

/*void
SendSSDPNotifies(int s, const char * host, unsigned short port,
                 unsigned int lifetime);*/
void
SendSSDPNotifies2(int * sockets,
                  unsigned short port,
                  unsigned int lifetime);
/*SendSSDPNotifies2(int * sockets, struct lan_addr_s * lan_addr, int n_lan_addr,
                  unsigned short port,
                  unsigned int lifetime);*/

#ifdef ENABLE_WSC_SERVICE
void
WSC_SendSSDPNotifies2(int * sockets,
                  unsigned short port,
                  unsigned int lifetime);
int
WSC_SendSSDPGoodbye(int * sockets, int n);
#endif /* ENABLE_WSC_SERVICE */

void
ProcessSSDPRequest(int s, unsigned short port);
/*ProcessSSDPRequest(int s, struct lan_addr_s * lan_addr, int n_lan_addr,
                   unsigned short port);*/

void
ProcessSSDPData(int s, const char *bufr, int n,
                const struct sockaddr * sendername, unsigned short port);

int
SendSSDPGoodbye(int * sockets, int n);

int
SubmitServicesToMiniSSDPD(const char * host, unsigned short port);

#endif

