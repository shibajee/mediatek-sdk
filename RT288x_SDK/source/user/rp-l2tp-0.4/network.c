/***********************************************************************
*
* network.c
*
* Code for handling the UDP socket we send/receive on.  All of our
* tunnels use a single UDP socket which stays open for the life of
* the application.
*
* Copyright (C) 2002 by Roaring Penguin Software Inc.
*
* This software may be distributed under the terms of the GNU General
* Public License, Version 2, or (at your option) any later version.
*
* LIC: GPL
*
***********************************************************************/

static char const RCSID[] =
"$Id: //WIFI_SOC/MP/SDK_4_3_0_0/RT288x_SDK/source/user/rp-l2tp-0.4/network.c#1 $";

#include "l2tp.h"
#include "event.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>

/* Our socket */
int Sock = -1;

static EventHandler *NetworkReadHandler = NULL;
static void network_readable(EventSelector *es,
			     int fd,
			     unsigned int flags,
			     void *data);
char Hostname[MAX_HOSTNAME];

static void
sigint_handler(int sig)
{
    static int count = 0;

    count++;
    fprintf(stderr, "In sigint handler: %d\n", count);
    if (count < 5) {
	l2tp_cleanup();
    }
    exit(1);
}

/**********************************************************************
* %FUNCTION: network_init
* %ARGUMENTS:
*  es -- an event selector
* %RETURNS:
*  >= 0 if all is OK, <0 if not
* %DESCRIPTION:
*  Initializes network; opens socket on UDP port 1701; sets up
*  event handler for incoming packets.
***********************************************************************/
int
l2tp_network_init(EventSelector *es)
{
    struct sockaddr_in me;
    int flags;

    gethostname(Hostname, sizeof(Hostname));
    Hostname[sizeof(Hostname)-1] = 0;

    Event_HandleSignal(es, SIGINT, sigint_handler);
    if (Sock >= 0) {
	if (NetworkReadHandler) {
	    Event_DelHandler(es, NetworkReadHandler);
	    NetworkReadHandler = NULL;
	}
	close(Sock);
	Sock = -1;
    }
    Sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (Sock < 0) {
	l2tp_set_errmsg("network_init: socket: %s", strerror(errno));
	return -1;
    }

    me.sin_family = AF_INET;
    me.sin_addr = Settings.listen_addr;
    me.sin_port = htons((uint16_t) Settings.listen_port);
    if (bind(Sock, (struct sockaddr *) &me, sizeof(me)) < 0) {
	l2tp_set_errmsg("network_init: bind: %s", strerror(errno));
	close(Sock);
	Sock = -1;
	return -1;
    }

    /* Set socket non-blocking */
    flags = fcntl(Sock, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(Sock, F_SETFL, flags);

    /* Set up the network read handler */
    Event_AddHandler(es, Sock, EVENT_FLAG_READABLE,
		     network_readable, NULL);
    return Sock;
}

/**********************************************************************
* %FUNCTION: network_readable
* %ARGUMENTS:
*  es -- event selector
*  fd -- socket
*  flags -- event-handling flags telling what happened
*  data -- not used
* %RETURNS:
*  Nothing
* %DESCRIPTION:
*  Called when a packet arrives on the UDP socket.
***********************************************************************/
static void
network_readable(EventSelector *es,
		 int fd,
		 unsigned int flags,
		 void *data)
{
    l2tp_dgram *dgram;

    struct sockaddr_in from;
    dgram = l2tp_dgram_take_from_wire(&from);
    if (!dgram) return;

    /* It's a control packet if we get here */
    l2tp_tunnel_handle_received_control_datagram(dgram, es, &from);
    l2tp_dgram_free(dgram);
}
