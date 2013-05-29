#!/bin/sh

#
# I try to get a repair script that can handle as many problems as possible.
# Feel free to send me some additions.
#
# (C) Michael Meskes <meskes@debian.org> Mon Jun 23 13:40:15 CEST 1997
# Placed under GPL.
# Improvements, and modification for Redhat by Marc Merlin 
# <marcsoft@merlins.org>

#
# who to send mail to
#
admin=root

#
# let's see what error message we got
#
case $1 in
#
#	ENFILE: file table overflow
#	=> increase file-max by 10%
#
 23) 	
	fm=`cat /proc/sys/fs/file-max`
	fm=`expr $fm + $fm / 10`
	echo $fm > /proc/sys/fs/file-max
#
#	create log entry
#
	echo "increased file-max to "$fm | logger -i -t repair -p daemon.info 
#
#	that's it, problem disappeared
#
	exit 0;;
#
#	ENETDOWN: network is down
#	ENETUNREACH: network is unreachable
#	=> try to reconfigure network interface, there is no guarantee that
#	   this helps, but if it does not, reboot won't either
#
	
100|101)
	if [ -x /etc/rc.d/init.d/network ]; then
		# Redhat
		/etc/rc.d/init.d/network stop
	elif [ -x /etc/init.d/networking ]; then
		# Debian
		/etc/init.d/networking stop
	else
		ifconfig |
		awk '/Link/ {print $1}' |
		while read device
		do
			ifconfig ${device} down 
		done
	fi

# Calling rmmod -a twice should remove all unused modules (including networking
# ones). It might not work with very old rmmod binaries though, I don't know
	rmmod -a
	rmmod -a

#
# make sure the modules gets back into it in case kerneld/kmod does not run
#
for module in `grep "^alias" /etc/conf.modules | awk '/eth/ {print $3}'`
do
        modprobe $module
done
		
#
#	bring it back up
#
if [ -x /etc/init.d/networking ]; then
    # Debian
    /etc/init.d/networking start
elif [ -x /etc/rc.d/init.d/network ]; then
    # Redhat
    /etc/rc.d/init.d/network start
else
    echo "Couldn't find network script to relaunch networking. Please edit $0" | logger -i -t repair -p daemon.info 
    exit $1
fi



#
#	create log entry
#
		echo "re-initialized network interface eth0" | logger -i -t repair -p daemon.info 
#		
#	that' all we can do here
#
	exit 0;;
esac

#
# couldn't do anything
# tell the sysadmin what's going on
#
if [ -x /usr/bin/mail ]
then
	echo `hostname`" is going down because of error "$1|/usr/bin/mail -s "System fault!" ${admin}
fi
#
# finally tell watchdog to reboot
#
exit $1
