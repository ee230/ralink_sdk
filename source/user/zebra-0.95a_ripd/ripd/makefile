CFLAGS= -c -DHAVE_CONFIG_H -DSYSCONFDIR=\"/var/zebra/\" -DMULTIPATH_NUM=1 -I. -I.. -I$(ROOTDIR)/lib/libzebra -Wall
LDFLAGS= -L$(ROOTDIR)/lib/libzebra -lzebra -lcrypt

SOURCES=ripd.c rip_debug.c rip_interface.c rip_main.c rip_offset.c rip_peer.c rip_routemap.c rip_snmp.c rip_zebra.c

OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=ripd

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
