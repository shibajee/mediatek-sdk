all: $(EXEC)

romfs:
ifeq ($(CONFIG_DEFAULTS_RALINK_MT7621), y)
ifeq ($(CONFIG_RALINKAPP_HWQOS), y)
	ln -sf qos_run_hw_mt7621 qos_run
else
ifeq ($(CONFIG_RALINKAPP_SWQOS), y)
	ln -sf qos_run_sw_mt7621 qos_run
endif
endif
endif
ifeq ($(CONFIG_DEFAULTS_RALINK_MT7620), y)
ifeq ($(CONFIG_RALINKAPP_SWQOS), y)
	ln -sf qos_run_sw_mt7620 qos_run
endif
endif

ifeq ($(CONFIG_DEFAULTS_RALINK_MT7628), y)
ifeq ($(CONFIG_RALINKAPP_SWQOS), y)
	ln -sf qos_run_sw_mt7628 qos_run
endif
endif

	$(ROMFSINST) /bin/qos_run