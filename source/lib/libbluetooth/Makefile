TOPDIR=../
include $(TOPDIR)Rules.mak

CFLAGS+=$(SSP_ALL_CFLAGS) -DHAVE_CONFIG_H -Iinclude/

LIBBLUETOOTH=libbluetooth.a
LIBBLUETOOTH_VERS=3.1.0
LIBBLUETOOTH_SHARED=libbluetooth.so.$(LIBBLUETOOTH_VERS)

OBJS    := bluetooth.o  hci.o  sdp.o

all: sym_link $(LIBBLUETOOTH)

sym_link:
	$(LN) -sf ../include include/bluetooth

$(OBJS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

shared: all
	$(LD) $(LDFLAGS) -soname=$(LIBBLUETOOTH_SHARED) \
		-o $(LIBBLUETOOTH_SHARED) --whole-archive $(LIBBLUETOOTH) \
		--no-whole-archive $(TOPDIR)libc/misc/internals/interp.o \
		-L$(TOPDIR)lib -lc $(LDADD_LIBFLOAT) $(LIBGCC);
	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(LIBBLUETOOTH_SHARED)
	$(INSTALL) -m 644 $(LIBBLUETOOTH_SHARED) $(TOPDIR)lib


$(LIBBLUETOOTH) ar-target: $(OBJS)
	$(AR) $(ARFLAGS) $(LIBBLUETOOTH) $(OBJS)
	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(LIBBLUETOOTH)
	$(INSTALL) -m 644 $(LIBBLUETOOTH) $(TOPDIR)lib

romfs:
	$(INSTALL) -m 644 $(LIBBLUETOOTH_SHARED) $(TOPDIR)lib

$(OBJS): Makefile

clean:
	$(RM) *.[oa] $(LIBBLUETOOTH_SHARED)*

