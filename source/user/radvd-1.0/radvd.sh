#!/bin/sh

radvd=`nvram_get 2860 radvdEnabled`
RADVD_FILE=/etc/radvd.conf
killall -q radvd
ifconfig sit0 down
echo "0" > /proc/sys/net/ipv6/conf/all/forwarding
rm $RADVD_FILE

if [ "$radvd" != "1" -a "$1" = "" ]; then
	exit 0
fi


if [ "$1" = "" ]; then
	PREFIX=0:0:0:0
else
	PREFIX=$1
fi

echo "interface br0
{
	AdvSendAdvert on;
	AdvOtherConfigFlag on;
	MinRtrAdvInterval 30;
	MaxRtrAdvInterval 100;

	prefix $PREFIX::/64
	{
		AdvOnLink on;
		AdvAutonomous on;
		AdvRouterAddr off;
" > $RADVD_FILE
if [ "$1" = "" ]; then
echo "		Base6to4Interface eth2.2;
		AdvValidLifetime 300;
		AdvPreferredLifetime 120;
	};
};" >> $RADVD_FILE
else
echo "		AdvValidLifetime 30000;
		AdvPreferredLifetime 12000;
	};
};" >> $RADVD_FILE
fi

echo "1" > /proc/sys/net/ipv6/conf/all/forwarding
if [ "$1" = "" ]; then
	ifconfig sit0 up
	ifconfig sit0 add 2002:1101:101::1101:101/16
	route -A inet6 add 2000::/3 gw ::17.1.1.20 dev sit0
	route -A inet6 add 2002:1101:101:0::/64 dev br0
fi
radvd -C $RADVD_FILE -d 1 &
