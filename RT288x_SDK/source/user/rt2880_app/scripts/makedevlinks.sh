#!/bin/sh

. /sbin/config.sh

mkdir /dev/pts
mount -t devpts devpts /dev/pts
mknod /dev/ram b 1 1
mknod /dev/ram0 b 1 0
mknod /dev/ram1 b 1 1
mknod /dev/ram2 b 1 2
mknod /dev/ram3 b 1 3

mknod /dev/mem c 1 1
mknod /dev/kmem c 1 2
mknod /dev/null c 1 3
mknod /dev/random c 1 8
mknod /dev/urandom c 1 9

mknod /dev/ptyp0 c 2 0
mknod /dev/ptyp1 c 2 1

mknod /dev/ttyp0 c 3 0
mknod /dev/ttyp1 c 3 1

mknod /dev/ptmx c 5 2

mknod /dev/ttyS0 c 4 64
mknod /dev/ttyS1 c 4 65
mknod /dev/console c 5 1

# for Option Icon 225 3G dongle
if [ "$CONFIG_USB_SERIAL_HSO" = "y" ] || [ "$CONFIG_USB_SERIAL_HSO" = "m" ]; then
	mknod /dev/ttyHS0 c 245 0
	mknod /dev/ttyHS1 c 245 1
	mknod /dev/ttyHS2 c 245 2
	mknod /dev/ttyHS3 c 245 3
fi

# for generic 3G dongle
if [ "$CONFIG_USB_SERIAL" = "y" ] || [ "$CONFIG_USB_SERIAL" = "m" ]; then
	mknod /dev/ttyUSB0 c 188 0
	mknod /dev/ttyUSB1 c 188 1
	mknod /dev/ttyUSB2 c 188 2
	mknod /dev/ttyUSB3 c 188 3
fi

# for BandLuxe 3G dongle
if [ "$CONFIG_BLK_DEV_SR" = "y" ] || [ "$CONFIG_BLK_DEV_SR" = "m" ]; then
mknod /dev/sr0 b 11 0
fi
if [ "$CONFIG_CHR_DEV_SG" = "y" ] || [ "$CONFIG_CHR_DEV_SG" = "m" ]; then
mknod /dev/sg0 c 21 0
fi

mknod /dev/mtdblock0 b 31 0
mknod /dev/mtdblock1 b 31 1
mknod /dev/mtdblock2 b 31 2
mknod /dev/mtdblock3 b 31 3
mknod /dev/mtdblock4 b 31 4
mknod /dev/mtdblock5 b 31 5
mknod /dev/mtdblock6 b 31 6
mknod /dev/mtdblock7 b 31 7
mknod /dev/mtd0 c 90 0
mknod /dev/mtd0ro c 90 1
mknod /dev/mtd1 c 90 2
mknod /dev/mtd1ro c 90 3
mknod /dev/mtd2 c 90 4
mknod /dev/mtd2ro c 90 5
mknod /dev/mtd3 c 90 6
mknod /dev/mtd3ro c 90 7
mknod /dev/mtd4 c 90 8
mknod /dev/mtd4ro c 90 9
mknod /dev/mtd5 c 90 10
mknod /dev/mtd5ro c 90 11
mknod /dev/mtd6 c 90 12
mknod /dev/mtd6ro c 90 13
mknod /dev/mtd7 c 90 14
mknod /dev/mtd7ro c 90 15

mknod /dev/video0 c 81 0
mknod /dev/ppp c 108 0
mknod /dev/pts/0 c 136 0
mknod /dev/pts/1 c 136 1
mknod /dev/pts/2 c 136 2
mknod /dev/pts/3 c 136 3
mknod /dev/spiS0 c 217 0
mknod /dev/i2cM0 c 218 0

mknod /dev/flash0 c 200	0
mknod /dev/swnat0 c 210	0
mknod /dev/hwnat0 c 220	0
mknod /dev/acl0 c 230 0
mknod /dev/ac0 c 240 0
mknod /dev/mtr0 c 250 0
mknod /dev/nvram c 251 0
mknod /dev/gpio c 252 0
mknod /dev/rdm0 c 253 0
mknod /dev/watchdog c 10 130
mknod /dev/pcm0 c 233 0
mknod /dev/i2s0 c 234 0
mknod /dev/cls0	c 235 0

if [ "$CONFIG_SOUND" = "y" ] || [ "$CONFIG_SOUND" = "m" ]; then
mkdir /dev/snd
mknod /dev/snd/controlC0 c 116 0
mknod /dev/snd/pcmC0D0c	c 116 24
mknod /dev/snd/pcmC0D0p	c 116 16
fi

if [ "$CONFIG_MTK_VOIP" = "y" ] ; then
mknod /dev/slic	c 225 0
mknod /dev/vdsp	c 245 0
fi
