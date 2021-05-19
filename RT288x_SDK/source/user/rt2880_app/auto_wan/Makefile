all: $(EXEC)

romfs:
ifeq ($(CONFIG_DEFAULTS_RALINK_MT7620), y)
	ln -sf auto_wan_pt_mt7620 auto_wan_pt
else
	ln -sf auto_wan_pt_n_mt7620 auto_wan_pt
endif


	$(ROMFSINST) /bin/auto_wan_pt







