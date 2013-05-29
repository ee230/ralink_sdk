EXEC = ftpcount ftpshut ftptop ftpwho ftpdctl
OBJS = ftpcount.o scoreboard.o ftpshut.o ftptop.o ftpwho.o misc.o
CFLAGS += -DHAVE_CONFIG_H -DLINUX -I../include 
LDFLAGS += -L../lib -L../lib/libcap

all: $(EXEC)

ftpcount: ftpcount.o scoreboard.o
	$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS) -lsupp -lcrypt

ftpshut: ftpshut.o
	$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS) -lsupp -lcrypt

ftpdctl: ../src/ftpdctl.o ../src/pool.o ../src/netaddr.o ../src/log.o ../src/ctrls.o
	$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS) -lcap -lsupp -lcrypt

ftptop: ftptop.o scoreboard.o
	$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS) -lncurses -lsupp -lcrypt

ftpwho: ftpwho.o scoreboard.o misc.o
	$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS) -lsupp -lcrypt

romfs:
	$(ROMFSINST) /bin/ftpcount
	$(ROMFSINST) /bin/ftpshut
	$(ROMFSINST) /bin/ftpdctl
	$(ROMFSINST) /bin/ftptop
	$(ROMFSINST) /bin/ftpwho

clean:
	rm -f *.o $(EXEC)

