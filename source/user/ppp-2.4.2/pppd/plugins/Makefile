#CC	= gcc
COPTS	= -O2 -g
CFLAGS	= $(COPTS) -I.. -I../../include -fPIC
LDFLAGS	= -shared
INSTALL	= install -o root

ifeq ($(CONFIG_USER_PPPD_WITH_PPPOE),y)
SUBDIRS := rp-pppoe
endif

ifeq ($(CONFIG_USER_PPPD_WITH_L2TP),y)
SUBDIRS += pppol2tp
endif

ifeq ($(CONFIG_USER_PPPD_WITH_PPTP),y)
SUBDIRS += pppopptp
endif

# Uncomment the next line to include the radius authentication plugin
# SUBDIRS += radius
PLUGINS := minconn.so passprompt.so passwordfd.so

# include dependencies if present
ifeq (.depend,$(wildcard .depend))
include .depend
endif

all:	$(PLUGINS)
	for d in $(SUBDIRS); do $(MAKE) $(MFLAGS) -C $$d all; done

%.so: %.c
	$(CC) -o $@ $(LDFLAGS) $(CFLAGS) $^

VERSION = $(shell awk -F '"' '/VERSION/ { print $$2; }' ../patchlevel.h)
LIBDIR	= $(DESTDIR)/usr/lib/pppd/$(VERSION)

romfs:
	for d in $(SUBDIRS); do $(MAKE) $(MFLAGS) -C $$d romfs; done

clean:
	rm -f *.o *.so *.a
	for d in $(SUBDIRS); do $(MAKE) $(MFLAGS) -C $$d clean; done

depend:
	$(CPP) -M $(CFLAGS) *.c >.depend
	for d in $(SUBDIRS); do $(MAKE) $(MFLAGS) -C $$d depend; done
