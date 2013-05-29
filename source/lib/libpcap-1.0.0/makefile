TOPDIR=../
include $(TOPDIR)Rules.mak

CONFOPTS= --host=mipsel-linux --with-pcap

LIB_LDFLAGS := $(subst -z defs,,$(LDFLAGS))


# upnp
LIBPCAP_A= libpcap.a
LIBPCAP_SHARED=libpcap.so
LIBPCAP_SHARED_FULLNAME=libpcap-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so

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
	$(LD) $(LIB_LDFLAGS) -soname=$(LIBPCAP_SHARED).$(MAJOR_VERSION) \
           -o $(LIBPCAP_SHARED_FULLNAME) --whole-archive $(LIBPCAP_A) \
           --no-whole-archive $(TOPDIR)libc/misc/internals/interp.o \
           -L$(TOPDIR)lib -lc $(LDADD_LIBFLOAT) $(LIBGCC);

	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(LIBPCAP_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBPCAP_SHARED).$(MAJOR_VERSION)
	$(INSTALL) -m 644 $(LIBPCAP_SHARED_FULLNAME) $(TOPDIR)lib
	$(LN) -sf $(LIBPCAP_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBPCAP_SHARED)
	$(LN) -sf $(LIBPCAP_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBPCAP_SHARED).$(MAJOR_VERSION)


