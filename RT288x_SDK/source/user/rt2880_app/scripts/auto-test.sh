#!/bin/sh
#
# $Id: //WIFI_SOC/TRUNK/RT288x_SDK/source/user/rt2880_app/scripts/nat.sh#6 $
#
# usage: auto-test.sh
#

lan_if=`ifconfig | grep eth2.1 | sed 's/.*eth/eth/' | sed 's/ .*$//'`
wan_if=`ifconfig | grep eth2.2 | sed 's/.*eth/eth/' | sed 's/ .*$//'`

if [ "$lan_if" == "" ]; then
	lan_if=`ifconfig | grep eth2 | sed 's/.*eth/eth/' | sed 's/ .*$//'`
fi
if [ "$wan_if" == "" ]; then
	wan_if=`ifconfig | grep eth3 | sed 's/.*eth/eth/' | sed 's/ .*$//'`
fi
if [ "$lan_if" == "" -o "$wan_if" == "" ]; then
	echo "!!! Can't execute LAN/WAN Bridging Test !!!"
	exit 1
fi
killall -q udhcpc
killall -q pppd
killall -q l2tpd
killall -q openl2tpd
ifconfig $wan_if 0.0.0.0
brctl addif br0 $wan_if
