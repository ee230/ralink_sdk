#	$Id: Makefile,v 1.1 2008-11-26 08:37:43 michael Exp $

CDEF1=	-DTERMIOS			# Use the termios structure
CDEF2=	-DSIGTYPE=void			# Standard definition
CDEF3=	-UNO_SLEEP			# Use the usleep function
CDEF4=	-DFNDELAY=O_NDELAY		# Old name value
CDEFS=	$(CDEF1) $(CDEF2) $(CDEF3) $(CDEF4)

COPTS=	-O2 -g -pipe $(CDEFS)
CFLAGS=	$(COPTS) $(CDEFS)

INSTALL= install

all:	chat

chat:	chat.o
	$(CC) -o chat chat.o

chat.o:	chat.c
	$(CC) -c $(CFLAGS) -o chat.o chat.c

install: chat
	mkdir -p $(BINDIR)
	$(INSTALL) -s -c chat $(BINDIR)
	$(INSTALL) -c -m 644 chat.8 $(MANDIR)/man8

romfs: chat
	$(ROMFSINST) /bin/chat
clean:
	rm -f chat.o chat *~
