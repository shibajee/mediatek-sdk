#!/bin/sh

if [ ! -n "$2" ]; then
  echo ""
  echo "insufficient arguments!"
  echo "Usage: $0 <rps_cpus:0x1~0xf> <interface:ra0,rai0,eth2,eth3...etc>"
  echo ""
  exit 0
fi


CPU="$1"
INTF="$2"

echo $CPU > /sys/class/net/$INTF/queues/rx-0/rps_cpus
