#!/bin/sh

# start this script inside the watchdog source tree to create a releasable tarball

major=`grep MAJOR_VERSION= configure.in | cut -f2 -d"="`
minor=`grep MINOR_VERSION= configure.in | cut -f2 -d"="`
if [ -d ../watchdog-$major.$minor ]
then
	echo "target directory exists"
	exit 1
fi

cp -a . ../watchdog-$major.$minor
cd ../watchdog-$major.$minor
aclocal
autoheader
automake --add-missing --copy
autoconf
rm -rf autom4te.cache
cd ..
tar --exclude CVS --exclude .git -zcf watchdog-$major.$minor.tar.gz watchdog-$major.$minor && rm -rf watchdog-$major.$minor

