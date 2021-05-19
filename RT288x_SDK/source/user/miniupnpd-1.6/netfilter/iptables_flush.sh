#! /bin/sh
# $Id: //WIFI_SOC/MP/SDK_4_3_0_0/RT288x_SDK/source/user/miniupnpd-1.6/netfilter/iptables_flush.sh#1 $
IPTABLES=/sbin/iptables

#flush all rules owned by miniupnpd
$IPTABLES -t nat -F MINIUPNPD
$IPTABLES -t filter -F MINIUPNPD

