EXE = watchdog

CFLAGS += -I../include/ -DHAVE_CONFIG_H

all: $(EXE)

watchdog: wd_keepalive.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

romfs:
	$(ROMFSINST) /sbin/watchdog

clean:
	rm -f *.o $(EXE)
