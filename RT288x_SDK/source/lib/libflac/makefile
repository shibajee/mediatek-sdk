TOPDIR=../
include $(TOPDIR)Rules.mak

CONFOPTS= --host=mipsel-linux

LIB_LDFLAGS := $(subst -z defs,,$(LDFLAGS))


# upnp
LIBFLAC_A= src/libFLAC/.libs/libFLAC.a
LIBFLAC_SHARED=src/libFLAC/.libs/libFLAC.so
LIBFLAC_SHARED_FULLNAME=libFLAC-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so

all: Makefile
	make -f Makefile all

Makefile:
	AR= AS= CC=$(CROSS_COMPILE)gcc CFLAGS=-fPIC CROSS= CROSS_COMPILE= CXX=$(CROSS_COMPILE)g++ HOSTCC= LD=$(CROSS_COMPILE)ld OBJCOPY= RANLIB= STRIP= GCC_DIR= ./configure $(CONFOPTS) --with-ogg-libraries=$(ROOTDIR)/lib/libogg/src/.libs/ --with-ogg-includes=$(ROOTDIR)/lib/libogg/include

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
	$(LD) $(LIB_LDFLAGS) -soname=$(LIBFLAC_SHARED).$(MAJOR_VERSION) \
           -o $(LIBFLAC_SHARED_FULLNAME) --whole-archive $(LIBFLAC_A) \
           --no-whole-archive \
           -L$(TOPDIR)lib -lc $(LDADD_LIBFLOAT);

	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(LIBFLAC_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBFLAC_SHARED).$(MAJOR_VERSION)
	$(INSTALL) -m 644 $(LIBFLAC_SHARED_FULLNAME) $(TOPDIR)lib
	$(INSTALL) -m 644 $(LIBFLAC_A) $(TOPDIR)lib
	$(LN) -sf $(LIBFLAC_SHARED_FULLNAME) $(LIBFLAC_SHARED)
	$(LN) -sf $(LIBFLAC_SHARED_FULLNAME) $(LIBFLAC_SHARED).$(MAJOR_VERSION)


