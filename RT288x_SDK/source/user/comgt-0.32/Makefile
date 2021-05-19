#
#   Makefile - build and install the comgt package
#   Copyright (C) 2005  Martin Gregorie
#   Copyright (C) 2006  Paul Hardwick
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
#    martin@gregorie.org, paul@peck.org.uk
#
#    $Id: //WIFI_SOC/MP/SDK_4_3_0_0/RT288x_SDK/source/user/comgt-0.32/Makefile#1 $
#
#

CPROG	= comgt
CFLAGS  := -c $(CFLAGS)
LDFLAGS := $(LDFLAGS)

all: $(CPROG)

romfs:
	$(ROMFSINST) /bin/$(CPROG)
	$(ROMFSINST) scripts/devices /etc_ro/ppp/3g
	$(ROMFSINST) scripts/command /etc_ro/ppp/3g
	$(ROMFSINST) scripts/dump /etc_ro/ppp/3g
	$(ROMFSINST) scripts/operator /etc_ro/ppp/3g
	$(ROMFSINST) scripts/sigmon /sbin
	$(ROMFSINST) scripts/read_sms /sbin
	$(ROMFSINST) scripts/send_sms /sbin

clean:
	-rm *.o 
	-rm $(CPROG) 
	-rm *~
	-rm $(SCRIPTSRC)*~


comgt: comgt.o
	$(CC) comgt.o $(LDFLAGS) -o comgt

comgt.o: comgt.c comgt.h
	$(CC) comgt.c $(CFLAGS) 

