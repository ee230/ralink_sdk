# Generated automatically from Makefile.in by configure.
# $Id: Makefile,v 1.1.1.1 2007-09-11 05:58:19 steven Exp $
# Makefile for LNS handlers
# LIC: GPL

HANDLERS=sync-pppd.so cmd.so

OBJS=pty.o sync-pppd.o dstring.o
SRCS=$(OBJS:.o=.c)
CFLAGS=-O2 -I.. -I../libevent -I../../libevent -Wall

all: $(HANDLERS) l2tp-control

l2tp-control: cmd-control.o
	$(CC) -o l2tp-control cmd-control.o

cmd-control.o: cmd-control.c
	$(CC) -c -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<

sync-pppd.so: pty.o sync-pppd.o
	$(CC) -shared -o $@ $^

cmd.so: cmd.o dstring.o
	$(CC) -shared -o $@ $^

clean:
	rm -f *.so *.o *~
	rm -f l2tp-control

romfs: all
	$(ROMFSINST) /sbin/l2tp-control
	$(ROMFSINST) /etc_ro/ppp/plugins/sync-pppd.so
	$(ROMFSINST) /etc_ro/ppp/plugins/cmd.so
