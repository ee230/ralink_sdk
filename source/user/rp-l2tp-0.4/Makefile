# Generated automatically from Makefile.in by configure.
# Generated automatically from Makefile.in by configure.
#***********************************************************************
#
# Makefile
#
# Makefile for L2TP code.
#
# Copyright (C) 2002 Roaring Penguin Software Inc.
#
# This software may be distributed under the terms of the GNU General
# Public License, Version 2, or (at your option) any later version.
#
# LIC: GPL
#
# $Id: Makefile,v 1.2 2007-09-11 08:03:14 steven Exp $
#***********************************************************************

VERSION=0.4

OBJS=auth.o debug.o dgram.o main.o md5.o network.o options.o peer.o session.o tunnel.o utils.o
EXTRA_LIBS=libevent/*.o -ldl

SRCS=$(OBJS:.o=.c)
CFLAGS=-I.. -Ilibevent -Wall -Wstrict-prototypes -ansi -pedantic -D_GNU_SOURCE -DVERSION=\"$(VERSION)\" 

TARGETS=l2tpd libl2tp.a handlers

all: $(TARGETS)

libl2tp.a: $(OBJS)
	rm -f $@
	$(AR) rc $@ $^
	$(RANLIB) $@ || true

l2tpd: libl2tp.a libevent/libevent.a
	$(MAKE) -C handlers
	$(CC) -o l2tpd -rdynamic $(OBJS) $(EXTRA_LIBS)

libevent/libevent.a:
	test -d libevent || ln -s ../libevent .
	$(MAKE) -C libevent

%.o: %.c
	$(CC) -c -o $@ $(CFLAGS) $<

depend: .depend

.depend: $(SRCS)
	$(CC) -M $(CFLAGS) $^ > .depend

handlers:
	$(MAKE) -C handlers

clean:
	rm -f *.o *.a *~
	rm -f l2tpd
	$(MAKE) -C handlers clean
	$(MAKE) -C libevent clean

romfs: all
	$(MAKE) -C handlers romfs
	$(ROMFSINST) /sbin/l2tpd 
	$(ROMFSINST) /sbin/l2tp.sh

