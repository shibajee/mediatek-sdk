#!/bin/sh
IPSEC_CONF_FILE=/etc/ipsec.conf
IPSEC_SECRET_FILE=/etc/ipsec.secrets

if [ ! -n "${12}" ]; then
	echo "insufficient arguments!"
	echo "Usage: $0 <tunnel_name> <auth_type:psk/rsa> <left_id> <left> <left_subnet> "
	echo "          <right_type:single/subnet/any/natt> <right_id> <right> <right_subnet> <nat-t:yes/no> <pskey/rsakey> <transport/tunnel>"
	echo "key:"
	echo "		-psk=pre shared key"
	echo "		-rsa=right rsa key"
	echo "right_subnet:"
	echo "		-Single: x.x.x.x/32"
	echo "		-Subnet: x.x.x.x/[0~32]"
	echo "		-Any: %any"
	echo "		-NAT-T: %any"
	exit 0
fi

TUNNEL_NAME="$1"
AUTH_TYPE="$2" #PSK/RSA
LEFTID="$3"
LEFT="$4" # x.x.x.x
LEFT_SUBNET="$5" # x.x.x.x/x
RIGHT_TYPE="$6"
RIGHTID="$7"
RIGHT="$8" # x.x.x.x
RIGHT_SUBNET="$9" 
NAT_TRAVERSAL="$10" #yes/no
KEY="${11}"
T_MODE="${12}"
echo "Generate connection profile to /etc/ipsec.conf"

if [ ! -f $IPSEC_CONF_FILE ]; then
echo "version 2.0" >>  $IPSEC_CONF_FILE
echo "" >>  $IPSEC_CONF_FILE
echo "config setup" >> $IPSEC_CONF_FILE
echo "  interfaces=%defaultroute" >> $IPSEC_CONF_FILE
echo "  klipsdebug=none" >> $IPSEC_CONF_FILE
echo "  plutodebug=none" >> $IPSEC_CONF_FILE
echo "  nat_traversal=$NAT_TRAVERSAL" >> $IPSEC_CONF_FILE
echo "  protostack=netkey" >> $IPSEC_CONF_FILE
echo "  plutostderrlog=/var/log/pluto.log" >> $IPSEC_CONF_FILE
echo "" >> $IPSEC_CONF_FILE
fi

echo "conn $TUNNEL_NAME" >> $IPSEC_CONF_FILE
#echo "  connaddrfamily=ipv6" >> $IPSEC_CONF_FILE
echo "  left=$LEFT" >> $IPSEC_CONF_FILE
if [ $T_MODE = "tunnel" ]; then
echo "  leftsubnet=$LEFT_SUBNET" >> $IPSEC_CONF_FILE
fi
echo "  leftid=$LEFTID" >> $IPSEC_CONF_FILE
echo "  type=$T_MODE" >> $IPSEC_CONF_FILE
echo "  rekey=yes" >> $IPSEC_CONF_FILE
echo "  rekeymargin=30s" >> $IPSEC_CONF_FILE
echo "  forceencaps=$NAT_TRAVERSAL" >> $IPSEC_CONF_FILE

#if [ $T_MODE = "transport" ]; then
#echo "  leftprotoport=6/23" >> $IPSEC_CONF_FILE
#echo "  rightprotoport=6/23" >> $IPSEC_CONF_FILE
#echo "  leftprotoport=17/1701" >> $IPSEC_CONF_FILE
#echo "  rightprotoport=17/1701" >> $IPSEC_CONF_FILE
#fi

if [ $AUTH_TYPE = "rsa" ]; then
ipsec showhostkey --left >> $IPSEC_CONF_FILE
fi

if [ $RIGHT_TYPE = "single" -o $RIGHT_TYPE = "subnet" ]; then
echo "  right=$RIGHT" >> $IPSEC_CONF_FILE
if [ $T_MODE = "tunnel" ]; then
echo "  rightsubnet=$RIGHT_SUBNET" >> $IPSEC_CONF_FILE
fi
elif [ $RIGHT_TYPE = "any" ]; then
echo "  right=any" >> $IPSEC_CONF_FILE
elif [ $RIGHT_TYPE = "natt" ]; then
echo "  right=any" >> $IPSEC_CONF_FILE
echo "  rightsubnetwithin=$LEFT_SUBNET" >> $IPSEC_CONF_FILE
else
	exit 0;
fi

echo "  rightid=$RIGHTID" >> $IPSEC_CONF_FILE
if [ $AUTH_TYPE = "rsa" ]; then
ipsec showhostkey --right >> $IPSEC_CONF_FILE
fi

echo "  auto=add" >> $IPSEC_CONF_FILE

if [ $AUTH_TYPE = "psk" ]; then
echo " Add PSA Key to /etc/ipsec.secrets"
echo "$LEFTID $RIGHTID : PSK \"$KEY\" " >> $IPSEC_SECRET_FILE
fi

