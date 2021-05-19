
# -*- sh -*-

VERSION = 1.5.0
V = 1
#CC = /opt/buildroot-gcc342/bin/mipsel-linux-gcc

#MTK/ralink

CFLAGS = -I../../lib/lzo-2.03/include/ -I../../lib/e2fsprogs-1.41.3/lib/ -I../../lib/zlib-1.2.3/

LDFLAGS = -L../../lib/lzo-2.03/src/.libs/ -L../../lib/zlib-1.2.3 -L../../lib/e2fsprogs-1.41.3/lib/uuid/

WITHOUT_XATTR = 1
#end

CPPFLAGS += -I./include -I$(BUILDDIR)/include -I./ubi-utils/include $(ZLIBCPPFLAGS) $(LZOCPPFLAGS)


ifeq ($(WITHOUT_XATTR), 1)
  CPPFLAGS += -DWITHOUT_XATTR
endif
ifeq ($(WITHOUT_LZO), 1)
  CPPFLAGS += -DWITHOUT_LZO
else
  LZOLDLIBS = -llzo2
endif

TESTS = tests
#remove serve_image & recv_image from MTD_BINS
MTD_BINS = \
	ftl_format flash_erase nanddump doc_loadbios \
	ftl_check mkfs.jffs2 flash_lock flash_unlock \
	flash_otp_info flash_otp_dump mtd_debug flashcp nandwrite nandtest \
	jffs2dump \
	nftldump nftl_format docfdisk \
	rfddump rfdformat \
	serve_image recv_image \
	sumtool jffs2reader
UBI_BINS = \
	ubiupdatevol ubimkvol ubirmvol ubicrc32 ubinfo ubiattach \
	ubidetach ubinize ubiformat ubirename mtdinfo ubirsvol

BINS = $(MTD_BINS)
BINS += mkfs.ubifs/mkfs.ubifs
BINS += $(addprefix ubi-utils/,$(UBI_BINS))
SCRIPTS = flash_eraseall

TARGETS = $(BINS)
TARGETS += lib/libmtd.a
TARGETS += ubi-utils/libubi.a

OBJDEPS = $(BUILDDIR)/include/version.h

include common.mk

CLEAN_FIND = find "$(BUILDDIR)/" -xdev '(' -name '*.[ao]' -o -name '.*.c.dep' ')'

clean::
ifneq ($(BUILDDIR)/.git,)
ifneq ($(BUILDDIR),.)
ifneq ($(BUILDDIR),$(CURDIR))
	rm -rf $(BUILDDIR)
endif
endif
endif
# findutils v4.1.x (RHEL 4) do not have '+' syntax
	@if test -d "$(BUILDDIR)/"; then \
		$(CLEAN_FIND) -exec rm -f {} + 2> /dev/null || \
		$(CLEAN_FIND) -exec rm -f {} \; ; \
	fi
	rm -f $(BUILDDIR)/include/version.h
	$(MAKE) -C $(TESTS) clean

install:: $(addprefix $(BUILDDIR)/,${BINS}) ${SCRIPTS}
	mkdir -p ${DESTDIR}/${SBINDIR}
	install -m 0755 $^ ${DESTDIR}/${SBINDIR}/
	mkdir -p ${DESTDIR}/${MANDIR}/man1
	install -m 0644 mkfs.jffs2.1 ${DESTDIR}/${MANDIR}/man1/
	-gzip -9f ${DESTDIR}/${MANDIR}/man1/*.1

tests::
	$(MAKE) -C $(TESTS)

cscope:
	cscope -bR

$(BUILDDIR)/include/version.h: $(BUILDDIR)/include/version.h.tmp
	$(call BECHO,CHK)
	$(Q)cmp -s $@ $@.tmp && rm -f $@.tmp || mv $@.tmp $@
$(BUILDDIR)/include/version.h.tmp:
	${Q}mkdir -p $(dir $@)
	$(Q)echo '#define VERSION "$(VERSION)"' > $@

#
# Utils in top level
#
obj-mkfs.jffs2 = compr_rtime.o compr_zlib.o compr_lzo.o compr.o rbtree.o
LDFLAGS_mkfs.jffs2 = $(ZLIBLDFLAGS) $(LZOLDFLAGS)
LDLIBS_mkfs.jffs2  = -lz $(LZOLDLIBS)

LDFLAGS_jffs2reader = $(ZLIBLDFLAGS) $(LZOLDFLAGS)
LDLIBS_jffs2reader  = -lz $(LZOLDLIBS)

$(foreach v,$(MTD_BINS),$(eval $(call mkdep,,$(v))))

#
# Common libmtd
#
obj-libmtd.a = libmtd.o libmtd_legacy.o libcrc32.o libfec.o
$(call _mkdep,lib/,libmtd.a)

#
# Utils in mkfs.ubifs subdir
#
obj-mkfs.ubifs = crc16.o lpt.o compr.o devtable.o \
	hashtable/hashtable.o hashtable/hashtable_itr.o
LDLIBS_mkfs.ubifs = -lz -llzo2 -lm -luuid
$(call mkdep,mkfs.ubifs/,mkfs.ubifs,,ubi-utils/libubi.a)

#
# Utils in ubi-utils/ subdir
#
obj-libiniparser.a = libiniparser.o dictionary.o
obj-libscan.a      = libscan.o
obj-libubi.a       = libubi.o
obj-libubigen.a    = libubigen.o

obj-mtdinfo   = libubigen.a
obj-ubinize   = libubigen.a libiniparser.a
obj-ubiformat = libubigen.a libscan.a

$(foreach v,libubi.a libubigen.a libiniparser.a libscan.a,$(eval $(call _mkdep,ubi-utils/,$(v))))
$(foreach v,$(UBI_BINS),$(eval $(call mkdep,ubi-utils/,$(v),libubi.a ubiutils-common.o)))

romfs:
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_DOCFDISK          /bin/docfdisk
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_DOC_LOADBIOS      /bin/doc_loadbios
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_FLASHCP           /bin/flashcp
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_FLASH_ERASE       /bin/flash_erase
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_FLASH_ERASEALL    /bin/flash_eraseall
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_FLASH_LOCK        /bin/flash_lock
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_FLASH_OTP_DUMP    /bin/flash_otp_dump
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_FLASH_OTP_INFO    /bin/flash_otp_info
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_FLASH_UNLOCK      /bin/flash_unlock
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_FTL_CHECK         /bin/ftl_check
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_FTL_FORMAT        /bin/ftl_format
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_JFFS2DUMP         /bin/jffs2dump
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_JFFS2READER       /bin/jffs2reader
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_MKFSJFFS2         /bin/mkfs.jffs2
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_MTD_DEBUG         /bin/mtd_debug
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_NANDDUMP          /bin/nanddump
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_NANDTEST          /bin/nandtest
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_NANDWRITE         /bin/nandwrite
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_NFTLDUMP          /bin/nftldump
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_NFTL_FORMAT       /bin/nftl_format
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_RECV_IMAGE        /bin/recv_image
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_RFDDUMP           /bin/rfddump
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_RFDFORMAT         /bin/rfdformat
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_SERVE_IMAGE       /bin/serve_image
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_SUMTOOL           /bin/sumtool
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_MKFSUBIFS         mkfs.ubifs/mkfs.ubifs /bin/mkfs.ubifs
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_MTDINFO           ubi-utils/mtdinfo /bin/mtdinfo
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_UBIATTACH         ubi-utils/ubiattach /bin/ubiattach
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_UBICRC32          ubi-utils/ubicrc32 /bin/ubicrc32
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_UBIDETACH         ubi-utils/ubidetach /bin/ubidetach
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_UBIFORMAT         ubi-utils/ubiformat /bin/ubiformat
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_UBIMKVOL          ubi-utils/ubimkvol /bin/ubimkvol
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_UBINFO            ubi-utils/ubinfo /bin/ubinfo
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_UBINIZE           ubi-utils/ubinize /bin/ubinize
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_UBIRENAME         ubi-utils/ubirename /bin/ubirename
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_UBIRMVOL          ubi-utils/ubirmvol /bin/ubirmvol
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_UBIRSVOL          ubi-utils/ubirsvol /bin/ubirsvol
	$(ROMFSINST) -e CONFIG_USER_MTDUTILS_UBIUPDATEVOL      ubi-utils/ubiupdatevol /bin/ubiupdatevol

