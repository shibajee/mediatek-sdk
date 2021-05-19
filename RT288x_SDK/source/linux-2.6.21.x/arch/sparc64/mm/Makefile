# $Id: //WIFI_SOC/MP/SDK_4_3_0_0/RT288x_SDK/source/linux-2.6.21.x/arch/sparc64/mm/Makefile#1 $
# Makefile for the linux Sparc64-specific parts of the memory manager.
#

EXTRA_AFLAGS := -ansi
EXTRA_CFLAGS := -Werror

obj-y    := ultra.o tlb.o tsb.o fault.o init.o generic.o

obj-$(CONFIG_HUGETLB_PAGE) += hugetlbpage.o
