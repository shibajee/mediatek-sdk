TOPDIR=../
include $(TOPDIR)Rules.mak

ifeq ($(CONFIG_ARCH_MT8127),y)
CONFOPTS= --host=arm-linux
else
CONFOPTS= --host=mipsel-linux
endif

LIB_LDFLAGS := $(subst -z defs,,$(LDFLAGS))


# gmp
LIBGMP_A= libgmp.a
LIBGMP_SHARED=libgmp.so
LIBGMP_SHARED_FULLNAME=libgmp-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so


all: Makefile
	make -f Makefile all
	cp .libs/$(LIBGMP_A) .

Makefile:
	AR=$(CROSS_COMPILE)ar AS=$(CROSS_COMPILE)as CC=$(CROSS_COMPILE)gcc LD=$(CROSS_COMPILE)ld OBJCOPY=$(CROSS_COMPILE)objcopy RANLIB=$(CROSS_COMPILE)ranlib STRIP=$(CROSS_COMPILE)strip ./configure $(CONFOPTS)

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
	$(LD) $(LIB_LDFLAGS) -soname=$(LIBGMP_SHARED).$(MAJOR_VERSION) \
         -o $(LIBGMP_SHARED_FULLNAME) --whole-archive $(LIBGMP_A) \
         --no-whole-archive \
         -L$(TOPDIR)lib -lc $(LDADD_LIBFLOAT) $(LIBGCC);

	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(LIBGMP_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBGMP_SHARED).$(MAJOR_VERSION)
	$(INSTALL) -m 644 $(LIBGMP_SHARED_FULLNAME) $(TOPDIR)lib
	$(LN) -sf $(LIBGMP_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBGMP_SHARED)
	$(LN) -sf $(LIBGMP_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBGMP_SHARED).$(MAJOR_VERSION)
