CFLAGS += -I$(ROOTDIR)/lib/libusb-1.0.0
LDFLAGS += -lpthread -lusb

all: lsusb 

lsusb: lsusb.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS) $(LDFLAGS)
	
romfs:
	$(ROMFSINST) /bin/lsusb

clean:
	-rm -f lsusb *.o
