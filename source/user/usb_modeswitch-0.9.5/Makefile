LIBDIR = $(ROOTDIR)/lib/libusb-0.1.12
CFLAGS += -I$(LIBDIR) 
LDLIBS += -lusb
EXEC = usb_modeswitch
SRCS = usb_modeswitch.c

$(EXEC): $(SRCS)
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

romfs:
	$(ROMFSINST) /bin/$(EXEC)
	$(ROMFSINST) conf /etc_ro/usb

clean:
	-rm -f $(EXEC) *.o

