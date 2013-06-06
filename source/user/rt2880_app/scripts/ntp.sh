#!/bin/sh
#
# $Id: ntp.sh,v 1.4 2008-01-21 08:39:58 yy Exp $
#
# usage: ntp.sh
#

srv=`nvram_get 2860 NTPServerIP`
sync=`nvram_get 2860 NTPSync`
tz=`nvram_get 2860 TZ`


killall -q ntpclient

if [ "$srv" = "" ]; then
	exit 0
fi


if [ "$sync" = "" ]; then
	sync=2
fi

sync=`expr $sync \* 3600`

if [ "$tz" = "" ]; then
	tz="UCT_000"
fi

#debug
#echo "serv=$srv"
#echo "sync=$sync"
#echo "tz=$tz"

echo $tz > /etc/tmpTZ
sed -e 's#.*_\(-*\)0*\(.*\)#GMT-\1\2#' /etc/tmpTZ > /etc/tmpTZ2
sed -e 's#\(.*\)--\(.*\)#\1\2#' /etc/tmpTZ2 > /etc/TZ
rm -rf /etc/tmpTZ
rm -rf /etc/tmpTZ2
if hash nslookup 2>/dev/null; then
	while ! nslookup pool.ntp.org >/dev/null 2>&1 ; do
		echo "NTP: Waiting for Internet to be ready"
		sleep 4
	done
fi
ntpclient -s -c 0 -h $srv -i $sync &

