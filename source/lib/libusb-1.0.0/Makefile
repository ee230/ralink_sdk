TOPDIR=../
include $(TOPDIR)Rules.mak

CFLAGS+=-DHAVE_CONFIG_H -Ilibusb

LIBUSB=libusb.a
LIBUSB_VERS=1.0.0
LIBUSB_SHARED=libusb.so.$(LIBUSB_VERS)

OBJS    := libusb/core.o  libusb/descriptor.o  libusb/io.o  libusb/sync.o libusb/os/linux_usbfs.o

all: $(LIBUSB)

$(OBJS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

shared: all
	$(LD) $(LDFLAGS) -soname=$(LIBUSB_SHARED) \
		-o $(LIBUSB_SHARED) --whole-archive $(LIBUSB) \
		--no-whole-archive $(TOPDIR)libc/misc/internals/interp.o \
		-L$(TOPDIR)lib -lc -lpthread $(LDADD_LIBFLOAT) $(LIBGCC);
	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(LIBUSB_SHARED)
	$(INSTALL) -m 644 $(LIBUSB_SHARED) $(TOPDIR)lib


$(LIBUSB) ar-target: $(OBJS)
	$(AR) $(ARFLAGS) $(LIBUSB) $(OBJS)
	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(LIBUSB)
	$(INSTALL) -m 644 $(LIBUSB) $(TOPDIR)lib

romfs:
	$(INSTALL) -m 644 $(LIBUSB_SHARED) $(TOPDIR)lib

$(OBJS): Makefile

clean:
	$(RM) *.[oa] $(LIBUSB_SHARED)* libusb/*.o libusb/os/*.o

