#!/bin/sh

opmode=`nvram_get 2860 OperationMode`
SAMBA_FILE=/etc/smb.conf

if [ ! -n "$2" ]; then
	echo "insufficient arguments!"
	echo "Usage: $0 <netbios_name> <workgroup>"
	echo "Example: $0 MT7623 Mediatek"
	exit 0
fi

NETBIOS_NAME="$1"
WORKGROUP="$2"
LOGPATH="$3"

echo "$LOGPATH"

echo "[global]
use sendfile = yes
use mmap = yes
load printers = no
netbios name = $NETBIOS_NAME
server string = Samba Server
workgroup = $WORKGROUP
security = user
log file = /var/log.samba
socket options = TCP_NODELAY SO_RCVBUF=480000 SO_SNDBUF=40000
encrypt passwords = yes
disable spoolss = yes
host msdfs = no
strict allocate = No
os level = 20
log level = 0
max log size = 100
dos charset = ASCII
unix charset = UTF8
display charset = UTF8
guest account = admin
bind interfaces only = no" > $SAMBA_FILE
if [ "$opmode" = 0 ]; then
	echo "interfaces = lo br0" >> $SAMBA_FILE
elif [ "$opmode" = 1 ]; then
	echo "interfaces = lo br0 eth2.2" >> $SAMBA_FILE
elif [ "$opmode" = 2 ]; then
	echo "interfaces = lo eth2 ra0" >> $SAMBA_FILE
elif [ "$opmode" = 3 ]; then
	echo "interfaces = lo br0 apcli0" >> $SAMBA_FILE
else
	echo "interfaces = lo" >> $SAMBA_FILE
fi

iptables -t raw -D PREROUTING -p tcp --dport 445  -j NOTRACK
iptables -t raw -A PREROUTING -p tcp --dport 445  -j NOTRACK
iptables -t raw -D OUTPUT -p tcp --sport 445  -j NOTRACK
iptables -t raw -A OUTPUT -p tcp --sport 445  -j NOTRACK

