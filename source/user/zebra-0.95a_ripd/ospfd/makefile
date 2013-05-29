TOPDIR= ../../../
CFLAGS= -c -DHAVE_CONFIG_H -DSYSCONFDIR=\"/var/zebra/\" -DMULTIPATH_NUM=1 -I. -I.. -I$(TOPDIR)lib/libzebra -Wall
LDFLAGS= -L$(TOPDIR)/lib/libzebra -lzebra -lcrypt

SOURCES= ospf_abr.c ospfd.c ospf_ia.c ospf_lsa.c ospf_neighbor.c ospf_opaque.c ospf_routemap.c ospf_te.c \
ospf_asbr.c ospf_dump.c ospf_interface.c ospf_lsdb.c ospf_network.c ospf_packet.c ospf_snmp.c ospf_vty.c \
ospf_ase.c ospf_flood.c ospf_ism.c ospf_main.c ospf_nsm.c ospf_route.c ospf_spf.c ospf_zebra.c

OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=ospfd

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS)  $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o
	rm -f $(EXECUTABLE)

romfs:
	$(ROMFSINST) $(EXECUTABLE) /bin/$(EXECUTABLE)
