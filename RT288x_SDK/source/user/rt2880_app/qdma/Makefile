EXEC = qdma  
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/net/raeth
CONF_H  = $(ROOTDIR)/$(LINUXDIR)/include/linux/autoconf.h

SRC=qdma.c

all: $(EXEC)
hw_qos: $(SRC) $(CONF_H)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(SRC) $(LDLIBS)

romfs:
	$(ROMFSINST) /bin/qdma

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o

