EXEC=igmpproxy
IGMP_PROXY_OBJ= igmpproxy.o config.o confread.o request.o udpsock.o mcgroup.o rttable.o \
		igmp.o ifvc.o callout.o kern.o syslog.o lib.o mroute-api.o

ifneq ($(KERNEL_HEADERS),)
CONF_H = $(ROOTDIR)/$(KERNEL_HEADERS)/include/linux/autoconf.h
KERNEL_H = $(ROOTDIR)/$(KERNEL_HEADERS)/include
else
CONF_H = $(ROOTDIR)/$(LINUXDIR)/include/linux/autoconf.h
KERNEL_H = $(ROOTDIR)/$(LINUXDIR)/include
endif

ifeq ("$(CONFIG_PRODUCT)", "RT3052")
IGMP_PROXY_OBJ+= ralink.o rt3052.o rt3052_reg.o
CFLAGS= -I$(KERNEL_H) -DRT3052_SUPPORT
endif

ifeq ("$(CONFIG_PRODUCT)", "RT3352")
IGMP_PROXY_OBJ+= ralink.o rt3052.o rt3052_reg.o
CFLAGS= -I$(KERNEL_H) -DRT3052_SUPPORT
endif

ifeq ("$(CONFIG_PRODUCT)", "RT5350")
IGMP_PROXY_OBJ+= ralink.o rt3052.o rt3052_reg.o
CFLAGS= -I$(KERNEL_H) -DRT3052_SUPPORT
endif

ifeq ("$(CONFIG_PRODUCT)", "RT6855")
IGMP_PROXY_OBJ+= ralink.o rtGSW.o rt3052_reg.o
CFLAGS= -I$(KERNEL_H) -DRT3052_SUPPORT
endif

ifeq ("$(CONFIG_PRODUCT)", "RT6855A")
IGMP_PROXY_OBJ+= ralink.o rtGSW.o rt3052_reg.o
CFLAGS= -I$(KERNEL_H) -DRT3052_SUPPORT
endif

ifeq ("$(CONFIG_PRODUCT)", "MT7620")
IGMP_PROXY_OBJ+= ralink.o rtGSW.o rt3052_reg.o
CFLAGS= -I$(KERNEL_H) -DRT3052_SUPPORT
endif

ifeq ("$(CONFIG_PRODUCT)", "MT7621")
IGMP_PROXY_OBJ+= ralink.o rtGSW.o rt3052_reg.o
CFLAGS= -I$(KERNEL_H) -DRT3052_SUPPORT
endif

ifeq ("$(CONFIG_PRODUCT)", "MT7628")
IGMP_PROXY_OBJ+= ralink.o rt3052.o rt3052_reg.o
CFLAGS= -I$(KERNEL_H) -DRT3052_SUPPORT
endif


CFLAGS+= -I$(ROOTDIR)/$(LINUXDIR)/drivers/net/raeth

#CFLAGS+= -DWIFI_IGMPSNOOP_SUPPORT

all: $(EXEC)

igmpproxy: build.h $(IGMP_PROXY_OBJ) $(CONF_H)
	$(CC) $(LDFLAGS) -o $@ $(IGMP_PROXY_OBJ)

romfs:
	$(ROMFSINST) /bin/$(EXEC)
	$(ROMFSINST) /bin/igmpproxy.sh

clean:
	rm -f *.o *.asm build.h $(EXEC)  

build.h	:  
	echo '#define BUILD "' `date +%y%m%d` '"' >build.h
