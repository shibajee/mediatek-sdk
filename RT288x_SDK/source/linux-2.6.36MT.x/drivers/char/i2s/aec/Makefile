KBUILD_CFLAGS += -I$(srctree)

obj-$(CONFIG_I2S_WITH_AEC) += aec.o
aec-objs := aec_api.o aec_util.o aec_lib/lec_lib.a aec_lib/bli_lib.a

ifeq ($(CONFIG_RALINK_I2S),y)
EXTRA_CFLAGS   +=-DCONFIG_RALINK_I2S
endif

clean:
	rm -rf ./*.mod.* ./*.ko ./*.o ./*.bak ./.*.cmd
	rm -fr modules.order
	rm -ff modules.builtin
