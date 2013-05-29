NAME = dnsmasq
OBJS = cache.o rfc1035.o util.o option.o forward.o isc.o network.o \
       dnsmasq.o dhcp.o lease.o rfc2131.o netlink.o dbus.o bpf.o \
       helper.o tftp.o log.o

CFLAGS = -O2 -D__uClinux__ -DNO_GETTEXT

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(EXTRALIBS) $(LDLIBS)
	$(STRIP) --remove-section=.note --remove-section=.comment $@

romfs:
	$(ROMFSINST) /bin/$(NAME)

clean:
	rm -f *~ *.mo *~ *.pot 
	rm -f *.o dnsmasq.a dnsmasq core */core
