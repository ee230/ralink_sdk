ifndef ROOTDIR
ROOTDIR = ../..
export ROOTDIR
endif

LIB_PATH = $(ROOTDIR)/lib/lib
INCLUDES=  -I$(ROOTDIR)/lib/libupnp/ixml/inc -I$(ROOTDIR)/lib/libupnp/upnp/inc -I$(ROOTDIR)/lib/libupnp/threadutil/inc
LIBS= -L$(LIB_PATH) -lpthread -lupnp -lixml -lthreadutil
FILES= main.o gatedevice.o pmlist.o util.o config.o

CFLAGS += -Wall -g -O2
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/net/raeth 

ifdef HAVE_LIBIPTC
ifdef LIBIPTC_PREFIX
LIBS += -L$(LIBIPTC_PREFIX)/lib
INCLUDES += -I$(LIBIPTC_PREFIX)/include
endif

LIBS += -liptc
INCLUDES += -DHAVE_LIBIPTC
FILES += iptc.o
endif

all: upnpd

upnpd: $(FILES)
	$(CC) $(CFLAGS) $(FILES) $(LIBS) -o $@
	$(STRIP) -s $@
	@echo "make $@ finished on `date`"

%.o:	%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

romfs:
	$(ROMFSINST) /bin/upnpd
	$(ROMFSINST) /bin/upnp_xml.sh
	$(ROMFSINST) etc/upnpd.conf /etc_ro/linuxigd/upnpd.conf
	$(ROMFSINST) etc/ligd.gif /etc_ro/linuxigd/ligd.gif

clean:
	rm -f *.o upnpd
