#!/bin/sh

FTPD_FILE=/etc/stupid-ftpd.conf


if [ ! -n "$1" ]; then
  echo "insufficient arguments!"
  echo "Usage: $0 <banned_hosts>"
  exit 0
fi

FTPD_BANNED_HOSTS="$1"

echo "ban=$FTPD_BANNED_HOSTS" >> $FTPD_FILE

