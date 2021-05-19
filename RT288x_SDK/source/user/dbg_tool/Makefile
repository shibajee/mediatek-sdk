all:

romfs:
	$(ROMFSINST) -e CONFIG_ARM arm/iperf /bin/iperf
	$(ROMFSINST) -e CONFIG_ARM arm/strace /bin/strace
	$(ROMFSINST) -e CONFIG_ARM arm/tcpdump /bin/tcpdump
	
	$(ROMFSINST) -e CONFIG_MIPS mips/gdb /bin/gdb
	$(ROMFSINST) -e CONFIG_MIPS mips/iperf /bin/iperf
	$(ROMFSINST) -e CONFIG_MIPS mips/strace /bin/strace
	$(ROMFSINST) -e CONFIG_MIPS mips/libncurses.so.5 /lib/libncurses.so.5
