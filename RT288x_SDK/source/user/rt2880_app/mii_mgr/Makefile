EXEC = mii_mgr mii_mgr_cl45
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/net/raeth

all: $(EXEC)

mii_mgr: mii_mgr.o
	$(CC) $(LDFLAGS) -o $@ $^

mii_mgr_cl45: mii_mgr_cl45.o
	$(CC) $(LDFLAGS) -o $@ $^

romfs:
	$(ROMFSINST) /bin/mii_mgr
	$(ROMFSINST) /bin/mii_mgr_cl45

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o

