#
SHELL:=/bin/bash
#
CFLAGS += -I$(ROOTDIR)/lib/libnvram
ifeq ($(shell [[ "$(CONFIG_MTK_VOIP)" == "y" || "$(CONFIG_MTK_TR069)" == "y" ]] && echo true), true)
CFLAGS += -I$(ROOTDIR)/user/openssl-0.9.8e/include
endif
ifeq ($(CONFIG_WSC_INCLUDED), y)
WFLAGS += -DRT2860_WSC_SUPPORT -DRTDEV_WSC_SUPPORT
ifeq ($(CONFIG_WSC_V2_SUPPORT), y)
WFLAGS += -DRT2860_WSCV2_SUPPORT -DRTDEV_WSCV2_SUPPORT
endif
endif
ifeq ($(CONFIG_RT2860V2_AP_WSC), y)
WFLAGS += -DRT2860_WSC_SUPPORT
ifeq ($(CONFIG_RT2860V2_AP_WSC_V2), y)
WFLAGS += -DRT2860_WSCV2_SUPPORT
endif
endif

#WFLAGS += -DRT2860_TXBF_SUPPORT -DRTDEV_TXBF_SUPPORT

ifeq ($(CONFIG_WDS_SUPPORT), y)
WFLAGS += -DRT2860_WDS_SUPPORT -DRTDEV_WDS_SUPPORT
endif
ifeq ($(CONFIG_RT2860V2_AP_WDS), y)
WFLAGS += -DRT2860_WDS_SUPPORT
endif

ifeq ($(CONFIG_MBSS_SUPPORT), y)
WFLAGS += -DRT2860_MBSS_SUPPORT -DRTDEV_MBSS_SUPPORT
ifeq ($(CONFIG_ENHANCE_NEW_MBSSID_MODE), y)
WFLAGS += -DRT2860_NEW_MBSS_SUPPORT -DRTDEV_NEW_MBSS_SUPPORT
endif
endif
ifeq ($(CONFIG_RT2860V2_AP_MBSS), y)
WFLAGS += -DRT2860_MBSS_SUPPORT
ifeq ($(CONFIG_NEW_MBSSID_MODE), y)
WFLAGS += -DRT2860_NEW_MBSS_SUPPORT
endif
endif

ifeq ($(CONFIG_IGMP_SNOOP_SUPPORT), y)
WFLAGS += -DRT2860_IGMPSNOOP_SUPPORT -DRTDEV_IGMPSNOOP_SUPPORT
endif
ifeq ($(CONFIG_RT2860V2_AP_IGMP_SNOOP), y)
WFLAGS += -DRT2860_IGMPSNOOP_SUPPORT
endif

ifeq ($(CONFIG_DFS_SUPPORT), y)
WFLAGS += -DRT2860_DFS_SUPPORT -DRTDEV_DFS_SUPPORT
endif
ifeq ($(CONFIG_RT2860V2_AP_DFS), y)
WFLAGS += -DRT2860_DFS_SUPPORT
endif

ifeq ($(CONFIG_CARRIER_DETECTION_SUPPORT), y)
WFLAGS += -DRT2860_CARRIER_SUPPORT -DRTDEV_CARRIER_SUPPORT
endif
ifeq ($(CONFIG_RT2860V2_AP_CARRIER), y)
WFLAGS += -DRT2860_CARRIER_SUPPORT
endif

ifneq ($(CONFIG_QOS_DLS_SUPPORT), y)
WFLAGS += -DRT2860_DLS_SUPPORT -DRTDEV_DLS_SUPPORT
endif
ifneq ($(CONFIG_RT2860V2_AP_DLS), y)
WFLAGS += -DRT2860_DLS_SUPPORT
endif

ifeq ($(CONFIG_DOT11N_DRAFT3), y)
WFLAGS += -DRT2860_11NDRAFT3_SUPPORT -DRTDEV_11NDRAFT3_SUPPORT
endif
ifeq ($(CONFIG_RT2860V2_AP_80211N_DRAFT3), y)
WFLAGS += -DRT2860_11NDRAFT3_SUPPORT
endif

ifeq ($(CONFIG_WAPI_SUPPORT), y)
WFLAGS += -DRT2860_WAPI_SUPPORT -DRTDEV_WAPI_SUPPORT
endif
ifeq ($(CONFIG_RT2860V2_AP_WAPI), y)
WFLAGS += -DRT2860_WAPI_SUPPORT
endif

#WFLAGS += -DRT2860_HS_SUPPORT -DRTDEV_HS_SUPPORT
ifeq ($(CONFIG_APCLI_SUPPORT), y)
WFLAGS += -DRT2860_APCLI_SUPPORT
endif

WFLAGS += -DRT2860_GBAND_SUPPORT
ifneq ($(findstring y, $(CONFIG_RALINK_RT2883)$(CONFIG_RALINK_RT3883)),)
WFLAGS += -DRT2860_ABAND_SUPPORT
endif
ifneq ($(findstring $(CONFIG_RT_FIRST_CARD), 5592 3593 3572 5572),)
WFLAGS += -DRT2860_ABAND_SUPPORT
endif
ifneq ($(findstring $(CONFIG_RT_FIRST_CARD), 7610 7612 7637),)
WFLAGS += -DRT2860_ABAND_SUPPORT -DRT2860_AC_SUPPORT
endif
WFLAGS += -DRTDEV_GBAND_SUPPORT
ifneq ($(findstring $(CONFIG_RT_SECOND_CARD), 5592 3593 3572 5572),)
WFLAGS += -DRTDEV_ABAND_SUPPORT
endif
ifneq ($(findstring $(CONFIG_RT_SECOND_CARD), 7610 7612 7637),)
WFLAGS += -DRTDEV_ABAND_SUPPORT -DRTDEV_AC_SUPPORT
endif

ifneq ($(findstring y, $(CONFIG_RALINK_RT2883)$(CONFIG_RALINK_RT3883)),)
WFLAGS += -DRT2860_3T3R_SUPPORT
endif
ifneq ($(findstring $(CONFIG_RT_FIRST_CARD), 3593),)
WFLAGS += -DRT2860_3T3R_SUPPORT
endif
ifneq ($(findstring y, $(CONFIG_RALINK_RT3050_1T1R)$(CONFIG_RALINK_RT5350)),)
WFLAGS += -DRT2860_1T1R_SUPPORT
endif
ifneq ($(findstring $(CONFIG_RT_SECOND_CARD), 3593),)
WFLAGS += -DRTDEV_3T3R_SUPPORT
endif
ifneq ($(findstring $(CONFIG_RT_SECOND_CARD), 7610),)
WFLAGS += -DRTDEV_1T1R_SUPPORT
endif

#WFLAGS += -DENHANCED_AP_STATUS_INFO

LDFLAGS += -L$(ROOTDIR)/lib/libnvram -L.
LDFLAGS += -L$(ROOTDIR)/user/openssl-0.9.8e -L.
LDNVRAMLIB += -lnvram 
LDWEBLIB += -lwebutil
ifeq ($(shell [[ "$(CONFIG_MTK_VOIP)" == "y" || "$(CONFIG_MTK_TR069)" == "y" ]] && echo true), true)
LDWEBLIB += -lcrypto -lssl
endif

GET_CGI = web
SET_CGI = internet.cgi wireless.cgi makeRequest.cgi firewall.cgi adm.cgi
ifeq ($(shell [[ "$(CONFIG_MTK_VOIP)" == "y" || "$(CONFIG_MTK_TR069)" == "y" ]] && echo true), true)
SET_CGI += voip.cgi tr069.cgi tr069upload_ca.cgi tr069upload_cc.cgi
endif
ifneq ($(findstring y, $(CONFIG_RALINKAPP_SWQOS)$(CONFIG_RALINKAPP_HWQOS)),)
SET_CGI += qos.cgi
endif
ifneq ($(findstring y, $(CONFIG_USB_STORAGE)$(CONFIG_MTK_MMC)),)
SET_CGI += nas.cgi
endif
ifneq ($(findstring y, $(CONFIG_USER_UVC_STREAM)$(CONFIG_USER_P910ND)$(CONFIG_USER_MTDAAPD)$(CONFIG_USER_MINIDLNA)),)
SET_CGI += usb.cgi
endif
EXEC= init_system
WEBLIBS = libwebutil.so
UPLOAD_CGI = upload.cgi upload_bootloader.cgi upload_settings.cgi \
	     ExportSettings.sh history.sh History PatchList
ifeq ($(CONFIG_RALINKAPP_WAPI), y)
UPLOAD_CGI += upload_wapi_as_cert.cgi upload_wapi_user_cert.cgi
endif

LINUX_CONF_H= $(ROOTDIR)/$(LINUXDIR)/include/linux/autoconf.h
BUSYBOX_CONF_H= $(ROOTDIR)/user/busybox/include/autoconf.h
USER_CONF_H= $(ROOTDIR)/config/autoconf.h




all: $(WEBLIBS) $(SET_CGI) $(GET_CGI) $(EXEC) crc32.o
	$(CC) $(CFLAGS) -DUPLOAD_FIRMWARE_SUPPORT -o upload.cgi upload.c crc32.o
	$(CC) $(CFLAGS) -o upload_settings.cgi upload_settings.c
	$(CC) $(CFLAGS) -DUPLOAD_BOOTLOADER_SUPPORT -o upload_bootloader.cgi upload.c crc32.o
ifeq ($(CONFIG_RALINKAPP_WAPI), y)
	$(CC) $(CFLAGS) -DUPLOAD_WAPI_AS_CERT_SUPPORT -o upload_wapi_as_cert.cgi upload_cert.c crc32.o
	$(CC) $(CFLAGS) -DUPLOAD_WAPI_USER_CERT_SUPPORT -o upload_wapi_user_cert.cgi upload_cert.c crc32.o
endif

romfs:
	cp $(ROOTDIR)/History .
	cp $(ROOTDIR)/PatchList .
	$(ROMFSINST) /lib/$(WEBLIBS)
	$(ROMFSINST) /bin/$(EXEC)
	$(ROMFSINST) /bin/$(GET_CGI)
	rm -rf cgi-bin; mkdir cgi-bin; cp *.cgi cgi-bin
	for i in $(SET_CGI); do cp $$i cgi-bin; done
	for i in $(UPLOAD_CGI); do cp $$i cgi-bin; done
	$(ROMFSINST) cgi-bin /etc_ro/lighttpd/www/cgi-bin

utils.o: utils.c
	$(CC) -c $(CFLAGS) -fPIC $(LDFLAGS) -o $@ $^ $(LDNVRAMLIB)

$(WEBLIBS): utils.o
	$(CC) -shared -Wl,-soname,$@ $(LDFLAGS) -o $@ $^ $(LDNVRAMLIB)

crc32.o: crc32.c
	$(CC) $(CFLAGS) -DUSE_HOSTCC -c crc32.c -o $@

%.cgi: %.c $(LINUX_CONF_H) $(USER_CONF_H) $(BUSYBOX_CONF_H)
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -o $@ $< $(LDNVRAMLIB) $(LDWEBLIB)

web: webtool.c oid.h $(LINUX_CONF_H) $(USER_CONF_H) $(BUSYBOX_CONF_H)
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -o $@ $< $(LDNVRAMLIB) $(LDWEBLIB)

init_system: init_system.c $(LINUX_CONF_H) $(USER_CONF_H) $(BUSYBOX_CONF_H)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< $(LDNVRAMLIB) $(LDWEBLIB)

clean:
	rm -f $(SET_CGI) $(GET_CGI) $(EXEC) $(WEBLIBS) *.cgi *.o *.so.*
	rm -rf cgi-bin
