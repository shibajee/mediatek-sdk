/* $Id: //WIFI_SOC/MP/SDK_4_3_0_0/RT288x_SDK/source/user/miniupnpd-1.6/upnpsoap.h#1 $ */
/* MiniUPnP project
 * http://miniupnp.free.fr/ or http://miniupnp.tuxfamily.org/
 * (c) 2006 Thomas Bernard
 * This software is subject to the conditions detailed
 * in the LICENCE file provided within the distribution */

#ifndef __UPNPSOAP_H__
#define __UPNPSOAP_H__

/* ExecuteSoapAction():
 * this method executes the requested Soap Action */
void
ExecuteSoapAction(struct upnphttp *, const char *, int);

/* SoapError():
 * sends a correct SOAP error with an UPNPError code and
 * description */
void
SoapError(struct upnphttp * h, int errCode, const char * errDesc);

#ifdef ENABLE_WSC_SERVICE
#include "wsc/wsc_common.h"
#include "wsc/wsc_msg.h"

extern int
WscDevGetAPSettings(
	IN struct upnphttp * h,
	IN uint32 ipAddr,
    OUT IXML_Document * out,
    OUT char **errorString );
extern int
WscDevGetSTASettings(
	IN struct upnphttp * h,
	IN uint32 ipAddr,
	OUT IXML_Document * out,
	OUT char **errorString);
extern int
WscDevSetAPSettings(
	IN struct upnphttp * h,
	IN uint32 ipAddr,
	OUT IXML_Document * out,
	OUT char **errorString);
extern int
WscDevDelAPSettings(
	IN struct upnphttp * h,
	IN uint32 ipAddr,
	OUT IXML_Document * out,
	OUT char **errorString);
extern int
WscDevSetSTASettings(
	IN struct upnphttp * h,
	IN uint32 ipAddr,
	OUT IXML_Document * out,
	OUT char **errorString);
extern int
WscDevRebootAP(
	IN struct upnphttp * h,
	IN uint32 ipAddr,
	OUT IXML_Document * out,
	OUT char **errorString);
extern int 
WscDevResetAP(
	IN struct upnphttp * h,
	IN uint32 ipAddr,
	OUT IXML_Document * out,
	OUT char **errorString);
extern int
WscDevRebootSTA(
	IN struct upnphttp * h,
	IN uint32 ipAddr,
	OUT IXML_Document * out,
	OUT char **errorString);
extern int
WscDevResetSTA(
	IN struct upnphttp * h,
	IN uint32 ipAddr,
	OUT IXML_Document * out,
	OUT char **errorString);
extern int 
WscDevSetSelectedRegistrar(
	IN struct upnphttp * h,
	IN uint32 ipAddr,
	OUT IXML_Document * out,
	OUT char **errorString);
extern int
WscDevPutWLANResponse(
	IN struct upnphttp * h,
	IN uint32 ipAddr,
	OUT IXML_Document * out,
	OUT char **errorString);
extern int 
WscDevPutMessage(
	IN struct upnphttp * h,
	IN uint32 ipAddr,
	OUT IXML_Document * out,
	OUT char **errorString);
extern int
WscDevGetDeviceInfo(
	IN struct upnphttp * h,
	IN uint32 ipAddr,
	OUT IXML_Document * out,
	OUT char **errorString);
#endif /* ENABLE_WSC_SERVICE */

#endif

