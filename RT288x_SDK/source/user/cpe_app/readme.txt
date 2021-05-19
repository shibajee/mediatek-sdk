#
# VoIP and Remote Management
# Release Note
#

==============================================================================
[NOTE] 2014.08.27 SPI GPIO driver release for MT7621
-Common
1. sync cpe sdk //Pach/TRUNK/Pach/mcu CL#107878
2. SPI GPIO feature-in for VoIP, bsp.ko
	- 2 ways to assign the SPI GPIO, and the module parameters will replace the define
 	    [v, library release][module parameters specified SPI GPIO]
		- insmod /lib/modules/*/kernel/drivers/syss.ko
		- insmod /lib/modules/*/kernel/drivers/misc/bsp.ko SLIC_RESET_GPIO=0 SPI_SCLK_GPIO=4 SPI_CS_GPIO=5 SPI_SDO_GPIO=3 SPI_SDI_GPIO=6

	    [x, source release][no parameter when insert bsp, the GPIO defined on hal_spi_mt7621.c(MT7621)]
		40 #if defined(GPIO_SPI)
		41 /* default spi gpio assignment */
		42 int SLIC_RESET_GPIO=0;
		43 int SPI_SCLK_GPIO=4;
		44 int SPI_CS_GPIO=5;
		45 int SPI_SDO_GPIO=3;
		46 int SPI_SDI_GPIO=6;
		47 #endif	 
		- insmod /lib/modules/*/kernel/drivers/syss.ko
		- insmod /lib/modules/*/kernel/drivers/misc/bsp.ko

==============================================================================
[NOTE] 2014.03.05 MTK VoIP and TR069 ready to use on MT7620
-Common
1. deleted CPE SDK Tree on APSoc version control system
2. by CPE SDK and APSoc Patch Tarball instead
3. sync cpe sdk CL#93744

-VoIP
1. (done) Register/MT/MO call test

-TR069
1. (done) initial device connection request test with ACS test Server

==============================================================================
[NOTE] 2014.03.04 
- VoIP and TR069 Environment sync.
1. 8M/64M (use 8M/32M profile) toolchain 463
2. nvram extern 1000
3. spi driver bug fixed for spi flash can not write
4. linux kernel patch for the symbol export, voip needed
5. mknod for voip vdsp(245) and slic(225) on internet.sh
6. add MT7620 default config for voip and tr069

==============================================================================
[NOTE]
- Common
1. toolchain should be use v463 with -mdsp -mdspr2 support
2. configuration it needs to extend the NVRAM size, MAX_CACHE_ENTRY( ex. 1000)
3. busybox, it needs "ln" utility

- VoIP
1. kernel it needs to let
	1) <M> Ralink RT2880 SPI Support
		[*]   SLIC CS Pin Connects to SPI CS1 
	2) <M> Ralink PCM Support
		(1)   PCM use SPI CH (NEW)
		(0)   PCM SLIC Reset pin map to GPIO Number (NEW)
		(2)   PCM Total Channel Number Supported (NEW)
		(117) PCM Clock Dividor Interger Part Value (NEW)
		(48)  PCM Clock Dividor Fraction Part Value (NEW)
		(3)   PCM slot mode (NEW)
	3) <M> Ralink GDMA Support 

2. busybox
	1) it needs "modprobe" module probe
	2) it needs "cut" tool
	3) it needs "find" tool
	4) it needs "ln" tool

