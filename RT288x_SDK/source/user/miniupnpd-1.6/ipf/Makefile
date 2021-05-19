# $Id: //WIFI_SOC/MP/SDK_4_3_0_0/RT288x_SDK/source/user/miniupnpd-1.6/ipf/Makefile#1 $
CC=gcc
CFLAGS=-Wall -g -I.

all:	testipfrdr

clean:
	rm *.o testipfrdr

testipfrdr:	testipfrdr.o ipfrdr.o
	$(CC) -o $@ $^

ipfrdr.o:	ipfrdr.c

testipfrdr.o:	testipfrdr.c

