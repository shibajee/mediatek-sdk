#!/bin/sh

CMD=$1
IP=$2

case $CMD in
	"adddb")
		PATAMETER_LIST=$3
		if [ -f "/etc/dhcp_parameter" ]; then
			sed "/${IP}/"d /etc/dhcp_parameter > /etc/tmp; cp /etc/tmp /etc/dhcp_parameter; rm /etc/tmp
		fi
		echo "$IP=$PATAMETER_LIST" >> /etc/dhcp_parameter
		;;
	"query")
		PATAMETER_LIST=`sed -n "/${IP}/"p /etc/dhcp_parameter | sed 's/^.*=//'`
		OS_NUM=`sed -n "/=${PATAMETER_LIST}$/"p /etc_ro/fp_db | sed 's/=.*//'`
		OS_NAME=`sed -n "/=${OS_NUM}$/"p /etc_ro/fp_db | sed 's/=.*//'`
		echo $OS_NAME
		;;	
esac
