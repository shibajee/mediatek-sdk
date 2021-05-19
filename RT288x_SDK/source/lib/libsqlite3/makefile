TOPDIR=../
include $(TOPDIR)Rules.mak

CONFOPTS= --host=mipsel-linux

LIB_LDFLAGS := $(subst -z defs,,$(LDFLAGS))


# upnp
LIBSQLITE3_A= .libs/libsqlite3.a
LIBSQLITE3_SHARED=libsqlite3.so
LIBSQLITE3_SHARED_FULLNAME=libsqlite3-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so

all: Makefile
	make -f Makefile all

Makefile:
	AR= AS= CC=$(CROSS_COMPILE)gcc CFLAGS=-fPIC CROSS= CROSS_COMPILE= CXX= HOSTCC= LD=$(CROSS_COMPILE)ld OBJCOPY= RANLIB= STRIP= GCC_DIR= ./configure $(CONFOPTS)

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
	$(LD) $(LIB_LDFLAGS) -soname=$(LIBSQLITE3_SHARED).$(MAJOR_VERSION) \
           -o $(LIBSQLITE3_SHARED_FULLNAME) --whole-archive $(LIBSQLITE3_A) \
           --no-whole-archive \
           -L$(TOPDIR)lib -lc $(LDADD_LIBFLOAT) $(LIBGCC);

	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(LIBSQLITE3_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBSQLITE3_SHARED).$(MAJOR_VERSION)
	$(INSTALL) -m 644 $(LIBSQLITE3_SHARED_FULLNAME) $(TOPDIR)lib
	$(INSTALL) -m 644 $(LIBSQLITE3_A) $(TOPDIR)lib
	$(LN) -sf $(LIBSQLITE3_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBSQLITE3_SHARED)
	$(LN) -sf $(LIBSQLITE3_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBSQLITE3_SHARED).$(MAJOR_VERSION)


