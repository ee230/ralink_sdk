TOPDIR=../
include $(TOPDIR)Rules.mak

CONFOPTS= --host=mipsel-linux --with-event

LIB_LDFLAGS := $(subst -z defs,,$(LDFLAGS))

MAJOR_VERSION=2
MINOR_VERSION=0
SUBLEVEL=2
LIBEVENT_A= libevent.a
LIBEVENT_SHARED=libevent-$(MAJOR_VERSION).$(MINOR_VERSION).so
LIBEVENT_SHARED_FULLNAME=libevent-$(MAJOR_VERSION).$(MINOR_VERSION).so

all: Makefile
	make -f Makefile all

Makefile:
	AR= AS= CC=$(CROSS_COMPILE)gcc CFLAGS= CROSS= CROSS_COMPILE= CXX= HOSTCC= LD=$(CONFIG_CROSS_COMPILER_PATH)/mipsel-linux-ld OBJCOPY= RANLIB= STRIP= GCC_DIR= ./configure $(CONFOPTS)

# the is a proxy section --YY
all-recursive: Makefile
	make -f Makefile all-recursive

# the is a proxy section --YY
all-am: Makefile
	make -f Makefile all-am

# the is a proxy section --YY
clean-am: Makefile
	make -f Makefile clean-am

clean: Makefile
	rm -rf *.so *.a
	make -f Makefile clean
	find ./ -name Makefile -exec rm -f {} \;

romfs:

shared: all
	$(INSTALL) -d $(TOPDIR)lib
	$(INSTALL) -m 644 .libs/libevent.so $(TOPDIR)lib
	$(INSTALL) -m 644 .libs/libevent_core.so $(TOPDIR)lib
	$(INSTALL) -m 644 .libs/libevent_extra.so $(TOPDIR)lib
	$(INSTALL) -m 644 .libs/libevent_pthreads.so.0.0.0 $(TOPDIR)lib
	$(LN) -sf libevent_core.so $(TOPDIR)lib/libevent_core.so.2
	$(LN) -sf libevent.so $(TOPDIR)lib/libevent.so.2
	$(LN) -sf libevent_pthreads.so.0.0.0 $(TOPDIR)lib/libevent_pthreads.so.0


