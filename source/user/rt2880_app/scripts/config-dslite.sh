#!/bin/sh

. /sbin/config.sh
. /sbin/global.sh

usage()
{
	echo "Usage:"
	echo "  $0 <AFTR IPv6 ADDR> <WAN IPv6 ADDR> <IPv6 WAN DEFAULT GW>"
	echo "Example:"
	echo "  $0 2001:db8::1 2001:db8::254 2003:db8::1"
	exit 1
}

AFTR_ADDR=$1
WAN_ADDR=$2
WAN_DEFAULT_GW=$3

if [ "$3" = "" ]; then
	echo "$0: insufficient arguments"
	usage $0
fi

#IPv6 Address
echo "ip -6 addr add $WAN_ADDR/32 dev $wan_if"
ip -6 addr add $WAN_ADDR/32 dev $wan_if

#IPv6 Routing
echo 1 >  /proc/sys/net/ipv6/conf/all/forwarding

#Create DS-Lite Interface
echo "ip -6 tunnel add dsltun mode ipip6 remote $AFTR_ADDR local $WAN_ADDR dev $wan_if"
ip -6 tunnel add dsltun mode ipip6 remote $AFTR_ADDR local $WAN_ADDR dev $wan_if
echo "ip link set dev dsltun up"
ip link set dev dsltun up

#Add default route
echo "ip route add default dev dsltun"
ip route add default dev dsltun

#In order to allow the B4 to reach the AFTR's logical IPv6 tunnel I/F. 
#add a ipv6 default route in the Router: 
echo "ip -6 route add default dev $wan_if"
ip -6 route add default dev $wan_if

#Static IPv6 Route
echo "ip -6 route add $AFTR_ADDR/128 via $WAN_DEFAULT_GW"
ip -6 route add $AFTR_ADDR/128 via $WAN_DEFAULT_GW

