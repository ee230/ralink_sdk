# Comment out line bellow if you don't want to use 
# libwrap (hosts.{allow,deny} access control)
#LIBWRAP = -lwrap

# If you don't have it in /var/log/subsys, uncomment and define
CFLAGS += -DLOCKFILE_DIR=\"/var/log\"

# GNU target string
CROSS = 

CFLAGS += -O2  -Wall

ifneq ("$(LIBWRAP)", "")
LIBWRAP += -DUSE_LIBWRAP
endif

PROG = p910nd

$(PROG): p910nd.c
	$(CC) -o $@ $^ $(CFLAGS) $(LIBWRAP)

romfs:
	$(ROMFSINST) /sbin/$(PROG)

clean:
	rm -f *.o $(PROG)
