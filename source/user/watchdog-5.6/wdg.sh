#!/bin/sh

CONF_FILE=/etc/watchdog.conf
echo "file  = /var/log/messages"  >> $CONF_FILE
echo "change  = 1407"  >> $CONF_FILE

# Uncomment to enable test. Setting one of these values to '0' disables it.
# These values will hopefully never reboot your machine during normal use
# (if your machine is really hung, the loadavg will go much higher than 25)
echo "max-load-1  = 24" >>  $CONF_FILE
echo "max-load-5  = 18" >>  $CONF_FILE
echo "max-load-15 = 12" >>  $CONF_FILE

# Note that this is the number of pages!
# To get the real size, check how large the pagesize is on your machine.
echo "min-memory  = 1" >> $CONF_FILE
echo "watchdog-device = /dev/watchdog"  >> $CONF_FILE

# This greatly decreases the chance that watchdog won't be scheduled before
# your machine is really loaded
echo "realtime = yes" >>  $CONF_FILE
echo "priority = 1" >> $CONF_FILE

# Defaults compiled into the binary
echo "admin = root" >> $CONF_FILE
echo "interval = 3" >> $CONF_FILE
echo "logtick = 1" >> $CONF_FILE

# Check if syslogd is still running by enabling the following line
echo "pidfile  = /var/run/syslogd.pid" >> $CONF_FILE

