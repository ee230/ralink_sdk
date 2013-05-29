
#
#
# Makefile for the linux version of stupid-ftpd
#
#
#


OBJS=ftpcommand.o ftpdconfig.o command.o ls.o stupid-ftpd.o
DOBJS=ftpcommand.do ftpdconfig.do command.do ls.do stupid-ftpd.do
POBJS=ftpcommand.po ftpdconfig.po command.po ls.po stupid-ftpd.po
LIBS=
CFLAGS=-O2 -Wall -Wstrict-prototypes
DCFLAGS=-g -Wall -Wstrict-prototypes
PCFLAGS=-g -Wall -Wstrict-prototypes -Wcast-align -Wwrite-strings -Wconversion -Waggregate-return -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls -Wnested-externs
EXEC=stupid-ftpd

.SUFFIXES: .c .o .do .po

all: $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS) $(LIBS)

debug: $(DOBJS)
	$(CC) $(DCFLAGS) -o $(EXEC) $(DOBJS) $(LIBS)

pedantic: $(POBJS)
	$(CC) $(PCFLAGS) -o $(EXEC) $(POBJS) $(LIBS)

clean:
	rm -f $(OBJS) $(DOBJS) $(POBJS) $(EXEC) *~

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

.c.do:
	$(CC) $(DCFLAGS) -c -o $@ $<

.c.po:
	$(CC) $(PCFLAGS) -c -o $@ $<


romfs: all
	$(ROMFSINST) /sbin/stupid-ftpd
	$(ROMFSINST) /sbin/stupid-ftpd-acl.sh  
	$(ROMFSINST) /sbin/stupid-ftpd-common.sh
	$(ROMFSINST) /sbin/stupid-ftpd-user.sh
