SSOBJ=ss.o ssfilter.o
LNSTATOBJ=lnstat.o lnstat_util.o

TARGETS=ss nstat ifstat rtacct lnstat 

include ../Config

all: $(TARGETS)

ss: $(SSOBJ) $(LIBUTIL)

nstat: nstat.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o nstat nstat.c -lm

ifstat: ifstat.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o ifstat ifstat.c $(LIBNETLINK) $(LIBUTIL) -lm

rtacct: rtacct.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o rtacct rtacct.c $(LIBNETLINK) $(LIBUTIL) -lm

#arpd: arpd.c
#	$(CC) $(CFLAGS) -I$(DBM_INCLUDE) $(LDFLAGS) -o arpd arpd.c $(LIBNETLINK) -ldb -lpthread

ssfilter.c: ssfilter.y
	bison ssfilter.y -o ssfilter.c

lnstat: $(LNSTATOBJ)

romfs: all
	$(ROMFSINST) -e CONFIG_USER_IPROUTE2_SS /bin/ss
	$(ROMFSINST) -e CONFIG_USER_IPROUTE2_NSTAT /bin/nstat
	$(ROMFSINST) -e CONFIG_USER_IPROUTE2_IFSTAT /bin/ifstat
	$(ROMFSINST) -e CONFIG_USER_IPROUTE2_RTACCT /bin/rtacct
	$(ROMFSINST) -e CONFIG_USER_IPROUTE2_LNSTAT /bin/lnstat

clean:
	rm -f *.o $(TARGETS) ssfilter.c
