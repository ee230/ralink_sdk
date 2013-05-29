#!/bin/sh
CONF_FILE=/etc/dhcp-fwd.conf

if [ ! -n "$3" ]; then
  echo "insufficient arguments!"
    echo "Usage: $0 <client interface> <server interface> <serv_ip / bcast>"
      exit 0
      fi

clients_IF="$1"
server_IF="$2"
server_IP="$3"



echo "##------------------------------------------------------"  > $CONF_FILE
echo "##               ENVIRONMENT SETTINGS" >> $CONF_FILE
echo "##------------------------------------------------------" >> $CONF_FILE
echo "user		admin" >> $CONF_FILE
echo "group		admin" >> $CONF_FILE

echo "chroot		/bin/chroot" >> $CONF_FILE
echo "logfile		/var/log/dhcp-fwd.log" >> $CONF_FILE

echo "## The loglevel option is not supported yet." >> $CONF_FILE
echo "loglevel		1" >> $CONF_FILE

echo "pidfile		/var/run/dhcp-fwd.pid" >> $CONF_FILE

echo "## resources." >> $CONF_FILE
echo "ulimit core	0" >> $CONF_FILE
echo "ulimit stack	64K" >> $CONF_FILE
echo "ulimit data	32K" >> $CONF_FILE
echo "ulimit rss	200K" >> $CONF_FILE
echo "ulimit nproc	0" >> $CONF_FILE
echo "ulimit nofile	0" >> $CONF_FILE
echo "ulimit as	0" >> $CONF_FILE

echo "#		IFNAME		clients	servers bcast">> $CONF_FILE
echo "if	$clients_IF	true	false	true" >> $CONF_FILE
echo "if	$server_IF	false	true	true" >> $CONF_FILE


echo "#		IFNAME		agent-id" >> $CONF_FILE
echo "name	$server_IF	ws-c" >> $CONF_FILE



if [ "$3" == "bcast" ]; then
	echo "server	bcast	$server_IF" >> $CONF_FILE
  
else
	echo "server	ip	$server_IP" >> $CONF_FILE

fi

