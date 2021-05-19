#!/bin/sh
IPSEC_CONF_FILE=/etc/ipsec.conf
PLUTO_PID=/var/run/pluto/pluto.pid

if [ ! -n "$2" ]; then
       echo "insufficient arguments!"
       echo "Usage: $0 <key_mode:ike/manual> <conn_name:name/all_pf>"
       exit 0
fi

KEY_MODE="$1"
CONN_NAME="$2"

#If pluto is not running, just start it
if [ ! -f $PLUTO_PID ]; then
	exit 0
fi

#stop all profile
if [ $CONN_NAME = "all_pf" ]; then
	ipsec setup stop
fi

#stop single profile
if [ $KEY_MODE = "ike" ]; then
	ipsec auto --delete $CONN_NAME
else
	ipsec manual --down $CONN_NAME
fi

