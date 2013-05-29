
all: Makefile
	make -f Makefile

Makefile:
#	LDFLAGS="-L../../lib/pcre-8.01/.libs -L../../../lib/pcre-8.01/.libs -L../../../../lib/pcre-8.01/.libs -L../../../../../lib/pcre-8.01/.libs -L../../../lib/libpcap-1.0.0 -L../../../../lib/libpcap-1.0.0 -L../../../../../lib/libpcap-1.0.0 -lpcre" CFLAGS="-I../../../lib/pcre-8.01 -I../../../../lib/pcre-8.01 -I../../../../../lib/pcre-8.01 -I../../../../../../lib/libpcap-1.0.0  -I../../../../../lib/libpcap-1.0.0 -I../../../../lib/libpcap-1.0.0 -I../../../lib/libpcap-1.0.0 -I../../lib/libpcap-1.0.0" ./configure --host=mipsel-linux --with-libpcap-includes=../../lib/libpcap-1.0.0/ --with-libpcap-libraries=../../lib/libpcap-1.0.0/ --with-libpcre-includes=../../lib/pcre-8.01/ --with-libpcre-libraries=../../lib/pcre-8.01/.libs --enable-dynamicplugin --disable-mpls --enable-decoder-preprocessor-rules --disable-aruba --disable-ipv6
	LDFLAGS="-L../../lib/pcre-8.01/.libs -L../../../lib/pcre-8.01/.libs -L../../../../lib/pcre-8.01/.libs -L../../../../../lib/pcre-8.01/.libs -L../../../lib/libpcap-1.0.0 -L../../../../lib/libpcap-1.0.0 -L../../../../../lib/libpcap-1.0.0 -lpcre" CFLAGS="-I../../../lib/pcre-8.01 -I../../../../lib/pcre-8.01 -I../../../../../lib/pcre-8.01 -I../../../../../../lib/libpcap-1.0.0  -I../../../../../lib/libpcap-1.0.0 -I../../../../lib/libpcap-1.0.0 -I../../../lib/libpcap-1.0.0 -I../../lib/libpcap-1.0.0" ./configure --host=mipsel-linux --with-libpcap-includes=../../lib/libpcap-1.0.0/ --with-libpcap-libraries=../../lib/libpcap-1.0.0/ --with-libpcre-includes=../../lib/pcre-8.01/ --with-libpcre-libraries=../../lib/pcre-8.01/.libs --disable-mpls --disable-aruba --disable-ipv6
clean:
	rm -rf *.so *.a *.o 
	make -f Makefile clean
	find . -name Makefile -exec rm -f {} \;
	rm -rf config.log config.status .deps stamp-h1

romfs:
	$(STRIP) src/.libs/snort
	$(ROMFSINST) src/.libs/snort /bin/snort
	$(ROMFSINST) etc/snort.conf /etc_ro/snort.conf
	$(ROMFSINST) etc/classification.config /etc_ro/classification.config
	$(ROMFSINST) etc/reference.config /etc_ro/reference.config
	$(ROMFSINST) etc/unicode.map /etc_ro/unicode.map

all-recursive: Makefile
	make -f Makefile all-recursive

all-am: Makefile
	make -f Makefile all-am

clean-am: Makefile
	make -f Makefile clean-am



