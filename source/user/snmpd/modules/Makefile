OBJS = icmp_vars.o ip_vars.o tcp_vars.o udp_vars.o systm.o rt_wireless.o ralink_esw.o

LIBRARY = libmodules.a
CFLAGS += -I../snmplib

all: $(LIBRARY)

test:

romfs:

$(LIBRARY): $(OBJS)
	$(AR) rcv $@ $(OBJS)
	-$(RANLIB) $@

clean:
	$(RM) $(OBJS) $(LIBRARY)
