
#CFLAGS	+= -I$(ROOTDIR)/lib/libnvram
#CFLAGS  += -I$(ROOTDIR)/$(LINUXDIR)/drivers/flash 
#CFLAGS  += -mips16 -mno-abicalls

#LDLIBS	+= -lnvram

#CONF_H	= $(ROOTDIR)/$(LINUXDIR)/include/linux/autoconf.h
#UCONF_H	= $(ROOTDIR)/config/autoconf.h

all:	pciephy pciephy.o
#
#	Primary link
#
pciephy: pciephy.o
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS) $(LDLIBS)

pciephy.o: pciephy.c
	$(CC) -o $@ -DLIBRARY_BUILD -c pciephy.c $(CFLAGS) $(LDFLAGS) $(LDLIBS)

romfs:
	$(STRIP) pciephy
	$(ROMFSINST) /bin/pciephy

clean:
	rm -f pciephy pciephy.o

