#LDFLAGS+=-L../../lib/libnl-3.2.11/lib/.libs		-lnl-3
LDFLAGS+=-lm

CFLAGS=			-g -I. -MMD \
			-Wall  -DL2TP_CMD_ROOT=\"l2tpv3tun\"  -DSYSCONFDIR=\"/mnt\"
#-Werror

#CFLAGS += -I../../lib/libnl-3.2.11/include
CFLAGS += -I./include -I$(ROOTDIR)/$(LINUXDIR)/include
#INSTALL?=	install

PROGS=		l2tpv3tun
LIB=./lib
GENL=./lib/genl
UTILOBJ=$(LIB)/utils.o $(LIB)/rt_names.o $(LIB)/ll_types.o $(LIB)/ll_proto.o $(LIB)/ll_addr.o $(LIB)/inet_proto.o

NLOBJ=$(LIB)/ll_map.o $(LIB)/libnetlink.o $(LIB)/msg.o $(LIB)/attr.o $(LIB)/nl.o $(LIB)/handlers.o $(LIB)/socket.o \
		$(LIB)/cache.o $(LIB)/object.o $(LIB)/libnl_utils.o $(LIB)/cache_mngr.o $(LIB)/cache_mngt.o $(LIB)/error.o \
		$(LIB)/data.o $(LIB)/addr.o $(LIB)/nl.o $(LIB)/ipx_ntop.o

GENLOBJ=$(GENL)/ctrl.o $(GENL)/family.o $(GENL)/genl.o $(GENL)/mngt.o
all:	$(PROGS)

l2tpv3tun:	main.o  $(GENLOBJ) $(UTILOBJ) $(NLOBJ)
	$(CC) -o $@ $^ $(LDFLAGS)
#l2tpv3tun_utils.o

clean:
	$(RM) $(wildcard *.o) $(wildcard *.d) $(PROGS) $(GENLOBJ) $(UTILOBJ) $(subst .o,.d,$(NLOBJ)) \
		$(subst .o,.d,$(UTILOBJ)) $(subst .o,.d,$(GENLOBJ))

install:
	$(INSTALL) -d $(DESTDIR)/usr/bin
	$(INSTALL) $(PROGS) $(DESTDIR)/usr/bin

sinclude $(wildcard *.d)

romfs:
	$(ROMFSINST) /bin/$(PROGS)
