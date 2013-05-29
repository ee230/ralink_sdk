DIRS = lib src modules #utils
OBJS += src/main.o src/timers.o src/sets.o src/pool.o src/table.o src/regexp.o src/dirtree.o src/support.o src/netaddr.o src/inet.o src/child.o src/parser.o src/log.o src/lastlog.o src/xferlog.o src/bindings.o src/netacl.o src/class.o src/scoreboard.o src/help.o src/feat.o src/netio.o src/response.o src/ident.o src/data.o src/modules.o src/display.o src/auth.o src/fsio.o src/mkhome.o src/ctrls.o src/event.o src/var.o src/trace.o src/utf8.o src/proctitle.o src/pidfile.o src/env.o modules/mod_core.o modules/mod_xfer.o modules/mod_auth_unix.o modules/mod_auth_file.o modules/mod_auth.o modules/mod_ls.o modules/mod_log.o modules/mod_site.o modules/mod_delay.o modules/mod_cap.o modules/module_glue.o 
LDLIBS = -Llib -Llib/libcap

all: 
	for i in $(DIRS) ; do $(MAKE) -C $$i $@ || exit $?; done 
	$(CC) -o proftpd $(OBJS) $(LDFLAGS) $(LDLIBS) -lcap -lsupp -lcrypt
	
romfs:
	for i in $(DIRS) ; do $(MAKE) -C $$i $@ || exit $?; done
	$(ROMFSINST) /bin/proftpd
	$(ROMFSINST) /bin/proftpd.sh

clean:
	for i in $(DIRS) ; do $(MAKE) -C $$i $@ || exit $?; done
	-rm -f proftpd

