#!/bin/sh

Usage()
{
	echo "Usage: $0 <command>"
	echo "  <command>: "
	echo "		  init - initialize minupnpd"
	echo "		  remove - Remove minupnpd"
	echo "Example:"
	echo "	$0 init"
	echo "	$0 remove"
	exit 1
}

if [ "$1" != "init" -a "$1" != "remove" ]; then
	echo "Unknown command!"
	Usage
	exit 1
fi

. /sbin/global.sh
. /sbin/config.sh

IPTABLES=iptables
WAN_IF=$wan_ppp_if
killall -q miniupnpd 1>/dev/null 2>&1
rm /etc/$MINIUPNPD_FILE 1>/dev/null 2>&1

$IPTABLES -t nat -F MINIUPNPD 1>/dev/null 2>&1
#rmeoving the rule to MINIUPNPD
$IPTABLES -t nat -D PREROUTING -i $WAN_IF -j MINIUPNPD 1>/dev/null 2>&1
$IPTABLES -t nat -X MINIUPNPD 1>/dev/null 2>&1

#removing the MINIUPNPD chain for filter
$IPTABLES -t filter -F MINIUPNPD 1>/dev/null 2>&1
#adding the rule to MINIUPNPD
$IPTABLES -t filter -D FORWARD -i $WAN_IF ! -o $WAN_IF -j MINIUPNPD 1>/dev/null 2>&1
$IPTABLES -t filter -X MINIUPNPD 1>/dev/null 2>&1

if [ "$1" == "init" ]; then
	MINIUPNPD_FILE=/etc/miniupnpd.conf
	LAN_IPADDR=`nvram_get 2860 lan_ipaddr`
	IGD=`nvram_get 2860 upnpEnabled`
	WPS1=`nvram_get 2860 WscModeOption`
	WPS2=`nvram_get rtdev WscModeOption`
	PORT=6352

	$IPTABLES -t nat -N MINIUPNPD
	$IPTABLES -t nat -A PREROUTING -i $WAN_IF -j MINIUPNPD
	$IPTABLES -t filter -N MINIUPNPD
	$IPTABLES -t filter -A FORWARD -i $WAN_IF ! -o $WAN_IF -j MINIUPNPD

	echo "ext_ifname=$WAN_IF

listening_ip=$LAN_IPADDR

port=$PORT

bitrate_up=800000000
bitrate_down=800000000

secure_mode=no

system_uptime=yes

notify_interval=30

uuid=68555350-3352-3883-2883-335030522880

serial=12345678

model_number=1

enable_upnp=no

" > $MINIUPNPD_FILE
	
	if [ "$WPS1" != "" -a "$WPS1" != "0" ]; then
		if [ "$IGD" == "1" ]; then
			miniupnpd -m 1 -I ra0 -P /var/run/miniupnpd.ra0 -G -i $WAN_IF -a $LAN_IPADDR -n 7777
		else
			miniupnpd -m 1 -I ra0 -P /var/run/miniupnpd.ra0 -i $WAN_IF -a $LAN_IPADDR -n 7777
		fi
		if [ "$WPS2" != "" -a "$WPS2" != "0" ]; then
			miniupnpd -m 1 -I rai0 -P /var/run/miniupnpd.rai0 -i $WAN_IF -a $LAN_IPADDR -n 8888
		fi
	elif [ "$WPS2" != "" -a "$WPS2" != "0" ]; then
		if [ "$IGD" == "1" ]; then
			miniupnpd -m 1 -I rai0 -P /var/run/miniupnpd.rai0 -G -i $WAN_IF -a $LAN_IPADDR -n 8888
		else
			miniupnpd -m 1 -I rai0 -P /var/run/miniupnpd.rai0 -i $WAN_IF -a $LAN_IPADDR -n 8888
		fi
	elif [ "$IGD" == "1" ]; then
		miniupnpd -G
	fi
fi
