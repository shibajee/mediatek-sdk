all:

romfs:

	$(ROMFSINST) -e CONFIG_ARM arm/start_endpoint /bin/start_endpoint
	$(ROMFSINST) -e CONFIG_ARM arm/endpoint /bin/endpoint
	$(ROMFSINST) -e CONFIG_ARM arm/echr.msg /bin/echr.msg
	$(ROMFSINST) -e CONFIG_ARM arm/endpoint.ini /bin/endpoint.ini
	
	$(ROMFSINST) -e CONFIG_MIPS mips/start_endpoint /bin/start_endpoint
	$(ROMFSINST) -e CONFIG_MIPS mips/endpoint /bin/endpoint
	$(ROMFSINST) -e CONFIG_MIPS mips/echr.msg /bin/echr.msg
	$(ROMFSINST) -e CONFIG_MIPS mips/endpoint.ini /bin/endpoint.ini
