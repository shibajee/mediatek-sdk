#!/bin/sh
IPSEC_CONF_FILE=/etc/ipsec.conf

if [ ! -n "$6" ]; then
	echo "insufficient arguments!"
	echo "Usage: $0 <auth_type:rsa/psk> <Encrypt:3des/aes128/null> <Auth:md5/sha1>"
	echo "<spi:100~FFF> <esp_encrypt_key> <esp_auth_key> "
	exit 0
fi

TYPE="$1" #RSA/PSK
ENCRYPT="$2" # Encryption: 3des, aes128, null
AUTH="$3" # Auth: md5, sha1
SPI="$4" # Security Parameter Index=0x100-0xFFF
ESP_ENC_KEY="$5" 
ESP_AUTH_KEY="$6"

echo "  auth=esp" >> $IPSEC_CONF_FILE
echo "  esp=$ENCRYPT-$AUTH" >> $IPSEC_CONF_FILE

echo "  spi=0x$SPI" >> $IPSEC_CONF_FILE
echo "  espenckey=0x$ESP_ENC_KEY" >> $IPSEC_CONF_FILE
echo "  espauthkey=0x$ESP_AUTH_KEY" >> $IPSEC_CONF_FILE
