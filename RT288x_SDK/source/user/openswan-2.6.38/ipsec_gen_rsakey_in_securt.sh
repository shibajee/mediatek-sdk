#!/bin/sh
TMP_FILE=/var/ipsec.secrets
SECRET_FILE=/etc/ipsec.secrets
PLUTO_PID=/var/run/pluto/pluto.pid

if [ ! -n "$1" ]; then
	echo "insufficient arguments!"
	echo "Usage: $0 <bits> "
	exit 0
fi

BITS="$1"

#generate /etc/ipsec.secrets
ipsec newhostkey --output $TMP_FILE --bits $BITS
cat $TMP_FILE >> $SECRET_FILE
rm -f $TMP_FILE

echo " Generate $BITS bits RSA Key to /etc/ipsec.secrets" 

if [ -f $PLUTO_PID ]; then
	ipsec auto --rereadsecrets
fi
