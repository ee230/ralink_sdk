CFLAGS= -c -DHAVE_CONFIG_H -DSYSCONFDIR=\"/var/zebra/\" -DMULTIPATH_NUM=1 -I. -I.. -I$(ROOTDIR)/lib/libzebra -Wall
LDFLAGS= -L$(ROOTDIR)/lib/libzebra -lzebra -lcrypt

SOURCES=connected.c   if_proc.c    ipforward_proc.c  irdp_packet.c   rtadv.c           zebra_rib.c   zserv.c\
		debug.c       interface.c  irdp_interface.c  main.c          rt_netlink.c      zebra_snmp.c\
		if_netlink.c  ioctl.c      irdp_main.c       redistribute.c  rtread_netlink.c  zebra_vty.c


OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=zebra

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS)  $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o *.a *.so
	rm -f $(EXECUTABLE)

romfs:
	$(ROMFSINST) $(EXECUTABLE) /bin/$(EXECUTABLE)
