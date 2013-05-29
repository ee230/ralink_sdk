#
# pppstats makefile
# $Id: Makefile,v 1.1 2007-08-22 05:54:13 steven Exp $
#

PPPSTATSRCS = pppstats.c
PPPSTATOBJS = pppstats.o

COPTS = -O
COMPILE_FLAGS = -I../include
LIBS =


CFLAGS = $(COPTS) $(COMPILE_FLAGS)

all: pppstats

romfs: pppstats
	$(ROMFSINST) /bin/pppstats

pppstats: $(PPPSTATSRCS)
	$(CC) $(CFLAGS) -o pppstats pppstats.c $(LIBS)

clean:
	rm -f pppstats *~ #* core

depend:
	cpp -M $(CFLAGS) $(PPPSTATSRCS) >.depend
#	makedepend $(CFLAGS) $(PPPSTATSRCS)
