#!/bin/sh

. /sbin/config.sh
. /sbin/global.sh
 

#====================
WAN_INT=$wan_if
LAN_INT=$lan_if

WAN_MAC="a"
LAN_MAC="b"

PIM6DD_FILE=/etc/pim6dd.conf
#====================


#global variable for function return
EUI_64_addr=""

#global variable for function return
is_mac_the_same=""

create_pim6dd_conf()
{
	i=3
	echo "" > $PIM6DD_FILE
	while [ true ]; do
		int=`sed -n "$i""p" /proc/net/dev | sed -e 's/\ *\([a-zA-Z0-9]*\):.*/\1/' | tr -d "\n"`
		if [ "$int" = "" ]; then
			# loop done
			break
		fi

		if [ "$int" != "$WAN_INT" -a "$int" != "$LAN_INT" ]; then
			echo "phyint ""$int"" disable" >> $PIM6DD_FILE
		fi
		i=`expr $i + 1`
	done
}

# EUI address calculation for ipv6 ready logo test
ipv6_ready_logo_EUI_64()
{
	eth_mac=`ifconfig $1 | sed -n '/HWaddr/p' | sed -e 's/.*HWaddr \(.*\)/\1/'`
	mac1=`echo $eth_mac | sed -e 's/\(.*\):\(.*\):\(.*\):\(.*\):\(.*\):\(.*\)/\1/'`
	mac2=`echo $eth_mac | sed -e 's/\(.*\):\(.*\):\(.*\):\(.*\):\(.*\):\(.*\)/\2/'`
	mac3=`echo $eth_mac | sed -e 's/\(.*\):\(.*\):\(.*\):\(.*\):\(.*\):\(.*\)/\3/'`
	mac4=`echo $eth_mac | sed -e 's/\(.*\):\(.*\):\(.*\):\(.*\):\(.*\):\(.*\)/\4/'`
	mac5=`echo $eth_mac | sed -e 's/\(.*\):\(.*\):\(.*\):\(.*\):\(.*\):\(.*\)/\5/'`
	mac6=`echo $eth_mac | sed -e 's/\(.*\):\(.*\):\(.*\):\(.*\):\(.*\):\(.*\)/\6/'`

	mac1_a=`echo $mac1 | sed -e 's/\(.\)\(.\)/\1/'`
	mac1_b=`echo $mac1 | sed -e 's/\(.\)\(.\)/\2/'`

#invert the 7th bit
	case $mac1_b in
		"0")    mac1_b="2" ;;
		"1")    mac1_b="3" ;;
		"2")    mac1_b="0" ;;
		"3")    mac1_b="1" ;;
		"4")    mac1_b="6" ;;
		"5")    mac1_b="7" ;;
		"6")    mac1_b="4" ;;
		"7")    mac1_b="5" ;;
		"8")    mac1_b="A" ;;
		"9")    mac1_b="B" ;;
		"A")    mac1_b="8" ;;
		"B")    mac1_b="9" ;;
		"C")    mac1_b="E" ;;
		"D")    mac1_b="F" ;;
		"E")    mac1_b="C" ;;
		"F")    mac1_b="D" ;;
		*)              echo "Unknown error";return;;
	esac

	if [ "$2" = "WAN" ]; then
		EUI_64_addr="3ffe:0501:ffff:0101:$mac1_a$mac1_b$mac2:$mac3""ff"":fe$mac4:$mac5$mac6"
		WAN_MAC=$eth_mac
	else
		EUI_64_addr="3ffe:0501:ffff:0100:$mac1_a$mac1_b$mac2:$mac3""ff"":fe$mac4:$mac5$mac6"
		LAN_MAC=$eth_mac
	fi
	return;
}

brctl setfd br0 1

# move it to "internet.sh"
#echo "2" > /proc/sys/net/ipv6/conf/br0/dad_transmits


ipv6_ready_logo_EUI_64 $WAN_INT "WAN"
ifconfig $WAN_INT add $EUI_64_addr/64

ipv6_ready_logo_EUI_64 $LAN_INT "LAN"
ifconfig $LAN_INT add $EUI_64_addr/64

if [ "$WAN_MAC" = "$LAN_MAC" ]; then
	echo
	echo "ipv6_logo.sh: Warning. WAN interface has the same MAC address with LAN!!"
	echo "ipv6_logo.sh: Change the WAN or LAN's MAC address to pass IPv6 Ready Logo."
	echo
	#sleep 5
	ifconfig $WAN_INT down
	ifconfig $WAN_INT hw ether 00:11:22:33:44:55
	ifconfig $WAN_INT up

	ipv6_ready_logo_EUI_64 $WAN_INT "WAN"
	ifconfig $WAN_INT add $EUI_64_addr/64
fi

echo "1" > /proc/sys/net/ipv6/conf/all/forwarding

create_pim6dd_conf

