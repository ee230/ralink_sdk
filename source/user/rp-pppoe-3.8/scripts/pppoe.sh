#!/bin/sh

# usage: pppoe.sh <user> <password> <eth_name>
#        each parameter could be "default"

CONF_FILE=/etc/ppp/pppoe.conf
PAP_FILE=/etc/ppp/pap-secrets
CHAP_FILE=/etc/ppp/chap-secrets

if [ ! -n "$3" ]; then
  echo "insufficient arguments!"
  echo "Usage: $0 <user> <password> <eth_name>"
  exit 0
fi

PPPOE_USER_NAME="$1"
PPPOE_PASSWORD="$2"
PPPOE_IF="$3"

# Secrets for authentication using PAP
if [ -n "$PPPOE_USER_NAME" ] ; then
echo "# Secrets for authentication using PAP
# client server	secret	IP addresses" > $PAP_FILE
echo "\"$PPPOE_USER_NAME\"  *	\"$PPPOE_PASSWORD\" *" >> $PAP_FILE

echo "# Secrets for authentication using CHAP
# client server	secret	IP addresses" > $CHAP_FILE
echo "\"$PPPOE_USER_NAME\"  *	\"$PPPOE_PASSWORD\" *" >> $CHAP_FILE
fi


echo "#Ethernet card connected to DSL modem" > $CONF_FILE
echo "ETH=\"$PPPOE_IF\"" >> $CONF_FILE

echo "#Make sure /etc/ppp/pap-secrets contains the right
#username/password combination.">> $CONF_FILE
echo "USER=\"$PPPOE_USER_NAME\"">>$CONF_FILE

echo "#Bring link up on demand?">>$CONF_FILE
echo "DEMAND=no" >> $CONF_FILE

echo "#DNS type: SERVER=obtain from server; SPECIFY=use DNS1 and DNS2;" >>$CONF_FILE
echo "DNSTYPE=SERVER" >> $CONF_FILE

echo "#Obtain DNS server addresses from the peer" >>$CONF_FILE
echo "PEERDNS=yes" >>$CONF_FILE

echo "# Make the PPPoE connection your default route." >>$CONF_FILE
echo "DEFAULTROUTE=yes" >>$CONF_FILE

echo "# 0: keeps trying to connect forever after pppoe-start is called
# otherwise: give out after CONNECT_TIMEOUT seconds" >>$CONF_FILE
echo "CONNECT_TIMEOUT=30" >>$CONF_FILE

echo "# How often in seconds pppoe-start polls to check if link is up" >>$CONF_FILE
echo "CONNECT_POLL=2" >>$CONF_FILE

echo "# Specific desired AC Name" >>$CONF_FILE
echo "ACNAME=" >>$CONF_FILE

echo "# Specific desired service name" >>$CONF_FILE
echo "SERVICENAME=" >>$CONF_FILE

echo "# Character to "echo" at each poll.  Use PING=" >>$CONF_FILE
echo "PING=\".\"" >>$CONF_FILE

echo "# File where the pppoe-connect script writes its process-ID.
# Three files are actually used:
#   $PIDFILE       contains PID of pppoe-connect script
#   $PIDFILE.pppoe contains PID of pppoe process
#   $PIDFILE.pppd  contains PID of pppd process" >>$CONF_FILE
echo "PIDFILE=\"/var/run/pppoe.pid\"" >>$CONF_FILE

echo "# Do you want to use synchronous PPP?"  >>$CONF_FILE
echo "SYNCHRONOUS=no"  >>$CONF_FILE

echo "# Do you want to clamp the MSS?" >>$CONF_FILE
echo "CLAMPMSS=1412" >>$CONF_FILE

echo "# LCP interval and failure count." >>$CONF_FILE
echo "LCP_INTERVAL=20" >>$CONF_FILE
echo "LCP_FAILURE=3" >>$CONF_FILE

echo "# PPPOE_TIMEOUT should be about 4 x LCP_INTERVAL" >>$CONF_FILE
echo "PPPOE_TIMEOUT=80" >>$CONF_FILE

echo "# Firewalling: One of NONE, STANDALONE or MASQUERADE" >>$CONF_FILE
echo "FIREWALL=NONE" >>$CONF_FILE

echo "# Linux kernel-mode plugin pppd." >>$CONF_FILE
echo "LINUX_PLUGIN=/etc/ppp/plugins/rp-pppoe.so" >>$CONF_FILE

echo "# Any extra arguments to pass to pppoe." >>$CONF_FILE
echo "PPPOE_EXTRA=\"\"" >>$CONF_FILE

echo "# Any extra arguments to pass to pppd." >>$CONF_FILE
echo "PPPD_EXTRA=\"\""  >>$CONF_FILE

echo "# If you want pppoe-connect to exit when connection drops" >>$CONF_FILE
echo "#RETRY_ON_FAILURE=no" >>$CONF_FILE

