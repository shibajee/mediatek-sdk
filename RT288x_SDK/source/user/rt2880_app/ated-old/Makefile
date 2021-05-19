NVDIR = $(ROOTDIR)/lib/libnvram
CFLAGS += -I$(NVDIR) 
LDFLAGS += -lnvram
EXEC = ated
SRCS = ated.c

ifneq ($(KERNEL_HEADERS),)
CONF_H  = $(ROOTDIR)/$(KERNEL_HEADERS)/include/linux/autoconf.h
else
CONF_H  = $(ROOTDIR)/$(LINUXDIR)/include/linux/autoconf.h
endif

all: $(EXEC)

$(EXEC): $(SRCS) $(CONF_H)
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(LDFLAGS) -o $@ $@.c $(LDLIBS)

romfs:
	$(ROMFSINST) /bin/$(EXEC)

clean:
	-rm -f $(EXEC) *.o

