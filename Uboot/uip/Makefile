# include ../config.mk
include $(TOPDIR)/config.mk

CFLAGS += -Iunix/ -Iuip/ -Iapps/webserver

LIB	= libuip.a
APPS	= webserver

OS_OBJS  = unix/unix.o unix/clock-arch.o

APP_OBJS = apps/webserver/httpd.o apps/webserver/http-strings.o \
	   apps/webserver/httpd-fs.o apps/webserver/httpd-cgi.o

UIP_OBJS = uip/uip.o uip/uip_arp.o uip/uiplib.o uip/psock.o uip/timer.o \
	   uip/uip-neighbor.o

OBJS	= $(UIP_OBJS) $(APP_OBJS) $(OS_OBJS)

all: $(LIB)

$(LIB):	$(OBJS)
	$(AR) crv $@ $(OBJS)

#########################################################################

.depend:	Makefile $(OBJS:.o=.c)
		$(CC) -M $(CFLAGS) $(OBJS:.o=.c) > $@

sinclude .depend

#########################################################################


