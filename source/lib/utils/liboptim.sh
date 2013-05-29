#!/bin/sh
#
# Winfred Lu <winfred_lu@ralinktech.com.tw>:
#   This is a simple bash script that calls mklibs to make optimized libraries,
#   and substitute them for the original ones.
#

CURRDIR=`pwd`
ROMFS=${CURRDIR}/"../romfs"
DIRU=${CURRDIR}/"../user"
DIRWT=${DIRU}/wireless_tools
DIRNP=${DIRU}/ntfsprogs/libntfs
DIRUPNP=${CURRDIR}/libupnp
if [ -n "$1" ]; then
	CROSS=$1
else
	CROSS="mipsel-linux-"
fi

which python2.4 || exit 0

find ${ROMFS}/bin ${ROMFS}/sbin ${ROMFS}/usr/bin ${ROMFS}/usr/sbin -type f | sed '/\.sh$/d' > progs

cd lib
#echo "Before optimizing:" > ../lib_diff
#find . -name "*-0.9.28.so" -exec ls -l {} \; >> ../lib_diff
#echo "--------------------" >> ../lib_diff

for file in `find . -type f -name 'lib[a-zA-Z]*.a'`
do
	file_pic=`echo $file | sed -e 's/\.a/_pic.a/'`
	ln -sf $file $file_pic
done
rm -f libdl_pic.a
rm -f libc_pic.a
if [ -e ${DIRWT}/libiw.a ]; then
	ln -sf ${DIRWT}/libiw.a libiw_pic.a
fi
if [ -e ${DIRNP}/libntfs.a ]; then
	ln -sf ${DIRNP}/libntfs.a libntfs_pic.a
fi
if [ -e ${DIRUPNP}/libixml.a ]; then
	ln -sf ${DIRUPNP}/libixml.a libixml_pic.a
fi
if [ -e ${DIRUPNP}/libthreadutil.a ]; then
	ln -sf ${DIRUPNP}/libthreadutil.a libthreadutil_pic.a
fi
if [ -e ${DIRUPNP}/libupnp.a ]; then
	ln -sf ${DIRUPNP}/libupnp.a libupnp_pic.a
fi

cd ../
rm -rf liboptim
mkdir -p liboptim
utils/mklibs -v -d liboptim -D -L lib -L ${DIRWT} -L ${DIRNP} --target mipsel-linux `cat progs`
${CROSS}sstrip `cat progs` 2> /dev/null
${CROSS}sstrip liboptim/* 2> /dev/null

rm -f progs

#echo "After optimizing:" >> lib_diff
#ls -l liboptim/ >> lib_diff

