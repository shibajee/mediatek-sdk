# Generated automatically from Makefile.in by configure.
# $Id: //WIFI_SOC/MP/SDK_4_3_0_0/RT288x_SDK/source/user/rp-l2tp-0.4/handlers/Makefile#1 $
# Makefile for LNS handlers
# LIC: GPL

HANDLERS=sync-pppd.so cmd.so

OBJS=pty.o sync-pppd.o dstring.o
SRCS=$(OBJS:.o=.c)
CFLAGS=-O2 -I.. -I../libevent -I../../libevent -Wall

all: $(HANDLERS) l2tp-control

l2tp-control: cmd-control.o
	$(CC) -o l2tp-control cmd-control.o

cmd-control.o: cmd-control.c
	$(CC) -c -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<

sync-pppd.so: pty.o sync-pppd.o
	$(CC) -shared -o $@ $^

cmd.so: cmd.o dstring.o
	$(CC) -shared -o $@ $^

clean:
	rm -f *.so *.o *~
	rm -f l2tp-control

romfs: all
	$(ROMFSINST) /sbin/l2tp-control
	$(ROMFSINST) /etc_ro/ppp/plugins/sync-pppd.so
	$(ROMFSINST) /etc_ro/ppp/plugins/cmd.so
