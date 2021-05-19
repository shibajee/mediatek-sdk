EXEC = switch
ifeq ($(CONFIG_P5_RGMII_TO_MT7530_MODE),y)
EXEC += switch_7530 
endif
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/net/raeth
CONF_H  = $(ROOTDIR)/$(LINUXDIR)/include/linux/autoconf.h

ifeq ($(CONFIG_RALINK_RT6855),y)
SRC=switch_gsw.c
else
ifeq ($(CONFIG_RALINK_MT7620),y)
SRC=switch_gsw.c
else
ifeq ($(CONFIG_RALINK_MT7621),y)
SRC=switch_gsw.c
else
ifeq ($(CONFIG_RALINK_RT6855A),y)
SRC=switch_gsw.c
else
ifeq ($(CONFIG_ARCH_MT8590),y)
SRC=switch_gsw.c
else
SRC=switch.c
endif
endif
endif
endif
endif

all: $(EXEC)
ifeq ($(CONFIG_P5_RGMII_TO_MT7530_MODE),y)
switch_7530: $(SRC) $(CONF_H)
	$(CC) $(CFLAGS) -DSWITCH_7530 $(LDFLAGS) -o $@ $(SRC) $(LDLIBS)
endif

switch: $(SRC) $(CONF_H)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(SRC) $(LDLIBS)

romfs:
	$(ROMFSINST) /bin/switch
ifeq ($(CONFIG_P5_RGMII_TO_MT7530_MODE),y)
	$(ROMFSINST) /bin/switch_7530
endif

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o

