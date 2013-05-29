#!/bin/sh
# This is a "repair binary" for watchdog that allows the tests to fail N times
# within a given period before a reboot is called.  Note that this "grace
# period" should really be a functionality of watchdog itself, IMHO.
#
# Erik Rossen <rossen@prolibre.com>

# If one does not change the default watchdog loop time of 10 secords, N=12
# will allow two minutes of failures before a reboot is signaled.
N=12

# CMAXAGE is the age in seconds that the counter file may have before it is
# considered too old and is wiped out.
CMAXAGE=20

ERR=$1

COUNTER=/var/run/watchdog.counter

if test -f $COUNTER; then
	COUNTERAGE=$(stat -c %Y $COUNTER)
	NOW=$(date +%s)
	if test $(($COUNTERAGE+$CMAXAGE)) -lt $NOW ; then
		rm $COUNTER
	else
		I=$(cat $COUNTER)
	fi
fi

I=${I:-0}

I=$(($I+1))

logger -t "watchdog[$$]" "Failure $I of $N"
logger -t "watchdog[$$]" "PROCESS LIST:"
ps auxww | logger -t "watchdog[$$]" 

if test "$I" -ge "$N" ; then
	logger -t "watchdog[$$]" Too many failures.  Signalling reboot.
	rm $COUNTER
	exit $ERR
fi

echo $I > $COUNTER
exit 0
