# Generated automatically from Makefile.in by configure.
#***********************************************************************
#
# Makefile
#
# Makefile for Roaring Penguin's Linux PPPoE plugin.
#
# Copyright (C) 2001 Roaring Penguin Software Inc.
#
# This program may be distributed according to the terms of the GNU
# General Public License, version 2 or (at your option) any later version.
#
# $Id: //WIFI_SOC/MP/SDK_4_3_0_0/RT288x_SDK/source/user/ppp-2.4.2/pppd/plugins/rp-pppoe/Makefile#1 $
#***********************************************************************

# Version is set ONLY IN THE MAKEFILE!  Don't delete this!
VERSION=3.3

COPTS=-O2 -g
CFLAGS=$(COPTS) -I../../../include/linux
all: rp-pppoe.so

rp-pppoe.so: libplugin.a plugin.o
	$(LD) -o rp-pppoe.so -shared plugin.o libplugin.a

romfs: all
	$(ROMFSINST) /etc_ro/ppp/plugins/rp-pppoe.so
	$(ROMFSINST) /sbin/pppoe.sh

clean:
	rm -f *.o *.so

plugin.o: plugin.c
	$(CC) '-DRP_VERSION="$(VERSION)"' $(CFLAGS) -I../../..  -c -o plugin.o -fPIC plugin.c

libplugin.a: discovery.o if.o common.o debug.o
	$(AR) -rc $@ $^

discovery.o: discovery.c
	$(CC) $(CFLAGS) '-DVERSION="$(VERSION)"' -c -o discovery.o -fPIC discovery.c

if.o: if.c
	$(CC) $(CFLAGS) '-DVERSION="$(VERSION)"' -c -o if.o -fPIC if.c

debug.o: debug.c
	$(CC) $(CFLAGS) '-DVERSION="$(VERSION)"' -c -o debug.o -fPIC debug.c

common.o: common.c
	$(CC) $(CFLAGS) '-DVERSION="$(VERSION)"' -c -o common.o -fPIC common.c

