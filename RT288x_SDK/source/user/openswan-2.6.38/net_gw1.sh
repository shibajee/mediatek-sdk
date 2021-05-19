#!/bin/sh
#ip -6 addr add 2001:ccc:6401:8000::1/64 dev br0
ip -6 addr add 2001:bbb:6401:8000::1/64 dev eth3

route -A inet6 add default gw 2001:bbb:6401:8000::2
echo 1 >/proc/sys/net/ipv6/conf/all/forwarding

ip -6 tunnel add ip6tnl1 mode ip4ip6 remote 2001:bbb:6401:8000::2 local 2001:bbb:6401:8000::1
ip link set dev ip6tnl1 up
ip -6 route add 2001::/4 dev ip6tnl1 metric 1
ip addr add 10.10.10.0/24 dev ip6tnl1