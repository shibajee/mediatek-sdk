#
# Makefile for the linux Journalling Flash FileSystem (JFFS) routines.
#
# $Id: //WIFI_SOC/MP/SDK_4_3_0_0/RT288x_SDK/source/linux-2.4.x/fs/jffs2/Makefile#1 $
#
# Note! Dependencies are done automagically by 'make dep', which also
# removes any old dependencies. DON'T put your own dependencies here
# unless it's something special (ie not a .c file).
#
# Note 2! The CFLAGS definitions are now in the main makefile...


COMPR_OBJS	:= compr.o compr_rubin.o compr_rtime.o pushpull.o \
			compr_zlib.o
JFFS2_OBJS	:= dir.o file.o ioctl.o nodelist.o malloc.o \
	read.o nodemgmt.o readinode.o super.o write.o scan.o gc.o \
	symlink.o build.o erase.o background.o

O_TARGET := jffs2.o

obj-y := $(COMPR_OBJS) $(JFFS2_OBJS)
obj-m := $(O_TARGET)

include $(TOPDIR)/Rules.make

