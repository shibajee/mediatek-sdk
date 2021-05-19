EXEC = mtd_write
OBJS = mtd.o

ifneq ($(KERNEL_HEADERS),)
CONF_H  = $(ROOTDIR)/$(KERNEL_HEADERS)/include/linux/autoconf.h
else
CONF_H  = $(ROOTDIR)/$(LINUXDIR)/include/linux/autoconf.h
endif

all: $(EXEC)

$(EXEC): $(OBJS) $(CONF_H)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

romfs:
	$(ROMFSINST) /bin/$(EXEC)

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o

