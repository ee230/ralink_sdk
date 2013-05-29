#!/bin/sh

FTPD_FILE=/etc/stupid-ftpd.conf


if [ ! -n "$5" ]; then
  echo "insufficient arguments!"
  echo "Usage: $0 <user> <passwd> <home_dir> <max_logins> <flags:D/U/O/M/E/A>"
  echo "<flags>     D - download
                    U - upload + making directories
                    O - overwrite existing files
                    M - allows multiple logins
                    E - allows erase operations
                    A - allows EVERYTHING(!)"
  exit 0
fi

FTPD_USER="$1"
FTPD_PASSWD="$2"
FTPD_HOME_DIR="$3"
FTPD_MAX_LOGINS="$4"
FTPD_FLAGS="$5"

echo "user=$FTPD_USER $FTPD_PASSWD $FTPD_HOME_DIR $FTPD_MAX_LOGINS $FTPD_FLAGS"  >> $FTPD_FILE
