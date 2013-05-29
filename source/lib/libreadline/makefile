TOPDIR=../
include $(TOPDIR)Rules.mak

CONFOPTS= --host=mipsel-linux

LIB_LDFLAGS := $(subst -z defs,,$(LDFLAGS))


# upnp
LIBREADLINE_A= libreadline.a
LIBREADLINE_SHARED=libreadline.so
LIBREADLINE_SHARED_FULLNAME=libreadline-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so

all: FORCE Makefile
	make -f Makefile all

FORCE:
	cp -f *.h readline

Makefile:
	AR= AS= CC=$(CROSS_COMPILE)gcc CFLAGS= CROSS= CROSS_COMPILE= CXX= HOSTCC= LD=$(CONFIG_CROSS_COMPILER_PATH)/mipsel-linux-ld OBJCOPY= RANLIB= STRIP= GCC_DIR= ./configure $(CONFOPTS)
#	CC="/opt/buildroot-gdb/bin/mipsel-linux-uclibc-gcc" ./configure $(CONFOPTS)

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
	$(LD) $(LIB_LDFLAGS) -soname=$(LIBREADLINE_SHARED).$(MAJOR_VERSION) \
           -o $(LIBREADLINE_SHARED_FULLNAME) --whole-archive $(LIBREADLINE_A) \
           --no-whole-archive $(TOPDIR)libc/misc/internals/interp.o \
           -L$(TOPDIR)lib -lc $(LDADD_LIBFLOAT) $(LIBGCC);

	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(LIBREADLINE_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBREADLINE_SHARED).$(MAJOR_VERSION)
	$(INSTALL) -m 644 $(LIBREADLINE_SHARED_FULLNAME) $(TOPDIR)lib
	$(LN) -sf $(LIBREADLINE_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBREADLINE_SHARED)
	$(LN) -sf $(LIBREADLINE_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBREADLINE_SHARED).$(MAJOR_VERSION)


