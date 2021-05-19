#!/bin/sh
IPSEC_CONF_FILE=/etc/ipsec.conf

if [ ! -n "$7" ]; then
	echo "insufficient arguments!"
	echo "Usage: $0 <auth_type:rsa/psk> <encrypt:3des/aes128/null>"
	echo "          <auth:md5/sha1> <lifetime> <key_grp:modp768/modp1024/modp1536>"
	echo "          <ikelifetime> <pfs:yes/no>"
	exit 0
fi

TYPE="$1" #RSA/PSK
ENCRYPT="$2" # Encryption: 3des, aes128, null
AUTH="$3" # Auth: md5, sha1
LIFETIME="$4"
KEY_GRP="$5" # Key Group: modp768, modp1024, modp1536
IKELIFETIME="$6"
PFS="$7" # Perfect Forward Secrecy: yes, no

if [ $AUTH = "null" ]; then
echo "  esp=$ENCRYPT" >> $IPSEC_CONF_FILE
else
echo "  auth=esp" >> $IPSEC_CONF_FILE
echo "  esp=$ENCRYPT-$AUTH" >> $IPSEC_CONF_FILE
#echo "  auth=ah" >> $IPSEC_CONF_FILE
#echo "  ah=md5 >> $IPSEC_CONF_FILE
fi
echo "  lifetime=$LIFETIME""s" >> $IPSEC_CONF_FILE

#if [ $TYPE = "rsa" ]; then
#echo "  authby=rsasig" >> $IPSEC_CONF_FILE

if [ $TYPE = "never" ]; then
echo "  authby=never" >> $IPSEC_CONF_FILE
else  
echo "  authby=secret" >> $IPSEC_CONF_FILE
fi

if [ $AUTH = "null" ]; then
echo "  ike=$ENCRYPT-$KEY_GRP" >> $IPSEC_CONF_FILE
else
echo "  ike=$ENCRYPT-$AUTH-$KEY_GRP" >> $IPSEC_CONF_FILE
fi
echo "  ikelifetime=$IKELIFETIME""s" >> $IPSEC_CONF_FILE
echo "  pfs=$PFS" >> $IPSEC_CONF_FILE

