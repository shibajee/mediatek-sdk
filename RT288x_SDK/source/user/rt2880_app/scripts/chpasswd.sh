#!/bin/sh

# $Id: //WIFI_SOC/MP/SDK_4_3_0_0/RT288x_SDK/source/user/rt2880_app/scripts/chpasswd.sh#1 $
# usage: chpasswd.sh <user name> [<password>]

usage()
{
	echo "Usage:"
	echo "  $0 <user name> [<password>]"
	exit 1
}

if [ "$1" == "" ]; then
	echo "$0: insufficient arguments"
	usage $0
fi

echo "$1:$2" > /tmp/tmpchpw
chpasswd < /tmp/tmpchpw
rm -f /tmp/tmpchpw
