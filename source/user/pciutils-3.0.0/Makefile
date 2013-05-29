# Makefile for The PCI Utilities
# (c) 1998--2008 Martin Mares <mj@ucw.cz>

VERSION=3.0.0
DATE=2008-04-10

# Host OS and release (override if you are cross-compiling)
HOST=
RELEASE=

# Support for compressed pci.ids (yes/no, default: detect)
ZLIB=

# Support for resolving ID's by DNS (yes/no, default: detect)
DNS=

# Build libpci as a shared library (yes/no; or local for testing; requires GCC)
SHARED=no

# ABI version suffix in the name of the shared library
ABI_VERSION=.3

# Base name of the library (overriden on NetBSD, which has its own libpci)
LIBNAME=libpci
PCILIB=libpci.a
CFLAGS += -I$(ROOTDIR)/lib/zlib

-include lib/config.mk

PCIINC=lib/config.h lib/header.h lib/pci.h lib/types.h lib/sysdep.h
PCIINC_INS=lib/config.h lib/header.h lib/pci.h lib/types.h

export

all: lib/$(PCILIB) lspci setpci 

lib/$(PCILIB): $(PCIINC) 
	$(MAKE) -C lib all

lspci: lspci.o common.o lib/$(PCILIB)
setpci: setpci.o common.o lib/$(PCILIB)

lspci.o: lspci.c pciutils.h $(PCIINC)
setpci.o: setpci.c pciutils.h $(PCIINC)
common.o: common.c pciutils.h $(PCIINC)

clean:
	rm -f `find . -name "*~" -o -name "*.[oa]" -o -name "\#*\#" -o -name TAGS -o -name core -o -name "*.orig"`
	rm -f lspci setpci lib/*.pc lib/*.so lib/*.so.* lib/*.a
	rm -rf maint/dist


romfs: all
	$(ROMFSINST)  /sbin/lspci
	$(ROMFSINST)  /sbin/setpci

