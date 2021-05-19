VERSION = 0.81

COPTS = -O2

ifneq ($(KERNEL_HEADERS),)
CFLAGS = $(COPTS) -I. -I../../.. -I$(ROOTDIR)/$(KERNEL_HEADERS)/include -fPIC
else
CFLAGS = $(COPTS) -I. -I../../.. -I$(ROOTDIR)/$(LINUXDIR)/include -fPIC
endif

PLUGINS := pptp.so

all: $(PLUGINS)

OBJS = dirutil.o pptp.o pptp_ctrl.o util.o \
       orckit_quirks.o pptp_callmgr.o pptp_quirks.o vector.o

%.so: $(OBJS)
	$(CC) $(CFLAGS) -o $@ -shared $^ $(LIBS)

romfs: all
	$(ROMFSINST) /etc_ro/ppp/plugins/$(PLUGINS)

clean:
	rm -f *.o *.so

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
