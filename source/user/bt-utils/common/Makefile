CFLAGS=-DHAVE_CONFIG_H -Icommon -I$(ROOTDIR)/lib/libbluetooth/include -Wall
LDFLAGS += -L$(ROOTDIR)/lib/libbluetooth
LDLIBS += -lbluetooth 

OBJS = oui.o textfile.o logging.o sdp-xml.o #glib-helper.o

libhelper.a: $(OBJS)
	$(AR) cru $@ $(OBJS)
	$(RANLIB) $@

clean:
	rm -fr *.o  *.a

romfs: 

