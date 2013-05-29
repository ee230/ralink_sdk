#!/bin/sh

# udhcpc script edited by Tim Riker <Tim@Rikers.org>

. /sbin/config.sh
. /sbin/global.sh

[ -z "$1" ] && echo "Error: should be called from udhcpc" && exit 1

RESOLV_CONF="/etc/resolv.conf"
[ -n "$broadcast" ] && BROADCAST="broadcast $broadcast"
[ -n "$subnet" ] && NETMASK="netmask $subnet"

case "$1" in
    deconfig)
        /sbin/ifconfig $interface 0.0.0.0
        ;;

    renew|bound)
        /sbin/ifconfig $interface $ip $BROADCAST $NETMASK

        if [ -n "$router" ] ; then
            echo "deleting routers"
            while route del default gw 0.0.0.0 dev $interface ; do
                :
            done

            metric=0
            for i in $router ; do
                metric=`expr $metric + 1`
                route add default gw $i dev $interface metric $metric
            done
        fi

        echo -n > $RESOLV_CONF
        [ -n "$domain" ] && echo search $domain >> $RESOLV_CONF
        for i in $dns ; do
            echo adding dns $i
            echo nameserver $i >> $RESOLV_CONF
        done
		# notify goahead when the WAN IP has been acquired. --yy
		killall -SIGTSTP goahead

		# restart igmpproxy daemon
		config-igmpproxy.sh
		if [ "$wanmode" = "L2TP" ]; then
			if [ "$CONFIG_PPPOL2TP" == "y" ]; then
				openl2tpd
			else
				l2tpd
				sleep 1
				l2tp-control "start-session $l2tp_srv"
			fi
		elif [ "$wanmode" = "PPTP" ]; then
			pppd file /etc/options.pptp  &
		fi
        ;;
esac

exit 0

