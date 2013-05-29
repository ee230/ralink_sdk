sbin_PROGRAMS = watchdog wd_keepalive

watchdog_SOURCES = file_stat.c file_table.c fstab.c iface.c ifdown.c keep_alive.c \
			load.c lomount.c memory.c mntent.c mount.c net.c nfsmount.c \
			nfsmount_clnt.c nfsmount_xdr.c pidfile.c shutdown.c sundries.c \
			temp.c test_binary.c umount.c version.c watchdog.c

wd_keepalive_SOURCES = wd_keepalive.c

INCLUDES = -I@top_srcdir@/include

distclean-depend:
	rm -rf .deps
