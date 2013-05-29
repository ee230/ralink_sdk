TOPDIR=../
include $(TOPDIR)Rules.mak

LIBUPNP=libupnp.a
LIBIXML=libixml.a
LIBTHREADUTIL=libthreadutil.a
CFLAGS += -I$(ROOTDIR)/lib/include -I$(ROOTDIR)/lib/libupnp -I$(ROOTDIR)/lib/libupnp/threadutil/inc 
CFLAGS += -I$(ROOTDIR)/lib/libupnp/ixml/inc -I$(ROOTDIR)/lib/libupn/upnp/inc

all: $(LIBUPNP) #$(LIBIXML) $(LIBTHREADUTIL)

shared: all
	$(MAKE) -C ixml shared
	$(MAKE) -C threadutil shared
	$(MAKE) -C upnp shared


$(LIBUPNP): 
	$(MAKE) -C upnp

$(LIBIXML): 
	$(MAKE) -C ixml

$(LIBTHREADUTIL): 
	$(MAKE) -C threadutil

romfs:
	$(MAKE) -C ixml romfs
	$(MAKE) -C threadutil romfs
	$(MAKE) -C upnp romfs
	
$(OBJS): Makefile

clean:
	$(MAKE) -C ixml clean
	$(MAKE) -C threadutil clean
	$(MAKE) -C upnp clean

