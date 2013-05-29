#!/bin/sh

SAMBA_FILE=/etc/smb.conf

if [ ! -n "$2" ]; then
	echo "insufficient arguments!"
	echo "Usage: $0 <dir name> <access path> <allow users>"
	echo "Example: $0 temp /temp admin"
	exit 0
fi

ALLOWUSERS="$3 $4 $5 $6 $7 $8 $9 ${10} ${11}"

echo "
["$1"]
path = "$2"
valid users = "$ALLOWUSERS"
browseable = yes
writable = yes
create mask = 0664
directory mask = 0775
" >> $SAMBA_FILE

