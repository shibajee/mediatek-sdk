#!/bin/sh
l2tpv3tun add tunnel tunnel_id 1 peer_tunnel_id 1 encap ip local 10.10.20.254 remote 10.10.20.253
l2tpv3tun add session tunnel_id 1 session_id 1 peer_session_id 1

ifconfig l2tpeth0 up 
brctl addif br0 l2tpeth0
