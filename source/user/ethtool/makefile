CONFOPTS= --host=mipsel-linux

all: Makefile
	make -f Makefile

Makefile:
	AR= AS= CC=$(CROSS_COMPILE)gcc CFLAGS= CROSS= CROSS_COMPILE= CXX= HOSTCC= LD=$(CROSS_COMPILE)ld OBJCOPY= RANLIB= STRIP=$(CROSS_COMPILE)strip GCC_DIR= ./configure $(CONFOPTS)

clean:
	rm -rf *.so *.a *.o ethtool
	rm -rf config.log config.status .deps ethtool-config.h Makefile stamp-h1

romfs:
#	$(STRIP) ethtool
	$(ROMFSINST) ethtool /bin/ethtool

# the is a proxy section --YY
all-recursive: Makefile
	make -f Makefile all-recursive

# the is a proxy section --YY
all-am: Makefile
	make -f Makefile all-am

# the is a proxy section --YY
clean-am: Makefile
	make -f Makefile clean-am



