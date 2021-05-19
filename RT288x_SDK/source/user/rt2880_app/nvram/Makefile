NVDIR = $(ROOTDIR)/lib/libnvram
CFLAGS += -I$(NVDIR) -I$(ROOTDIR)/$(LINUXDIR)/drivers/char -I$(ROOTDIR)/user/lighttpd-1.4.20/cgi_src
ifeq ($(findstring y, $(CONFIG_RT2860V2_AP_WSC)$(CONFIG_WSC_INCLUDED)), y)
CFLAGS += -DWSC_SUPPORT
endif
LDFLAGS += -lnvram
ifeq ($(findstring y, $(CONFIG_USER_LIGHTY)), y)
LDFLAGS += -L$(ROOTDIR)/user/lighttpd-1.4.20/cgi_src -lwebutil
endif
LIBNVRAM = $(ROOTDIR)/lib/lib/libnvram-0.9.28.so
EXEC = nvram_daemon ralink_init
CONF_H	= $(ROOTDIR)/$(LINUXDIR)/include/linux/autoconf.h
UCONF_H	= $(ROOTDIR)/config/autoconf.h

WAPI_ON=$(CONFIG_RT2860V2_AP_WAPI)$(CONFIG_RT3090_AP_WAPI)$(CONFIG_RT5392_AP_WAPI)$(CONFIG_RT5592_AP_WAPI)$(CONFIG_RT3593_AP_WAPI)$(CONFIG_RT3572_AP_WAPI)$(CONFIG_RT5572_AP_WAPI)$(CONFIG_RT3680_iNIC_AP_WAPI)$(CONFIG_WAPI_SUPPORT)
ifeq ($(findstring y, $(WAPI_ON)), y)
CFLAGS += -DWAPI_SUPPORT
endif

all: $(EXEC)

ifdef CONFIG_RT2880_L2_MANAGE
nvram_daemon: md5c.o rmmsg.o ramad.o nvram_daemon.o
else
nvram_daemon: nvram_daemon.o
endif
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

ralink_init: ralink_init.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

ralink_init.o: $(CONF_H) $(UCONF_H) $(LIBNVRAM)

nvram_daemon.o: $(CONF_H) $(UCONF_H) $(LIBNVRAM)

clean:
	rm -f *.o $(EXEC)

romfs:
	for i in $(EXEC) ; do $(ROMFSINST) /bin/$$i ; done
	ln -sf ralink_init nvram_get
	ln -sf ralink_init nvram_set
	mv -f nvram_get $(ROMFSDIR)/bin
	mv -f nvram_set $(ROMFSDIR)/bin

