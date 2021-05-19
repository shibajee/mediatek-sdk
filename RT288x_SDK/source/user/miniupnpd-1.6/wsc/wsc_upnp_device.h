

#ifndef __WSC_UPNP_DEVICE_H__
#define __WSC_UPNP_DEVICE_H__

int
WscDevGetAPSettings(
	IN struct upnphttp * h,
	IN uint32 ipAddr,
    OUT IXML_Document * out,
    OUT char **errorString );
int
WscDevGetSTASettings(
	IN struct upnphttp * h,
	IN uint32 ipAddr,
	OUT IXML_Document * out,
	OUT char **errorString);
int
WscDevSetAPSettings(
	IN struct upnphttp * h,
	IN uint32 ipAddr,
	OUT IXML_Document * out,
	OUT char **errorString);
int
WscDevDelAPSettings(
	IN struct upnphttp * h,
	IN uint32 ipAddr,
	OUT IXML_Document * out,
	OUT char **errorString);
int
WscDevSetSTASettings(
	IN struct upnphttp * h,
	IN uint32 ipAddr,
	OUT IXML_Document * out,
	OUT char **errorString);
int
WscDevRebootAP(
	IN struct upnphttp * h,
	IN uint32 ipAddr,
	OUT IXML_Document * out,
	OUT char **errorString);
int 
WscDevResetAP(
	IN struct upnphttp * h,
	IN uint32 ipAddr,
	OUT IXML_Document * out,
	OUT char **errorString);
int
WscDevRebootSTA(
	IN struct upnphttp * h,
	IN uint32 ipAddr,
	OUT IXML_Document * out,
	OUT char **errorString);
int
WscDevResetSTA(
	IN struct upnphttp * h,
	IN uint32 ipAddr,
	OUT IXML_Document * out,
	OUT char **errorString);
int WscDevSetSelectedRegistrar(
	IN struct upnphttp * h,
	IN uint32 ipAddr,
	OUT IXML_Document * out,
	OUT char **errorString);
int
WscDevPutWLANResponse(
	IN struct upnphttp * h,
	IN uint32 ipAddr,
	OUT IXML_Document * out,
	OUT char **errorString);

int
WscDevPutMessageResp(
	IN WscEnvelope *msgEnvelope);

int 
WscDevPutMessage(
	IN struct upnphttp * h,
	IN uint32 ipAddr,
	OUT IXML_Document * out,
	OUT char **errorString);

int
WscDevGetDeviceInfoResp(
	IN WscEnvelope *envelope);

int
WscDevGetDeviceInfo(
	IN struct upnphttp * h,
	IN uint32 ipAddr,
	OUT IXML_Document * out,
	OUT char **errorString);

#ifdef __cplusplus
}
#endif

#endif
