# $Id: //WIFI_SOC/MP/SDK_4_3_0_0/RT288x_SDK/source/user/miniupnpd-1.6/ipfw/Makefile#1 $
CC=gcc
CFLAGS=-Wall -g -I.
RM=rm -f

all:	testipfwrdr

clean:
	$(RM) *.o testipfwrdr

testipfwrdr:	testipfwrdr.o ipfwrdr.o
	$(CC) -o $@ $^

ipfwrdr.o:	ipfwrdr.c

testipfwrdr.o:	testipfwrdr.c

