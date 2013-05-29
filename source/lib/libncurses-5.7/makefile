TOPDIR=../
include $(TOPDIR)Rules.mak

CONFOPTS= --host=mipsel-linux

LIB_LDFLAGS := $(subst -z defs,,$(LDFLAGS))


# upnp
LIBNCURSE_A= libncurses.a
LIBNCURSE_SHARED=libncurses.so
LIBNCURSE_SHARED_FULLNAME=libncurses-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so

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
	$(LD) $(LIB_LDFLAGS) -soname=$(LIBNCURSE_SHARED).$(MAJOR_VERSION) \
           -o $(LIBNCURSE_SHARED_FULLNAME) --whole-archive lib/$(LIBNCURSE_A) \
           --no-whole-archive $(TOPDIR)libc/misc/internals/interp.o \
           -L$(TOPDIR)lib -lc $(LDADD_LIBFLOAT) $(LIBGCC);

	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(LIBNCURSE_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBNCURSE_SHARED).$(MAJOR_VERSION)
	$(INSTALL) -m 644 $(LIBNCURSE_SHARED_FULLNAME) $(TOPDIR)lib
	$(LN) -sf $(LIBNCURSE_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBNCURSE_SHARED)
	$(LN) -sf $(LIBNCURSE_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBNCURSE_SHARED).$(MAJOR_VERSION)


