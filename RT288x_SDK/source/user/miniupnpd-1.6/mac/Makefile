# $Id: //WIFI_SOC/MP/SDK_4_3_0_0/RT288x_SDK/source/user/miniupnpd-1.6/mac/Makefile#1 $
# made for GNU Make
CFLAGS = -Wall -g
EXECUTABLES = testgetifstats

all:	$(EXECUTABLES)

clean:
	rm -f *.o $(EXECUTABLES)

testmacrdr.o:	testmacrdr.c macrdr.h

testgetifstats:	testgetifstats.o getifstats.o

