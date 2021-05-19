TOPDIR=../
include $(TOPDIR)Rules.mak

LIBUPNP=libupnp166.a
LIBIXML=libixml166.a
LIBTHREADUTIL=libthreadutil166.a
CFLAGS += -I$(ROOTDIR)/lib/include -I$(ROOTDIR)/lib/libupnp-1.6.6 -I$(ROOTDIR)/lib-1.6.6/libupnp/threadutil/inc 
CFLAGS += -I$(ROOTDIR)/lib/libupnp-1.6.6/ixml/inc -I$(ROOTDIR)/lib/libupn-1.6.6/upnp/inc

all: $(LIBUPNP) $(LIBIXML) $(LIBTHREADUTIL)

shared: all
	$(MAKE) -C ixml shared
	$(MAKE) -C threadutil shared
	$(MAKE) -C upnp shared


$(LIBUPNP): 
	$(MAKE) -C upnp

$(LIBIXML): 
	$(MAKE) -C ixml

$(LIBTHREADUTIL): 
	$(MAKE) -C threadutil

romfs:
	$(MAKE) -C ixml romfs
	$(MAKE) -C threadutil romfs
	$(MAKE) -C upnp romfs
	
$(OBJS): Makefile

clean:
	$(MAKE) -C ixml clean
	$(MAKE) -C threadutil clean
	$(MAKE) -C upnp clean

