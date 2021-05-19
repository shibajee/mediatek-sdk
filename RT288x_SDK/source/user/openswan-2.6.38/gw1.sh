#!/bin/sh
echo 0 > /sys/class/net/eth3/queues/rx-0/rps_cpus
if [ "$1" == "1" -o "$1" == "ip4trans" ]; then
ipsec_add_main_in_conf.sh CONN1 psk 10.10.20.253 "10.10.20.253" "10.10.20.253" single 10.10.20.254 "10.10.20.254" "10.10.20.254" yes 123456789 transport
fi

if [ "$1" == "0" -o "$1" == "ip4tun" ]; then
route del -net 10.10.10.0 netmask 255.255.255.0
ifconfig br0 10.10.40.254 netmask 255.255.255.0
route add default gw 10.10.20.254
killall -q -USR1 udhcpd
echo "" > /var/udhcpd.leases
dhcp=`nvram_get 2860 dhcpEnabled`
if [ "$dhcp" = "1" ]; then
	#start=`nvram_get 2860 dhcpStart`
	#end=`nvram_get 2860 dhcpEnd`
	start="10.10.40.150"
	end="10.10.40.200"
	lan_if="br0"
	mask=`nvram_get 2860 dhcpMask`
	pd=`nvram_get 2860 dhcpPriDns`
	sd=`nvram_get 2860 dhcpSecDns`
	gw="10.10.40.254"
	#gw='nvram_get 2860 dhcpGateway'
	lease=`nvram_get 2860 dhcpLease`
	static1=`nvram_get 2860 dhcpStatic1 | sed -e 's/;/ /'`
	static2=`nvram_get 2860 dhcpStatic2 | sed -e 's/;/ /'`
	static3=`nvram_get 2860 dhcpStatic3 | sed -e 's/;/ /'`
	config-udhcpd.sh -s $start
	config-udhcpd.sh -e $end
	config-udhcpd.sh -i $lan_if
	config-udhcpd.sh -m $mask
	config-udhcpd.sh -g $gw
	if [ "$pd" != "" -o "$sd" != "" ]; then
		config-udhcpd.sh -d $pd $sd
	fi
	if [ "$lease" != "" ]; then
		config-udhcpd.sh -t $lease
	fi
	config-udhcpd.sh -r 1
fi
if [ "$2" != "ah+esp" ]; then 
ipsec_add_main_in_conf.sh CONN1 psk @LEFT "10.10.20.253" "10.10.40.0/24" subnet @RIGHT "10.10.20.254" "10.10.10.0/24" yes 123456789 tunnel
fi
fi

if [ "$1" == "4in6tun" ]; then
net_gw1.sh
cp /sbin/ipsec_4in6_gw1.conf /etc/ipsec.conf
cp /sbin/ipsec_4in6_gw1.secrets /etc/ipsec.secrets
fi

if [ "$1" == "ip6trans" ]; then
ip -6 addr add 2001:bbb:6401:8000::1/64 dev eth3
route -A inet6 add default gw 2001:bbb:6401:8000::2
echo 1 >/proc/sys/net/ipv6/conf/all/forwarding
cp /sbin/ipsec_ip6_gw1.conf /etc/ipsec.conf
cp /sbin/ipsec_4in6_gw1.secrets /etc/ipsec.secrets
fi

if [ "$1" != "4in6tun" -a "$1" != "ip6trans" -a "$2" != "ah+esp" ]; then
ipsec_add_ikekey_in_conf.sh psk aes128 sha1 86400 modp1024 86400 no
fi


if [ "$1" == "0" -o "$1" == "ip4tun" ]; then
iptables -I FORWARD -s 10.10.40.0/24 -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --set-mss 1452
iptables -I FORWARD -d 10.10.10.0/24 -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --set-mss 1452
iptables -I FORWARD -s 10.10.40.0/24 -p tcp --tcp-flags SYN,RST SYN  -j TCPMSS --clamp-mss-to-pmtu
iptables -I FORWARD -d 10.10.10.0/24 -p tcp --tcp-flags SYN,RST SYN  -j TCPMSS --clamp-mss-to-pmtu
#iptables -t nat -F
iptables -t nat -I POSTROUTING -o eth3 -s 10.10.40.0/24 -j ACCEPT
fi

if [ "$1" == "ip4tun" -a "$2" == "ah+esp" ]; then
cp /sbin/ipsec_gw1.conf /etc/ipsec.conf
cp /sbin/ipsec_gw1.secrets /etc/ipsec.secrets
fi

if [ "$1" != "help" -o "$1" != "-h" -o "$1" != "?" -o "$1" != "-?" ]; then
insmod crypto_k
ipsec_connect.sh ike CONN1&
sleep 3
ipsec auto --up CONN1&
else
echo "=== openswan config ==="
echo "gw1.sh ip4tun  : ipv4 tunnel mode "
echo "gw1.sh ip4trans : ipv4 transport mode"
echo "gw1.sh 4in6tun : 4in6 tunnel mode"
echo "gw1.sh ip6tun : ipv6 tunnel mode"
#echo "gw1.sh ip6trans : ipv6 transport mode"
echo "======================="
fi


