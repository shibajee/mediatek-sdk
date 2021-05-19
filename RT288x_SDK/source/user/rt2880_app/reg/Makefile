EXEC = reg
OBJS = reg.o
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/net/rt_rdm -I$(ROOTDIR)/$(LINUXDIR)/include

ifeq ($(CONFIG_DEFAULTS_MEDIATEK_MT7623),y)
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/mediatek/kernel/include 
endif

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

romfs:
	$(ROMFSINST) /bin/$(EXEC)

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o

