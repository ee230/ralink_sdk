TOPDIR=../
include $(TOPDIR)Rules.mak

CONFOPTS= --host=mipsel-linux --disable-cpp

LIB_LDFLAGS := $(subst -z defs,,$(LDFLAGS))

LIBPCRE_A= .libs/libpcre.a
LIBPCRE_SHARED=libpcre.so
LIBPCRE_SHARED_FULLNAME=libpcre-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so

all: Makefile
	make -f Makefile all

Makefile:
	AR= AS= CC=$(CROSS_COMPILE)gcc CFLAGS= CROSS= CROSS_COMPILE= CXX= HOSTCC= LD=$(CONFIG_CROSS_COMPILER_PATH)/mipsel-linux-ld OBJCOPY= RANLIB= STRIP= GCC_DIR= ./configure $(CONFOPTS)

all-recursive: Makefile
	make -f Makefile all-recursive

all-am: Makefile
	make -f Makefile all-am

clean-am: Makefile
	make -f Makefile clean-am

clean: Makefile
	rm -rf *.so *.a
	make -f Makefile clean
	find ./ -name Makefile -exec rm -f {} \;

romfs:

shared: all
	$(LD) $(LIB_LDFLAGS) -soname=$(LIBPCRE_SHARED).$(MAJOR_VERSION) \
           -o $(LIBPCRE_SHARED_FULLNAME) --whole-archive $(LIBPCRE_A) \
           --no-whole-archive $(TOPDIR)libc/misc/internals/interp.o \
           -L$(TOPDIR)lib -lc $(LDADD_LIBFLOAT) $(LIBGCC);
	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(LIBPCRE_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBPCRE_SHARED).$(MAJOR_VERSION)
	$(INSTALL) -m 644 $(LIBPCRE_SHARED_FULLNAME) $(TOPDIR)lib
	$(LN) -sf $(LIBPCRE_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBPCRE_SHARED)
	$(LN) -sf $(LIBPCRE_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBPCRE_SHARED).$(MAJOR_VERSION)
