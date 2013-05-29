# If you want to debug daemon, add following line

cfg_filename = dhcp-fwd.conf
AM_CPPFLAGS = -DCFG_FILENAME='"/bin/$(cfg_filename)"'

CFLAGS +=  -DDBG -DHAVE_CONFIG_H  -I.  $(AM_CPPFLAGS) 

OBJS =	src/parser.o src/main.o src/cfg.o src/recvfromflags.o src/output.o 

all: dhcp-fwd

dhcp-fwd: $(OBJS)
	$(CC) -o dhcpfwd  $(OBJS) 
romfs:
	$(ROMFSINST) /bin/dhcpfwd
	$(ROMFSINST) dhcp-fwd.conf /bin/dhcp-fwd.conf

clean:
	rm -f dhcpfwd *.elf *.gdb *.o src/*.o
