TOPDIR=../
include $(TOPDIR)Rules.mak

CONFOPTS= --host=mipsel-linux

LIB_LDFLAGS := $(subst -z defs,,$(LDFLAGS))

all: Makefile
	make -f Makefile all

Makefile:
	AR= AS= CC=$(CROSS_COMPILE)gcc CROSS= CROSS_COMPILE= CXX= HOSTCC= LD=$(CROSS_COMPILE)ld OBJCOPY= RANLIB= STRIP= GCC_DIR= ./configure $(CONFOPTS)

# the is a proxy section --YY
all-recursive: Makefile
	make -f Makefile all-recursive

# the is a proxy section --YY
all-am: Makefile
	make -f Makefile all-am

# the is a proxy section --YY
clean-am: Makefile
	make -f Makefile clean-am

clean: Makefile
	rm -rf *.so *.a
	make -f Makefile clean
	find ./ -name Makefile -exec rm -f {} \;

romfs:

shared: all
	$(INSTALL) -m 644 ./lib/.libs/libnl-genl-3.so.200 $(TOPDIR)lib
	$(INSTALL) -m 644 ./lib/.libs/libnl-3.so.200 $(TOPDIR)lib
	$(INSTALL) -m 644 ./lib/.libs/libnl-idiag-3.so.200 $(TOPDIR)lib
	$(INSTALL) -m 644 ./lib/.libs/libnl-route-3.so.200 $(TOPDIR)lib
	$(INSTALL) -m 644 ./lib/.libs/libnl-nf-3.so.200 $(TOPDIR)lib

	$(LN) -sf $(TOPDIR)lib/libnl-genl-3.so.200 $(TOPDIR)lib/libnl-genl-3.so
	$(LN) -sf $(TOPDIR)lib/libnl-3.so.200 $(TOPDIR)lib/libnl-3.so
	$(LN) -sf $(TOPDIR)lib/libnl-idiag-3.so.200 $(TOPDIR)lib/lib-idiag-3.so
	$(LN) -sf $(TOPDIR)lib/libnl-route-3.so.200 $(TOPDIR)lib/lib-route-3.so
	$(LN) -sf $(TOPDIR)lib/libnl-nf-3.so.200 $(TOPDIR)lib/lib-nf-3.so


