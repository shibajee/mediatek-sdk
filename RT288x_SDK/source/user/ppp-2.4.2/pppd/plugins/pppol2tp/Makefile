COPTS = -O2

ifneq ($(KERNEL_HEADERS),)
CFLAGS = $(COPTS) -I. -I../.. -I$(ROOTDIR)/$(KERNEL_HEADERS)/include -fPIC
else
CFLAGS = $(COPTS) -I. -I../.. -I$(ROOTDIR)/$(LINUXDIR)/include -fPIC
endif

VERSION = 0.16
PLUGINS := pppol2tp.so openl2tp.so

all: $(PLUGINS)

%.so: %.o
	$(CC) $(CFLAGS) -o $@ -shared $^ $(LIBS)

romfs: all
	$(ROMFSINST) /etc_ro/ppp/plugins/pppol2tp.so
	$(ROMFSINST) /etc_ro/ppp/plugins/openl2tp.so

clean:
	rm -f *.o *.so

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
