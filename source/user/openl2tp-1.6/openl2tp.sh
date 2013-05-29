#!/bin/sh

OPENL2TP_FILE=/etc/openl2tpd.conf

if [ ! -n "$4" ]; then
  echo "insufficient arguments!"
  echo "Usage: $0 <user> <password> <server_ip> <opmode> <idle_time>"
  exit 0
fi

OPENL2TP_USER_NAME="$1"
OPENL2TP_PASSWORD="$2"
OPENL2TP_SERVER_IP="$3"
OPENL2TP_OPMODE="$4"
OPENL2TP_IDLETIME="$5"

echo "ppp profile modify profile_name=default \\"  > $OPENL2TP_FILE
echo "auth_eap=no auth_mschapv1=no auth_mschapv2=no \\"  >> $OPENL2TP_FILE
echo "default_route=no "  >> $OPENL2TP_FILE

echo "tunnel create tunnel_name=ralink dest_ipaddr=$OPENL2TP_SERVER_IP \\" >> $OPENL2TP_FILE
if [ $OPENL2TP_OPMODE == "KeepAlive" ]; then
	echo "persist=yes" >> $OPENL2TP_FILE
elif [ $OPENL2TP_OPMODE == "OnDemand" ]; then
	OPENL2TP_IDLETIME=`expr $OPENL2TP_IDLETIME \* 60`
	echo "persist=no \\" >> $OPENL2TP_FILE
	echo "idle_timeout=$OPENL2TP_IDLETIME" >> $OPENL2TP_FILE
fi

echo "session create tunnel_name=ralink \\" >> $OPENL2TP_FILE
echo "session_name=ralink \\" >> $OPENL2TP_FILE
echo "user_name=$OPENL2TP_USER_NAME \\" >> $OPENL2TP_FILE
echo "user_password=$OPENL2TP_PASSWORD" >> $OPENL2TP_FILE
