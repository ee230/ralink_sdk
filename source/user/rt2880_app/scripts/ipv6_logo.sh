#!/bin/sh

#====================

WAN_INT="eth2.2"
LAN_INT="br0"

WAN_MAC="a"
LAN_MAC="b"
#====================


#global variable for function return
EUI_64_addr=""

#global variable for function return
is_mac_the_same=""


# EUI address calculation for ipv6 ready logo test
ipv6_ready_logo_EUI_64()
{
	eth_mac=`ifconfig $1 | sed -n '/HWaddr/p' | sed -e 's/.*HWaddr \(.*\)/\1/'`
	mac1=`echo $eth_mac | sed -e 's/\(.*\):\(.*\):\(.*\):\(.*\):\(.*\):\(.*\)/\1/'`
	mac2=`echo $eth_mac | sed -e 's/\(.*\):\(.*\):\(.*\):\(.*\):\(.*\):\(.*\)/\2/'`
	mac3=`echo $eth_mac | sed -e 's/\(.*\):\(.*\):\(.*\):\(.*\):\(.*\):\(.*\)/\3/'`
	mac4=`echo $eth_mac | sed -e 's/\(.*\):\(.*\):\(.*\):\(.*\):\(.*\):\(.*\)/\4/'`
	mac5=`echo $eth_mac | sed -e 's/\(.*\):\(.*\):\(.*\):\(.*\):\(.*\):\(.*\)/\5/'`
	mac6=`echo $eth_mac | sed -e 's/\(.*\):\(.*\):\(.*\):\(.*\):\(.*\):\(.*\)/\6/'`

	mac1_a=`echo $mac1 | sed -e 's/\(.\)\(.\)/\1/'`
	mac1_b=`echo $mac1 | sed -e 's/\(.\)\(.\)/\2/'`

	case $mac1_b in
		"0")    mac1_b="2" ;;
		"1")    mac1_b="3" ;;
		"2")    mac1_b="2" ;;
		"3")    mac1_b="3" ;;
		"4")    mac1_b="6" ;;
		"5")    mac1_b="7" ;;
		"6")    mac1_b="6" ;;
		"7")    mac1_b="7" ;;
		"8")    mac1_b="a" ;;
		"9")    mac1_b="b" ;;
		"A")    mac1_b="a" ;;
		"B")    mac1_b="b" ;;
		"C")    mac1_b="e" ;;
		"D")    mac1_b="f" ;;
		"E")    mac1_b="e" ;;
		"F")    mac1_b="f" ;;
		*)              echo "Unknown error";return;;
	esac

	if [ "$2" = "WAN" ]; then
		EUI_64_addr="3ffe:0501:ffff:0101:$mac1_a$mac1_b$mac2:$mac3""ff"":fe$mac4:$mac5$mac6"
		WAN_MAC=$eth_mac
	else
		EUI_64_addr="3ffe:0501:ffff:0100:$mac1_a$mac1_b$mac2:$mac3""ff"":fe$mac4:$mac5$mac6"
		LAN_MAC=$eth_mac
	fi
	return;
}

brctl setfd br0 1

# move it to "internet.sh"
#echo "2" > /proc/sys/net/ipv6/conf/br0/dad_transmits

ipv6_ready_logo_EUI_64 $WAN_INT "WAN"
ifconfig $WAN_INT add $EUI_64_addr/64

ipv6_ready_logo_EUI_64 $LAN_INT "LAN"
ifconfig $LAN_INT add $EUI_64_addr/64

if [ "$WAN_MAC" = "$LAN_MAC" ]; then
	echo
	echo "ipv6_logo.sh: Warning. WAN interface has the same MAC address with LAN!!"
	echo "ipv6_logo.sh: Change the WAN or LAN's MAC address to pass IPv6 Ready Logo."
	echo
	sleep 5
fi
echo "1" > /proc/sys/net/ipv6/conf/all/forwarding

killall -9 ecmh
sleep 1
ecmh

