TOPDIR=../
include $(TOPDIR)Rules.mak

CONFOPTS= --host=mipsel-linux

LIB_LDFLAGS := $(subst -z defs,,$(LDFLAGS))


# upnp
LIBVORBIS_A= lib/.libs/libvorbis.a
LIBVORBIS_SHARED=lib/.libs/libvorbis.so
LIBVORBIS_SHARED_FULLNAME=lib/libvorbis-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so

all: Makefile
	make -f Makefile all

Makefile:
	AR= AS= CC=$(CROSS_COMPILE)gcc LDFLAGS=-L$(ROOTDIR)/lib/libogg/src/.libs/ CFLAGS=-fPIC CROSS= CROSS_COMPILE= CXX= HOSTCC= LD=$(CROSS_COMPILE)ld OBJCOPY= RANLIB= STRIP= GCC_DIR= ./configure $(CONFOPTS) --disable-oggtest --with-ogg=$(ROOTDIR)/lib/libogg/src/.libs/

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
	$(LD) $(LIB_LDFLAGS) -soname=$(LIBVORBIS_SHARED).$(MAJOR_VERSION) \
           -o $(LIBVORBIS_SHARED_FULLNAME) --whole-archive $(LIBVORBIS_A) \
           --no-whole-archive \
           -L$(TOPDIR)lib -lc $(LDADD_LIBFLOAT) $(LIBGCC);

	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(LIBVORBIS_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBVORBIS_SHARED).$(MAJOR_VERSION)
	$(INSTALL) -m 644 $(LIBVORBIS_SHARED_FULLNAME) $(TOPDIR)lib
	$(INSTALL) -m 644 $(LIBVORBIS_A) $(TOPDIR)lib
	$(LN) -sf $(LIBVORBIS_SHARED_FULLNAME) $(LIBVORBIS_SHARED)
	$(LN) -sf $(LIBVORBIS_SHARED_FULLNAME) $(LIBVORBIS_SHARED).$(MAJOR_VERSION)


