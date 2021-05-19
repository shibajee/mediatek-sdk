# $Id: //WIFI_SOC/MP/SDK_4_3_0_0/RT288x_SDK/source/linux-2.6.21.x/arch/cris/kernel/Makefile#1 $
#
# Makefile for the linux kernel.
#

extra-y	:= vmlinux.lds

obj-y   := process.o traps.o irq.o ptrace.o setup.o \
	   time.o sys_cris.o semaphore.o

obj-$(CONFIG_MODULES)    += crisksyms.o
obj-$(CONFIG_MODULES)	 += module.o
obj-$(CONFIG_SYSTEM_PROFILER)   += profile.o

clean:

