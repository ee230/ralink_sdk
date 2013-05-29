#
# pppd makefile for Linux
# $Id: Makefile.linux,v 1.1 2008-08-04 06:11:51 winfred Exp $
#

# Default installation locations
BINDIR = /usr/sbin
MANDIR = /usr/man

PPPDSRCS = main.c magic.c fsm.c lcp.c ipcp.c upap.c chap.c md5.c ccp.c \
	   ipxcp.c auth.c options.c sys-linux.c md4.c chap_ms.c cbcp.c \
	   demand.c utils.c multilink.c tdb.c tty.c sha1dgst.c mppe.c \
	   extra_crypto.c
HEADERS =  callout.h pathnames.h patchlevel.h chap.h md5.h chap_ms.h md4.h \
	   ipxcp.h cbcp.h tdb.h sha_locl.h mppe.h extra_crypto.h
MANPAGES = pppd.8
PPPDOBJS = main.o magic.o fsm.o lcp.o ipcp.o upap.o chap.o md5.o ccp.o \
	   auth.o options.o demand.o utils.o sys-linux.o ipxcp.o multilink.o \
	   tdb.o tty.o cbcp.o mppe.o sha1dgst.o extra_crypto.o

all: pppd

#
# include dependancies if present and backup if as a header file
ifeq (.depend,$(wildcard .depend))
include .depend
endif

# CC = gcc
#
COPTS = -O2 -pipe -Wall -g
LIBS =
DEBUGFLAGS = -DDEBUGALL

ifneq ($(wildcard /usr/lib/libcrypt.*),)
LIBS += -lcrypt
endif

# Uncomment the next 2 lines to include support for Microsoft's
# MS-CHAP authentication protocol.
CHAPMS=y
USE_CRYPT=y
ifneq ($(wildcard /usr/lib/libcrypt.*),)
HAVE_CRYPT_H=y
endif

# Uncomment these lines to use MSLANMAN and MPPE options
# both need the MS-CHAP protocol above
# MSLANMAN=y
MPPE=y

# Uncomment the following for dynamic IP allocation
#DYNAMIC=y

# Uncomment the following for CBCP support
# CBCP_SUPPORT=y

# Uncomment the next line to include support for PPP packet filtering.
# This requires that the libpcap library and headers be installed
# and that the kernel driver support PPP packet filtering, which it
# doesn't yet.
#FILTER=y

HAS_SHADOW=y
#USE_PAM=y
#HAVE_INET6=y

PLUGIN=y

INCLUDE_DIRS= -I. -I../include

COMPILE_FLAGS= -D_linux_=1 -DHAVE_PATHS_H -DIPX_CHANGE -DHAVE_MULTILINK -DHAVE_MMAP

CFLAGS= $(COPTS) $(COMPILE_FLAGS) $(INCLUDE_DIRS)

ifdef CHAPMS
CFLAGS   += -DCHAPMS=1
ifndef USE_CRYPT
LIBS     := -ldes $(LIBS)
else
CFLAGS   += -DUSE_CRYPT=1
ifneq ($(wildcard /usr/include/crypt.h),)
CFLAGS   += -DHAVE_CRYPT_H=1
endif
endif
PPPDOBJS += md4.o chap_ms.o
ifdef MSLANMAN
CFLAGS   += -DMSLANMAN=1
endif
ifdef MPPE
CFLAGS   += -DMPPE=1
endif
endif

ifdef HAS_SHADOW
CFLAGS   += -DHAS_SHADOW
#LIBS     := -lshadow $(LIBS)
endif

# For "Pluggable Authentication Modules", see ftp.redhat.com:/pub/pam/.
ifdef USE_PAM
CFLAGS   += -DUSE_PAM
LIBS     := -lpam -ldl $(LIBS)
endif

# Lock library binary for Linux is included in 'linux' subdirectory.
ifdef LOCKLIB
LIBS     := -llock $(LIBS)
CFLAGS   += -DLOCKLIB=1
endif

ifdef DYNAMIC
CFLAGS   += -DDYNAMIC=1
endif

ifdef CBCP_SUPPORT
CFLAGS   += -DCBCP_SUPPORT=1
endif

ifdef PLUGIN
CFLAGS	+= -DPLUGIN
LDFLAGS	+= -Wl,-E
LIBS	+= -ldl
endif

ifdef FILTER
LIBS    += -lpcap
CFLAGS  += -DPPP_FILTER -I/usr/include/pcap
endif

ifdef HAVE_INET6
     PPPDSRCS += ipv6cp.c eui64.c
     HEADERS  += ipv6cp.h eui64.h
     PPPDOBJS += ipv6cp.o eui64.o
     CFLAGS   += -DINET6=1
endif


INSTALL= install -o root

install: pppd
	mkdir -p $(BINDIR) $(MANDIR)
	$(INSTALL) -s -c -m 555 pppd $(BINDIR)/pppd
	if chgrp pppusers $(BINDIR)/pppd 2>/dev/null; then \
	  chmod o-rx,u+s $(BINDIR)/pppd; fi
	$(INSTALL) -c -m 444 pppd.8 $(MANDIR)/man8

pppd: $(PPPDOBJS)
	$(CC)  $(CFLAGS) $(LDFLAGS) -o pppd $(PPPDOBJS) $(LIBS)

clean:
	rm -f $(PPPDOBJS) pppd *~ #* core

depend:
	$(CPP) -M $(CFLAGS) $(PPPDSRCS) >.depend
