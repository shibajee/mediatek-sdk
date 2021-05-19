#
SHELL:=/bin/bash
#
# environment information
CPE_APP_DIR=$(shell pwd)
CPE_APP_ROOTFS_DIR=$(CPE_APP_DIR)/out/rootfs
CPE_APP_CFG_DIR=$(CPE_APP_ROOTFS_DIR)/usr/cfg

CPE_APP_CONFIG_DIR=$(CPE_APP_DIR)/platform

ifeq ($(CONFIG_PRODUCT), MT7620)
CPE_APP_CONFIG_FN=mt7620_cpe.config
CPE_APP_CONFIG=$(CPE_APP_CONFIG_DIR)/$(CPE_APP_CONFIG_FN)
CFG_PLATFORM=`tr '[A-Z]' '[a-z]' <<<\""$(CONFIG_PRODUCT)\""`
#CPE_APP_PRJ_FILE=MT7620_CPE_APSOC_INT.prj
CPE_APP_PRJ_TYPE=mt7620_cpe
else ifeq ($(CONFIG_PRODUCT), MT7621)
CPE_APP_CONFIG_FN=mt7621_cpe.config
CPE_APP_CONFIG=$(CPE_APP_CONFIG_DIR)/$(CPE_APP_CONFIG_FN)
CFG_PLATFORM=`tr '[A-Z]' '[a-z]' <<<\""$(CONFIG_PRODUCT)\""`
#CPE_APP_PRJ_FILE=MT7620_CPE_APSOC_INT.prj
CPE_APP_PRJ_TYPE=mt7621_cpe
else
CPE_APP_CONFIG_FN=unknown_cpe.config
CPE_APP_PRJ_TYPE=unknown_cpe
endif

CPE_APP_BUILD_TOOL=./build.pl
CPE_SDK_BUILD_CONFIG=build.config

# TODO: replace by the CONFIG_CROSS_COMPILER_PATH
#CFG_BUILD_PATH_DIR="/opt/buildroot-gcc463/usr/bin"
CFG_BUILD_PATH_DIR=$(CONFIG_CROSS_COMPILER_PATH)

CFG_BUILD_PATH=`sed -e "s/\/usr\/bin//g" <<< $(CFG_BUILD_PATH_DIR)`
CFG_CONFIG_CROSS_COMPILER_PATH=$(CFG_BUILD_PATH)/usr/bin
CFG_BUILD_PATH_SDK=$(shell sed -e 's/\//\\\\\\\\\//g' <<< "/opt/buildroot-gcc463")
APSOC_CONF_DIR=$(CPE_APP_DIR)/../../vendors/$(CONFIG_VENDOR)/$(CONFIG_PRODUCT)
APSOC_CONF_FILE1=$(APSOC_CONF_DIR)/RT2860_default_vlan
APSOC_CONF_FILE2=$(APSOC_CONF_DIR)/RT2860_default_novlan
APSOC_ROMFS_DIR=$(ROMFSDIR)
LINUX_VER=$(shell sed -e 's/linux-//g' -e 's/.x//g' <<< $(LINUXDIR))

CFG_REPACK_FORCE=0
CFG_APPLY_ALL=1
CFG_PACK_LOCK=cfg_pack_lock
RCS_PACK_LOCK=rcs_pack_lock
V=0

# SDK
SDK_FN=$(shell ls | grep SDK.$(CONFIG_PRODUCT)*.tar.gz)
PATCH_FN=$(shell ls | grep apsoc_patch-*.tar.gz)

# tool
#CPE_APPINST=$(ROMFSINST)
CPE_APPINST=cp -a

# common
ETC_INIT_D_INST=
ETC_CRON_INST=
USR_SBIN_INST=openssl stun
USR_LIB_INST=
USR_TRANS_INST=
USR_VOIP_INST=
BIN_INST=
SBIN_INST=sncfg
MNT_INST=
#MNT_CONF_INST=openssl.cnf
MNT_CONF_INST=
KERMEL_DRIVERS_MISC_INST=
KERMEL_DRIVERS_NET_INST=
KERMEL_DRIVERS_INST=

LIB_INST=\
		libcrypto.so \
		libcrypto.so.1.0.1 \
		libssl.so libssl.so.1.0.1 \
		librt-0.9.33.2.so \
		librt.so.0 libopenssl.so
LIB_INST+=\
		libsncfg.so

#LIB_INST+=\
		libpthread-0.9.33.2.so \
		libcrypt.so.0 \
		libcrypt-0.9.33.2.so \
		libdl-0.9.33.2.so \
		libc.so.0 

#LIB_INST+=\
		ld-uClibc.so.0 \
		ld-uClibc-0.9.33.2.so \
		libuClibc-0.9.33.2.so

#LIB_INST+=\
		libz.so.1 \
		libnsl.so.0 \
		libm-0.9.33.2.so \
		libnsl-0.9.33.2.so \
		libz.so.1.2.8 \
		libresolv-0.9.33.2.so

ifeq ($(strip $(CONFIG_MTK_VOIP)), y)
ETC_INIT_D_INST+=sc_auto_switch.sh sc_auto_prov.sh sc
ETC_CRON_INST+=sc.cron
USR_SBIN_INST+=sipclient
USR_LIB_INST+=
USR_TRANS_INST+=sc.trans
USR_VOIP_INST+=voip-x-default.ini
BIN_INST+=voconf_main.elf
SBIN_INST+=
MNT_INST+=
MNT_CONF_INST+=
LIB_INST+=
KERMEL_DRIVERS_MISC_INST+=fxs3.ko foip.ko ovdsp.ko lec.ko drc.ko slic3.ko acodec.ko ortp.ko pcm.ko bsp.ko
KERMEL_DRIVERS_NET_INST+=ksocket.ko
KERMEL_DRIVERS_INST+=syss.ko
endif

ifeq ($(strip $(CONFIG_MTK_TR069)), y)
ETC_INIT_D_INST+=cwmpd tr_parser_stats.sh
ETC_CRON_INST+=
USR_SBIN_INST+=
USR_LIB_INST+=
USR_TRANS_INST+=
USR_VOIP_INST+=
BIN_INST+=
SBIN_INST+=tr_cert_update.sh dping autoreboot.sh cwmpapp.elf cwmpd.elf
#MNT_INST+=cwmp_client.pem cwmp_cacert.pem
MNT_INST+=
MNT_CONF_INST+=
LIB_INST+=libtr_parser_mtk.so
KERMEL_DRIVERS_MISC_INST+=
KERMEL_DRIVERS_NET_INST+=
KERMEL_DRIVERS_INST+=
endif

ifeq ($(strip $(CONFIG_MTK_OMA)), y)
ETC_INIT_D_INST+=oma_dm oma_dm_boot
ETC_CRON_INST+=
USR_SBIN_INST+=
USR_LIB_INST+=
USR_TRANS_INST+=oma_dm.trans
USR_VOIP_INST+=
BIN_INST+=
SBIN_INST+=dmd.elf dmapp.elf
MNT_INST+=
MNT_CONF_INST+=
LIB_INST+=libdm_mo.so liboma_dm.so
KERMEL_DRIVERS_MISC_INST+=
KERMEL_DRIVERS_NET_INST+=
KERMEL_DRIVERS_INST+=
endif

.PHONY: sdk_check patch_check env_var_list gen_cpe_app_config all_ex

# romfs_ex: dynamic gen voip and tr069 config for 4 line support, but nvram size is not enough, shirk to 1 line support
#	    TODO- for extend	
all: sdk_check patch_check env_var_list gen_cpe_app_config gen_build_tool gen_apsoc_build_config all_ex

sdk_check:
	@echo CPE_APP_PRJ_TYPE=$(CPE_APP_PRJ_TYPE)
	@echo CPE_APP_CONFIG_FN=$(CPE_APP_CONFIG_FN)
	@echo SDK_FN=$(SDK_FN)
	@bash -c '\
		if [ ! -f ./lock_cpe_sdk ] ; then \
			if [ ! $(SDK_FN) == "" ] ; then \
				echo "untar $(SDK_FN)...";\
				tar vxzf $(SDK_FN) ;\
				cp -af SDK.$(COFIG_PRODUCT)*/* . ;\
				ls | grep SDK.$(CONFIG_PRODUCT) > log.sdk.info ;\
				rm -rf SDK.$(CONFIG_PRODUCT)_CPE* ;\
				touch ./lock_cpe_sdk ;\
			else\
				echo "!!!SDK tarball not found!\n";\
				exit 1;\
			fi;\
		fi;'
patch_check:
	@bash -c '\
		if [ ! -f ./lock_apsoc_patch ] ; then \
			if [ ! $(PATCH_FN) == "" ] ; then \
				echo "untar $(PATCH_FN)...";\
				tar vxzf $(PATCH_FN) ;\
				cp -af apsoc_patch-*/* . ;\
				ls | grep apsoc_patch- > log.apsoc_patch.info ;\
				rm -rf apsoc_patch-* ;\
				touch ./lock_apsoc_patch ;\
			else\
				echo "!!!Patch tarball not found!\n";\
			fi;\
		fi;'

gen_build_tool:
	#@bash -c '\
	#	if [ ! -d $(CPE_APP_DIR)/work/install/bin ]; then \
	#		mkdir -p $(CPE_APP_DIR)/work/install/bin; \
	#		cp -a $(CPE_APP_DIR)/work/libbase/bin $(CPE_APP_DIR)/work/install ;\
	#		find $(CPE_APP_DIR)/work/install/bin/. -name "*.sh" -or -name "*.pl" | xargs chmod +x ;\
	#	fi;'
gen_apsoc_build_config:
	@bash -c '\
		cp -f $(CPE_SDK_BUILD_CONFIG) $(CPE_SDK_BUILD_CONFIG).ori;\
		sed -e 's/APSOC_INT=0/APSOC_INT=1/g' -e 's/NVRAM_WRAPPER=0/NVRAM_WRAPPER=1/g' < $(CPE_SDK_BUILD_CONFIG).ori > $(CPE_SDK_BUILD_CONFIG);\
		'

gen_cpe_app_config:
	@bash -c '\
		if [ -f $(CPE_APP_CONFIG) ]; then \
			mv -f $(CPE_APP_CONFIG) $(CPE_APP_CONFIG).ori ;\
		fi;'
	@bash -c '\
		echo "\$$PLATFORM="$(CFG_PLATFORM)";" > $(CPE_APP_CONFIG);\
		echo "\$$BUILD_PATH=""\""$(CFG_BUILD_PATH)"\";" >> $(CPE_APP_CONFIG);\
		echo "\$$CONFIG_CROSS_COMPILER_PATH=""\""$(CFG_CONFIG_CROSS_COMPILER_PATH)"\";" >> $(CPE_APP_CONFIG);\
		echo "\$$BUILD_PATH_SDK=""\""$(CFG_BUILD_PATH_SDK)"\";" >> $(CPE_APP_CONFIG);\
		echo "\$$ARCH=""\"mips""\";" >> $(CPE_APP_CONFIG);\
		echo "\$$CROSS_PREFIX=""\"mipsel-linux""\";" >> $(CPE_APP_CONFIG);\
		echo "\$$ASYNC_BUILD=""\"y""\";" >> $(CPE_APP_CONFIG);\
		echo "\$$ASYNC_SRC=""\"y""\";" >> $(CPE_APP_CONFIG);\
		echo "\$$BUILD_TYPE=""\"NFULL""\";" >> $(CPE_APP_CONFIG);\
		'	
env_var_list:
	# cpe_app
	@echo "CPE_APP_ROOTFS_DIR    =" $(CPE_APP_ROOTFS_DIR)
	@echo "APSOC_CONF_DIR        =" $(APSOC_CONF_DIR)
	@echo "APSOC_ROMFS_DIR       =" $(APSOC_ROMFS_DIR)
	@echo "CFG_BUILD_PATH_DIR    =" $(CFG_BUILD_PATH_DIR)
	@echo "CFG_BUILD_PATH        =" $(CFG_BUILD_PATH)
	# Makefile environment
	@echo "VENDOR                ="$(VENDOR)            
	@echo "PRODUCT               ="$(PRODUCT)           
	@echo "ROOTDIR               ="$(ROOTDIR)           
	@echo "LINUXDIR              ="$(LINUXDIR)          
	@echo "HOSTCC                ="$(HOSTCC)
	@echo "CONFIG_SHELL          ="$(CONFIG_SHELL)
	@echo "CONFIG_CONFIG         ="$(CONFIG_CONFIG)
	@echo "BUSYBOX_CONFIG        ="$(BUSYBOX_CONFIG)
	@echo "LINUX_CONFIG          ="$(LINUX_CONFIG)  
	@echo "MODULES_CONFIG        ="$(MODULES_CONFIG)
	@echo "ROMFSDIR              ="$(ROMFSDIR)      
	@echo "SCRIPTSDIR            ="$(SCRIPTSDIR)
	@echo "VERSIONPKG            ="$(VERSIONPKG)        
	@echo "VERSIONSTR            ="$(VERSIONSTR)        
	@echo "ROMFSINST             ="$(ROMFSINST)         
	@echo "PATH                  ="$(PATH)              
	@echo "IMAGEDIR              ="$(IMAGEDIR)          
	@echo "RELFILES              ="$(RELFILES)          
	@echo "TFTPDIR               ="$(TFTPDIR)           
	@echo "BUILD_START_STRING    ="$(BUILD_START_STRING)
	@echo "HOST_NCPU             ="$(HOST_NCPU)         
	@echo "CONFIG_PRODUCT        ="$(CONFIG_PRODUCT)  
	@echo "CONFIG_VENDOR         ="$(CONFIG_VENDOR)
	@echo "CONFIG_CROSS_COMPILER_PATH="$(CONFIG_CROSS_COMPILER_PATH)
	@echo "LINUX_VER"=$(LINUX_VER)
	@echo "KERNELRELEASE="$(KERNELRELEASE)
	@echo "CROSS_COMPILE="$(CROSS_COMPILE)

all_ex:
	# integrate
	#./build.pl mt7620_cpe MT7620_CPE_APSOC_INT.prj i
	#$(CPE_APP_BUILD_TOOL) $(CPE_APP_PRJ_TYPE) $(CPE_APP_PRJ_FILE) i
	./build.sdk $(CPE_SDK_BUILD_CONFIG) i 

pack_voip:
	# for the flash save, install seperate 


romfs_cpe_app_inst:
	# [etc/init.d]
	mkdir -p $(APSOC_ROMFS_DIR)/etc_ro/init.d
	for f in $(ETC_INIT_D_INST) ; do \
		[ "$V" ] && echo "files="$$f; \
		$(CPE_APPINST) $(CPE_APP_ROOTFS_DIR)/etc/init.d/$$f $(APSOC_ROMFS_DIR)/etc_ro/init.d ; \
	done	
	# [etc/cron]
	mkdir -p $(APSOC_ROMFS_DIR)/etc_ro/cron
	for f in $(ETC_CRON_INST) ; do \
		[ "$V" ] && echo "files="$$f; \
		$(CPE_APPINST) $(CPE_APP_ROOTFS_DIR)/etc/cron/$$f $(APSOC_ROMFS_DIR)/etc_ro/cron ; \
	done	
	# [usr/sbin]
	mkdir -p $(APSOC_ROMFS_DIR)/usr/sbin
	for f in $(USR_SBIN_INST) ; do \
		[ "$V" ] && echo "files="$$f; \
		$(CPE_APPINST) $(CPE_APP_ROOTFS_DIR)/usr/sbin/$$f $(APSOC_ROMFS_DIR)/usr/sbin ; \
	done	
	# [usr/lib]
	mkdir -p $(APSOC_ROMFS_DIR)/usr/lib
	for f in $(USR_LIB_INST) ; do \
		[ "$V" ] && echo "files="$$f; \
		$(CPE_APPINST) $(CPE_APP_ROOTFS_DIR)/usr/lib/$$f $(APSOC_ROMFS_DIR)/usr/lib ; \
	done	
	# [usr/trans]
	mkdir -p $(APSOC_ROMFS_DIR)/usr/trans
	for f in $(USR_TRANS_INST) ; do \
		[ "$V" ] && echo "files="$$f; \
		$(CPE_APPINST) $(CPE_APP_ROOTFS_DIR)/usr/trans/$$f $(APSOC_ROMFS_DIR)/usr/trans ; \
	done	
	# [usr/voip]
	mkdir -p $(APSOC_ROMFS_DIR)/usr/voip
	for f in $(USR_VOIP_INST) ; do \
		[ "$V" ] && echo "files="$$f; \
		$(CPE_APPINST) $(CPE_APP_ROOTFS_DIR)/usr/voip/$$f $(APSOC_ROMFS_DIR)/usr/voip ; \
	done	
	# [bin]
	mkdir -p $(APSOC_ROMFS_DIR)/bin
	for f in $(BIN_INST) ; do \
		[ "$V" ] && echo "files="$$f; \
		$(CPE_APPINST) $(CPE_APP_ROOTFS_DIR)/bin/$$f $(APSOC_ROMFS_DIR)/bin ; \
	done	
	# [sbin]
	mkdir -p $(APSOC_ROMFS_DIR)/sbin
	for f in $(SBIN_INST) ; do \
		[ "$V" ] && echo "files="$$f; \
		$(CPE_APPINST) $(CPE_APP_ROOTFS_DIR)/sbin/$$f $(APSOC_ROMFS_DIR)/sbin ; \
	done	
	# [mnt]
	mkdir -p $(APSOC_ROMFS_DIR)/mnt
	for f in $(MNT_INST) ; do \
		[ "$V" ] && echo "files="$$f; \
		$(CPE_APPINST) $(CPE_APP_ROOTFS_DIR)/mnt/$$f $(APSOC_ROMFS_DIR)/mnt ; \
	done	
	# [mnt/conf]
	mkdir -p $(APSOC_ROMFS_DIR)/mnt/conf
	for f in $(MNT_CONF_INST) ; do \
		[ "$V" ] && echo "files="$$f; \
		$(CPE_APPINST) $(CPE_APP_ROOTFS_DIR)/mnt/conf/$$f $(APSOC_ROMFS_DIR)/mnt/conf ; \
	done	
	# [lib]
	mkdir -p $(APSOC_ROMFS_DIR)/lib
	for f in $(LIB_INST) ; do \
		[ "$V" ] && echo "files="$$f; \
		$(CPE_APPINST) $(CPE_APP_ROOTFS_DIR)/lib/$$f $(APSOC_ROMFS_DIR)/lib ; \
	done	
	# [lib/modules/$(LINUX_VER)/kernel/drivers/misc]
	mkdir -p $(APSOC_ROMFS_DIR)/lib/modules/$(LINUX_VER)/kernel/drivers/misc
	for f in $(KERMEL_DRIVERS_MISC_INST) ; do \
		[ "$V" ] && echo "files="$$f; \
		$(CPE_APPINST) $(CPE_APP_ROOTFS_DIR)/lib/modules/*/kernel/drivers/misc/$$f $(APSOC_ROMFS_DIR)/lib/modules/$(LINUX_VER)/kernel/drivers/misc ; \
	done	
	# [lib/modules/$(LINUX_VER)/kernel/drivers/net]
	mkdir -p $(APSOC_ROMFS_DIR)/lib/modules/$(LINUX_VER)/kernel/drivers/net
	for f in $(KERMEL_DRIVERS_NET_INST) ; do \
		[ "$V" ] && echo "files="$$f; \
		$(CPE_APPINST) $(CPE_APP_ROOTFS_DIR)/lib/modules/*/kernel/drivers/net/$$f $(APSOC_ROMFS_DIR)/lib/modules/$(LINUX_VER)/kernel/drivers/net ; \
	done	
	# [lib/modules/$(LINUX_VER)/kernel/drivers]
	mkdir -p $(APSOC_ROMFS_DIR)/lib/modules/$(LINUX_VER)/kernel/drivers
	for f in $(KERMEL_DRIVERS_INST) ; do \
		[ "$V" ] && echo "files="$$f; \
		$(CPE_APPINST) $(CPE_APP_ROOTFS_DIR)/lib/modules/*/kernel/drivers/$$f $(APSOC_ROMFS_DIR)/lib/modules/$(LINUX_VER)/kernel/drivers ; \
	done	

romfs_ex:
	# pack configuration
	@bash -c '\
		if [ "$(CFG_REPACK_FORCE)" == "1" ] ; then \
			echo "remove $(APSOC_CONF_DIR)/$(CFG_PACK_LOCK)"; \
			rm -rf $(APSOC_CONF_DIR)/$(CFG_PACK_LOCK); \
		else\
			echo CFG_REPACK_FORCE=$(CFG_REPACK_FORCE); \
		fi;\
		'
	# check double pack
	@bash -c '\
		if [ ! -f $(APSOC_CONF_DIR)/$(CFG_PACK_LOCK) ]; then \
			if [ "$(CFG_APPLY_ALL)" == "1" ] ; then \
				echo "Apply All Configuration..."; \
				cat $(CPE_APP_CFG_DIR)/* >> $(APSOC_CONF_FILE1);	\
				cat $(CPE_APP_CFG_DIR)/* >> $(APSOC_CONF_FILE2);	\
			else \
				if [ "$(CONFIG_MTK_VOIP)" == "y" ] ; then \
					cat $(CPE_APP_CFG_DIR)/sc_acct.cfg >> $(APSOC_CONF_FILE1);	\
					cat $(CPE_APP_CFG_DIR)/sc_ftr.cfg >> $(APSOC_CONF_FILE1);	\
					cat $(CPE_APP_CFG_DIR)/sc_line.cfg >> $(APSOC_CONF_FILE1);	\
					cat $(CPE_APP_CFG_DIR)/sc_prv.cfg >> $(APSOC_CONF_FILE1);	\
					cat $(CPE_APP_CFG_DIR)/sc_sys.cfg >> $(APSOC_CONF_FILE1);	\
					cat $(CPE_APP_CFG_DIR)/sc_acct.cfg >> $(APSOC_CONF_FILE2);	\
					cat $(CPE_APP_CFG_DIR)/sc_ftr.cfg >> $(APSOC_CONF_FILE2);	\
					cat $(CPE_APP_CFG_DIR)/sc_line.cfg >> $(APSOC_CONF_FILE2);	\
					cat $(CPE_APP_CFG_DIR)/sc_prv.cfg >> $(APSOC_CONF_FILE2);	\
					cat $(CPE_APP_CFG_DIR)/sc_sys.cfg >> $(APSOC_CONF_FILE2);	\
					echo "CONFIG_MTK_VOIP="$(CONFIG_MTK_VOIP);\
				fi;\
				if [ "$(CONFIG_MTK_TR069)" == "y" ] ; then \
					echo "CONFIG_MTK_TR069="$(CONFIG_MTK_TR069);\
				fi;\
			fi;\
			touch $(APSOC_CONF_DIR)/$(CFG_PACK_LOCK);	\
		else \
			echo "file exist: $(APSOC_CONF_DIR)/$(CFG_PACK_LOCK)"; \
		fi;'

romfs: 

romfs_rcs_pack:
	# check double pack
	@bash -c '\
		if [ ! -f $(APSOC_CONF_DIR)/$(RCS_PACK_LOCK) ]; then \
			if [ -f $(APSOC_CONF_DIR)/rcS ] ; then \
				echo "# dynamic add by cpe_app" >> $(APSOC_CONF_DIR)/rcS ;\
				echo "ln -s /etc_ro/init.d /etc/init.d" >> $(APSOC_CONF_DIR)/rcS ;\
				echo "ln -s /etc_ro/cron /etc/cron" >> $(APSOC_CONF_DIR)/rcS ;\
				echo "" >> $(APSOC_CONF_DIR)/rcS ;\
				touch $(APSOC_CONF_DIR)/$(RCS_PACK_LOCK);	\
			else \
				echo "file not found: $(APSOC_CONF_DIR)/rcS"; \
			fi; \
		else \
			echo "file exist: $(APSOC_CONF_DIR)/$(RCS_PACK_LOCK)"; \
		fi;'

romfs.post: romfs_cpe_app_inst
	#find $(APSOC_ROMFS_DIR) -name "*.ko"
	#cat $(APSOC_CONF_DIR)/rcS

clean:
	#./build.pl mt7620_cpe MT7620_CPE_APSOC_INT.prj c
	#$(CPE_APP_BUILD_TOOL) $(CPE_APP_PRJ_TYPE) $(CPE_APP_PRJ_FILE) c
	@rm -rf $(CPE_APP_DIR)/out

cpe_app_clean:
	@rm -rf build*
	@rm -rf log.*
	@rm -rf lock*
	@rm -rf config*
	@rm -rf tool*
	@rm -rf out*
	@rm -rf work
	@rm -rf platform





