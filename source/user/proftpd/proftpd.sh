#!/bin/sh

PROFTPD_FILE=/etc/proftpd.conf

Usage()
{
	echo "insufficient arguments!"
	echo "Usage: $0 <target> [option]..."
	echo "       $0 server <ServerName> <IP> <TcpPort> <MaxConn>"
	echo "       $0 user <AddDir> <ReName> <ReMove> <Read> <Write> <Download> <Upload>"
	exit 0
}

setFtpServer()
{
	# This is a basic ProFTPD configuration file (rename it to 
	# 'proftpd.conf' for actual use.  It establishes a single server
	# and a single anonymous login.  It assumes that you have a user/group
	# "nobody" and "ftp" for normal operation and anon.

	echo "ServerName $SERVER_NAME" > $PROFTPD_FILE
	echo "DefaultAddress $IP_ADDR" >> $PROFTPD_FILE
	echo "ServerType  standalone" >> $PROFTPD_FILE
	echo "DefaultServer on" >> $PROFTPD_FILE
	echo "AuthUserFile /etc/passwd" >> $PROFTPD_FILE
	#ScoreboardFile	/etc/proftpd.scoreboard

	# Port 21 is the standard FTP port.
	echo "Port $TCP_PORT" >> $PROFTPD_FILE

	# Umask 022 is a good standard umask to prevent new dirs and files
	# from being group and world writable.
	echo "Umask 022" >> $PROFTPD_FILE

	# To prevent DoS attacks, set the maximum number of child processes
	# to 30.  If you need to allow more than 30 concurrent connections
	# at once, simply increase this value.  Note that this ONLY works
	# in standalone mode, in inetd mode you should use an inetd server
	# that allows you to limit maximum number of processes per service
	# (such as xinetd).
	echo "MaxInstances $MAX_CONN" >> $PROFTPD_FILE

	# Set the user and group under which the server will run.
	echo "User admin" >> $PROFTPD_FILE
	echo "Group admin" >> $PROFTPD_FILE

	# To cause every FTP user to be "jailed" (chrooted) into their home
	# directory, uncomment this line.
	# DefaultRoot ~
	echo "DefaultRoot ~" >> $PROFTPD_FILE

	echo "UseReverseDNS off" >> $PROFTPD_FILE
	echo "IdentLookups off" >> $PROFTPD_FILE

	echo "RootLogin on" >> $PROFTPD_FILE

	echo "DelayEngine off" >> $PROFTPD_FILE
}

setFtpUser()
{
	if [ "$MKDCAP" == "0" ]; then
		echo "<Limit MKD>" >> $PROFTPD_FILE
		echo "  Denyall" >> $PROFTPD_FILE
		echo "</Limit>" >> $PROFTPD_FILE
	fi
	if [ "$RNFRCAP" == "0" ]; then
		echo "<Limit RNFR>" >> $PROFTPD_FILE
		echo "  Denyall" >> $PROFTPD_FILE
		echo "</Limit>" >> $PROFTPD_FILE
	fi
	if [ "$DELECAP" == "0" ]; then
		echo "<Limit DELE>" >> $PROFTPD_FILE
		echo "  Denyall" >> $PROFTPD_FILE
		echo "</Limit>" >> $PROFTPD_FILE
		echo "<Limit RMD>" >> $PROFTPD_FILE
		echo "  Denyall" >> $PROFTPD_FILE
		echo "</Limit>" >> $PROFTPD_FILE
	fi
	if [ "$READCAP" == "0" ]; then
		echo "<Limit READ>" >> $PROFTPD_FILE
		echo "  Denyall" >> $PROFTPD_FILE
		echo "</Limit>" >> $PROFTPD_FILE
	fi
	if [ "$WRITECAP" == "0" ]; then
		echo "<Limit WRITE>" >> $PROFTPD_FILE
		echo "  Denyall" >> $PROFTPD_FILE
		echo "</Limit>" >> $PROFTPD_FILE
	fi
	if [ "$RETRCAP" == "0" ]; then
		echo "<Limit RETR>" >> $PROFTPD_FILE
		echo "  Denyall" >> $PROFTPD_FILE
		echo "</Limit>" >> $PROFTPD_FILE
	fi
	if [ "$STORCAP" == "0" ]; then
		echo "<Limit STOR>" >> $PROFTPD_FILE
		echo "  Denyall" >> $PROFTPD_FILE
		echo "</Limit>" >> $PROFTPD_FILE
		fullcap=0
	fi
	if [ "$STORCAP" == "0" ]; then
		echo "<Limit STOR>" >> $PROFTPD_FILE
		echo "  Denyall" >> $PROFTPD_FILE
		echo "</Limit>" >> $PROFTPD_FILE
	fi
	if [ "$fullcap" == "1" ]; then
		echo "<Limit ALL>" >> $PROFTPD_FILE
		echo "  Allowall" >> $PROFTPD_FILE
		echo "</Limit>" >> $PROFTPD_FILE
	fi
}

if [ "$1" == "server" ]; then
	if [ ! -n "$5" ]; then
		Usage
	else
		SERVER_NAME="$2"
		IP_ADDR="$3"
		TCP_PORT="$4"
		MAX_CONN="$5"
		setFtpServer
	fi
elif [ "$1" == "user" ]; then
	if [ ! -n "$8" ]; then
		Usage
	else
		MKDCAP=$2
		RNFRCAP=$3
		DELECAP=$4
		READCAP=$5
		WRITECAP=$6
		RETRCAP=$7
		STORCAP=$8
		setFtpUser
	fi
elif [ "$1" == "anonymous" ]; then
	if [ ! -n "$2" ]; then
		MAXCLIENTS=10
	else
		MAXCLIENTS=$2
	fi
	echo "<Anonymous ~>" >> $PROFTPD_FILE  
	echo "User		anonymous" >> $PROFTPD_FILE
	echo "Group		anonymous" >> $PROFTPD_FILE
	echo "MaxClients	$MAXCLIENTS" >> $PROFTPD_FILE
	echo "</Anonymous>" >> $PROFTPD_FILE
else
	Usage
fi

iptables -t raw -D PREROUTING -p tcp --dport 20  -j NOTRACK
iptables -t raw -A PREROUTING -p tcp --dport 20  -j NOTRACK
iptables -t raw -D OUTPUT -p tcp --sport 20  -j NOTRACK
iptables -t raw -A OUTPUT -p tcp --sport 20  -j NOTRACK

