ra_dir_$(CONFIG_RALINKAPP_REG_RW)	+= reg
ra_dir_$(CONFIG_RALINKAPP_ATED)		+= ated
ra_dir_$(CONFIG_RALINKAPP_CSR)		+= csr_test
ra_dir_$(CONFIG_RALINKAPP_FLASH)	+= flash
ra_dir_$(CONFIG_RALINKAPP_HWNAT)	+= hw_nat
ra_dir_$(CONFIG_RALINKAPP_MII_MGR)	+= mii_mgr
ra_dir_$(CONFIG_RALINKAPP_NVRAM)	+= nvram
ra_dir_$(CONFIG_RALINKAPP_GPIO)		+= gpio
ra_dir_$(CONFIG_RALINKAPP_SPI)		+= spicmd
ra_dir_$(CONFIG_RALINKAPP_I2C)		+= i2ccmd
ra_dir_$(CONFIG_RALINKAPP_SCRIPTS)	+= scripts
ra_dir_$(CONFIG_RALINKAPP_QDMA)         += qdma 
ra_dir_$(CONFIG_RALINKAPP_MEMORY_USAGE)	+= mem_usage
ra_dir_$(CONFIG_RALINKAPP_SWITCH)	+= switch
ra_dir_$(CONFIG_RALINKAPP_I2S)		+= i2scmd
ra_dir_$(CONFIG_RALINKAPP_SPDIF)        += spdifcmd
ra_dir_$(CONFIG_RALINKAPP_PCM)		+= pcmcmd
ra_dir_$(CONFIG_RALINKAPP_PCIE_PHY)	+= pcie_phy
ra_dir_$(CONFIG_RALINKAPP_CLS)		+= clscmd
ra_dir_$(CONFIG_RALINKAPP_SUPERDMZ)	+= super_dmz
ra_dir_$(CONFIG_RALINKAPP_SWQOS)	+= qos
ra_dir_$(CONFIG_RALINKAPP_HWQOS)	+= qos
ra_dir_$(CONFIG_RALINKAPP_BUSARB)	+= busarb
ra_dir_$(CONFIG_RALINKAPP_ETHPHY)	+= ethphy
ra_dir_$(CONFIG_RALINKAPP_KPROFILE)	+= kprofile
ra_dir_$(CONFIG_USER_NFCSD)		+= nfc
ra_dir_$(CONFIG_USER_NFCHOD)		+= handover
ra_dir_$(CONFIG_RALINKAPP_LONG_LOOP)    += long_loop

all romfs:
	for i in $(ra_dir_y) ; do make -C $$i $@ || exit $?; done

clean:
	for i in $(ra_dir_y) ; do make -C $$i clean ; done

