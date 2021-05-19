#
# pppstats makefile
# $Id: //WIFI_SOC/MP/SDK_4_3_0_0/RT288x_SDK/source/user/ppp-2.4.2/pppstats/Makefile#1 $
#

PPPSTATSRCS = pppstats.c
PPPSTATOBJS = pppstats.o

COPTS = -O
COMPILE_FLAGS = -I../include
LIBS =


CFLAGS = $(COPTS) $(COMPILE_FLAGS)

all: pppstats

romfs: pppstats
	$(ROMFSINST) /bin/pppstats

pppstats: $(PPPSTATSRCS)
	$(CC) $(CFLAGS) -o pppstats pppstats.c $(LIBS)

clean:
	rm -f pppstats *~ #* core

depend:
	cpp -M $(CFLAGS) $(PPPSTATSRCS) >.depend
#	makedepend $(CFLAGS) $(PPPSTATSRCS)
