TOPDIR=../../
include $(TOPDIR)Rules.mak

CFLAGS+=-DHAVE_CONFIG_H -Iinc -I../ -I$(ROOTDIR)/lib/include

LIBTHREADUTIL=libthreadutil166.a
LIBTHREADUTIL_VERS=1.6.6
LIBTHREADUTIL_SHARED=libthreadutil166.so.$(LIBTHREADUTIL_VERS)

OBJS    := src/FreeList.o src/LinkedList.o  src/ThreadPool.o  src/TimerThread.o

all: $(LIBTHREADUTIL)

$(OBJS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

shared: all
	$(LD) $(LDFLAGS) -soname=$(LIBTHREADUTIL_SHARED) \
		-o $(LIBTHREADUTIL_SHARED) --whole-archive $(LIBTHREADUTIL) \
		--no-whole-archive \
		-L$(TOPDIR)lib -lc -lpthread $(LDADD_LIBFLOAT) $(LIBGCC);
	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(LIBTHREADUTIL_SHARED)
	$(INSTALL) -m 644 $(LIBTHREADUTIL_SHARED) $(TOPDIR)lib


$(LIBTHREADUTIL) ar-target: $(OBJS)
	$(AR) $(ARFLAGS) $(LIBTHREADUTIL) $(OBJS)
	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(LIBTHREADUTIL)
	$(INSTALL) -m 644 $(LIBTHREADUTIL) $(TOPDIR)lib

romfs:
	$(INSTALL) -m 644 $(LIBTHREADUTIL_SHARED) $(TOPDIR)lib

$(OBJS): Makefile

clean:
	$(RM) *.[oa] $(LIBTHREADUTIL_SHARED)* 

