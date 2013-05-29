#!/bin/sh

FTPD_FILE=/etc/stupid-ftpd.conf

if [ ! -n "$4" ]; then
  echo "insufficient arguments!"
  echo "Usage: $0 <port> <max_users> <login_timeout> <stay_timeout>"
  exit 0
fi

FTPD_PORT="$1"
FTPD_MAX_USERS="$2"
FTPD_LOGIN_TIMEOUT="$3"
FTPD_STAY_TIMEOUT="$4"

echo "mode=daemon" > $FTPD_FILE
echo "serverroot=/sbin/stupid-ftpd" >> $FTPD_FILE
echo "changeroottype=real" >> $FTPD_FILE
echo "port=$FTPD_PORT" >> $FTPD_FILE
echo "maxusers=$FTPD_MAX_USERS" >> $FTPD_FILE
echo "login-timeout=$FTPD_LOGIN_TIMEOUT" >> $FTPD_FILE
echo "timeout=$FTPD_STAY_TIMEOUT" >> $FTPD_FILE
echo "banmsg=Go away !" >> $FTPD_FILE

# Message Of The Day (motd)
# It will be displayed after the login procedure.

#motd=/tmp/stupid-ftpd.motd

# Message on quit
# It will be displayed when quitting.

#byemsg=/tmp/stupid-ftpd.bye

# Log
#log=/var/stupid-ftpd.log
