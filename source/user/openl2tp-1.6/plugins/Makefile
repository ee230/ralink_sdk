CFLAGS	:= $(CFLAGS.optimize) -MMD -Wall \
	-isystem $(KERNEL_BLDDIR)/include -isystem $(KERNEL_SRCDIR)/include \
	-I. -I.. -I../usl -fPIC $(EXTRA_CFLAGS)
LDFLAGS	:= -shared

PLUGINS.c:=	ppp_unix.c ppp_null.c ipsec.c event_sock.c

ifneq (,$(findstring -DL2TP_TEST,$(CFLAGS)))
PLUGINS.c+=	ppp_test.c
endif

PLUGINS.so=	$(PLUGINS.c:%.c=%.so)

SRC.generated=

.PHONY:		all clean install TAGS generated-files

all:		generated-files $(PLUGINS.so)

clean:
		$(RM) $(PLUGINS.so) $(wildcard *.o) $(wildcard *.d) $(SRC.generated)

%.so: %.c
		$(CC) -o $@ $(LDFLAGS) $(CFLAGS) $<

romfs:	all
		$(ROMFSINST) /etc_ro/ppp/plugins/ppp_unix.so
#		$(ROMFSINST) /etc_ro/ppp/plugins/ppp_null.so
#		$(ROMFSINST) /etc_ro/ppp/plugins/ipsec.so
#		$(ROMFSINST) /etc_ro/ppp/plugins/event_sock.so

generated-files:	$(SRC.generated)

include $(wildcard *.d) /dev/null
