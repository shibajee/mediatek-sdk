# $Id: //WIFI_SOC/MP/SDK_4_3_0_0/RT288x_SDK/source/lib/libupnp-1.6.6-dms/threadutil/Makefile.am#1 $
#
# "Makefile.am" for "libupnp/threadutil"
#
# (C) Copyright 2005 Rémi Turboult <r3mi@users.sourceforge.net>
#

AM_CPPFLAGS		= -I$(srcdir)/inc -I$(srcdir)/src/inc

if ENABLE_DEBUG
    AM_CPPFLAGS		+= -DDEBUG -DSTATS
else
    AM_CPPFLAGS		+= -DNO_DEBUG -DNDEBUG
endif


lib_LTLIBRARIES		= libthreadutil.la

libthreadutil_la_LDFLAGS = -version-info $(LT_VERSION_THREADUTIL)

libthreadutil_la_SOURCES = \
			src/FreeList.c src/LinkedList.c \
			src/ThreadPool.c src/TimerThread.c

upnpincludedir		= $(includedir)/upnp
upnpinclude_HEADERS	= \
			inc/FreeList.h inc/LinkedList.h \
			inc/ThreadPool.h inc/TimerThread.h \
			inc/ithread.h

