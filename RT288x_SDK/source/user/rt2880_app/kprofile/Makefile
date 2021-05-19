#CFLAGS= -mips1 -msoft-float 
#CFLAGS= -mips1 -msoft-float 
#CFLAGS= -mips32r2 -msoft-float -DTRENDCHIP -muclibc

ifeq ($(TC2031_SUPPORT),1)
CFLAGS += -DTC2031_DEBUG
endif

ifneq ($(strip $(TCSUPPORT_CODE_SHRINK)),)
CFLAGS += -Os
endif

ifneq ($(TC_CFLAGS), )
CFLAGS += $(TC_CFLAGS)
endif

LDFLAGS += -L ../lib #-ltcapi

OBJS=profiling.o

all: profiling

.c.o:
	$(CC) -c $(CFLAGS) $<

profiling: $(OBJS) 
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

romfs:
	$(ROMFSINST) /bin/profiling

clean:
	rm -f *.o *.a $(EXECS)
