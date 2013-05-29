#!/bin/sh

CONF_FILE=/etc/bpalogin.conf

if [ ! -n "$3" ]; then
  echo "insufficient arguments!"
  echo "Usage: $0 <user> <password> <auth_server>"
  exit 0
fi

BIGPOND_USER_NAME="$1"
BIGPOND_PASSWORD="$2"
BIGPOND_AUTHSERVER="$3"

echo "username $BIGPOND_USER_NAME" > $CONF_FILE
echo "password $BIGPOND_PASSWORD" >>  $CONF_FILE
echo "authserver $BIGPOND_AUTHSERVER" >>  $CONF_FILE
echo "localport 5050" >> $CONF_FILE
echo "logging syslog" >> $CONF_FILE
echo "debuglevel 0" >> $CONF_FILE
echo "minheartbeatinterval 60" >> $CONF_FILE
echo "maxheartbeatinterval 420" >>  $CONF_FILE
echo "connectedprog bpa_connect" >> $CONF_FILE
echo "disconnectedprog bpa_disconnect" >> $CONF_FILE
