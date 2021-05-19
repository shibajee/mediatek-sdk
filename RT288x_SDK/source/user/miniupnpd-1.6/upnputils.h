/* $Id: //WIFI_SOC/MP/SDK_4_3_0_0/RT288x_SDK/source/user/miniupnpd-1.6/upnputils.h#1 $ */
/* MiniUPnP project
 * http://miniupnp.free.fr/ or http://miniupnp.tuxfamily.org/
 * (c) 2011-2012 Thomas Bernard
 * This software is subject to the conditions detailed
 * in the LICENCE file provided within the distribution */

#ifndef __UPNPUTILS_H__
#define __UPNPUTILS_H__

/**
 * convert a struct sockaddr to a human readable string.
 * [ipv6]:port or ipv4:port
 * return the number of characters used (as snprintf)
 */
int
sockaddr_to_string(const struct sockaddr * addr, char * str, size_t size);

/**
 * set the file description as non blocking
 * return 0 in case of failure, 1 in case of success
 */
int
set_non_blocking(int fd);

#endif

