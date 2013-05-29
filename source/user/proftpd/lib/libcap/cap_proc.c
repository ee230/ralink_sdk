/*
 * $Id: cap_proc.c,v 1.3 2009-11-13 06:05:55 steven Exp $
 *
 * Copyright (c) 1997-8 Andrew G Morgan <morgan@linux.kernel.org>
 *
 * See end of file for Log.
 *
 * This file deals with setting capabilities on processes.
 */

#include "libcap.h"

cap_t cap_get_proc(void)
{
    cap_t result;

    /* allocate a new capability set */
    result = cap_init();
    if (result) {
	_cap_debug("getting current process' capabilities");

	/* fill the capability sets via a system call */
	if (capget(&result->head, &result->set)) {
	    cap_free(&result);
	}
    }

    return result;
}

int cap_set_proc(cap_t cap_d)
{
    int retval;

    if (!good_cap_t(cap_d)) {
	errno = EINVAL;
	return -1;
    }

    _cap_debug("setting process capabilities");
    retval = capset(&cap_d->head, &cap_d->set);

    cap_d->head.version = _LINUX_CAPABILITY_VERSION;
    return retval;
}

/* the following two functions are not required by POSIX */

/* read the caps on a specific process */

int capgetp(pid_t pid, cap_t cap_d)
{
    int error;

    if (!good_cap_t(cap_d)) {
	errno = EINVAL;
	return -1;
    }

    _cap_debug("getting process capabilities for proc %d", pid);

    cap_d->head.pid = pid;
    error = capget(&cap_d->head, &cap_d->set);
    cap_d->head.version = _LINUX_CAPABILITY_VERSION;
    cap_d->head.pid = 0;

    return error;
}

/* set the caps on a specific process/pg etc.. */

int capsetp(pid_t pid, cap_t cap_d)
{
    int error;

    if (!good_cap_t(cap_d)) {
	errno = EINVAL;
	return -1;
    }

    _cap_debug("setting process capabilities for proc %d", pid);
    cap_d->head.pid = pid;
    error = capset(&cap_d->head, &cap_d->set);
    cap_d->head.version = _LINUX_CAPABILITY_VERSION;
    cap_d->head.pid = 0;

    return error;
}

/*
 * $Log: cap_proc.c,v $
 * Revision 1.3  2009-11-13 06:05:55  steven
 *
 * automatically CVS: CVS: Committing in . CVS: CVS: Added Files: CVS: .autom4te.cfg .sgbuilt_user COPYING CREDITS ChangeLog INSTALL
 * README.Unixware README.capabilities README.classes CVS: README.controls README.cygwin README.facl README.modules CVS: README.ports
 * RELEASE_NOTES acconfig.h aclocal.m4 config.guess CVS: config.h.in ltmain.sh proftpd.conf proftpd.spec stamp-h CVS: stamp-h.in
 * config/ftp.webnas config/proftpd.nvramconfig CVS: contrib/INSTALL.mod_wrap contrib/README CVS: contrib/README.mod_wrap
 * contrib/README.ratio contrib/ftpasswd CVS: contrib/ftpquota contrib/mod_ban.c contrib/mod_ctrls_admin.c CVS: contrib/mod_ifsession.c
 * contrib/mod_ldap.c CVS: contrib/mod_quotatab.c contrib/mod_quotatab.h CVS: contrib/mod_quotatab_file.c contrib/mod_quotatab_ldap.c
 * contrib/mod_rewrite.c contrib/mod_site_misc.c CVS: contrib/mod_sql.c contrib/mod_sql.h contrib/mod_sql_mysql.c CVS:
 * contrib/mod_sql_postgres.c contrib/mod_tls.c CVS: contrib/mod_wrap.c contrib/mod_wrap2_file.c CVS: contrib/mod_wrap2_sql.c
 * contrib/xferstats.holger-preiss CVS: contrib/dist/rpm/ftp.pamd contrib/dist/rpm/proftpd.init.d CVS:
 * contrib/dist/rpm/proftpd.init.d.suse CVS: contrib/dist/rpm/proftpd.logrotate contrib/dist/rpm/xinetd CVS:
 * contrib/mod_load/Makefile.in contrib/mod_load/config.guess CVS: contrib/mod_load/config.sub contrib/mod_load/configure CVS:
 * contrib/mod_load/configure.in contrib/mod_load/getloadavg.c CVS: contrib/mod_load/install-sh contrib/mod_load/mod_load.c.in CVS:
 * contrib/mod_load/mod_load.h.in contrib/mod_wrap2/Makefile.in CVS: contrib/mod_wrap2/configure contrib/mod_wrap2/configure.in CVS:
 * contrib/mod_wrap2/install-sh contrib/mod_wrap2/mod_wrap2.c CVS: contrib/mod_wrap2/mod_wrap2.h.in doc/Configuration.html CVS:
 * doc/Configuration.pdf doc/Configuration.sgml doc/faq.html CVS: doc/license.txt doc/mod_sample.c doc/contrib/ftpasswd.html CVS:
 * doc/contrib/ftpquota.html doc/contrib/index.html CVS: doc/contrib/mod_ban.html doc/contrib/mod_ctrls_admin.html CVS:
 * doc/contrib/mod_ifsession.html doc/contrib/mod_load.html CVS: doc/contrib/mod_quotatab.html CVS: doc/contrib/mod_quotatab_file.html
 * doc/contrib/mod_radius.html CVS: doc/contrib/mod_rewrite.html doc/contrib/mod_site_misc.html CVS: doc/contrib/mod_sql.html
 * doc/contrib/mod_tls.html CVS: doc/contrib/mod_wrap.html doc/contrib/mod_wrap2.html CVS: doc/contrib/mod_wrap2_file.html
 * doc/contrib/mod_wrap2_sql.html CVS: doc/howto/ASCII.html doc/howto/AuthFiles.html CVS: doc/howto/Authentication.html
 * doc/howto/BCP.html CVS: doc/howto/Chroot.html doc/howto/Classes.html CVS: doc/howto/ConfigFile.html doc/howto/Controls.html CVS:
 * doc/howto/CreateHome.html doc/howto/DNS.html CVS: doc/howto/DSO.html doc/howto/Debugging.html CVS: doc/howto/Directory.html
 * doc/howto/DisplayFiles.html CVS: doc/howto/FTP.html doc/howto/FXP.html doc/howto/Filters.html CVS: doc/howto/Globbing.html
 * doc/howto/Limit.html CVS: doc/howto/ListOptions.html doc/howto/Logging.html CVS: doc/howto/NAT.html doc/howto/Quotas.html
 * doc/howto/Regex.html CVS: doc/howto/SQL.html doc/howto/SSH.html CVS: doc/howto/Scoreboard.html doc/howto/ServerType.html CVS:
 * doc/howto/Stopping.html doc/howto/TLS.html CVS: doc/howto/Timestamps.html doc/howto/Tracing.html CVS: doc/howto/Umask.html
 * doc/howto/Upgrade.html CVS: doc/howto/Vhost.html doc/howto/VirtualUsers.html CVS: doc/howto/index.html doc/modules/mod_auth_file.html
 * doc/modules/mod_dso.html doc/modules/mod_facl.html CVS: doc/modules/mod_lang.html doc/rfc/draft-bonachea-sftp-00.txt CVS:
 * doc/rfc/draft-ietf-ftpext-mlst-15.txt CVS: doc/rfc/draft-ietf-ftpext-sec-consider-02.txt CVS: doc/rfc/rfc0959.txt doc/rfc/rfc2228.txt
 * doc/rfc/rfc2389.txt CVS: doc/rfc/rfc2428.txt doc/rfc/rfc4217.txt include/Makefile CVS: include/Makefile.in include/auth.h
 * include/bindings.h CVS: include/buildstamp.h include/child.h include/class.h CVS: include/compat.h include/conf.h include/config.h
 * include/event.h include/feat.h include/fsio.h include/ftp.h CVS: include/glibc-glob.h include/help.h include/ident.h CVS:
 * include/inet.h include/install-sh include/libsupp.h CVS: include/log.h include/mkhome.h include/mod_ctrls.h CVS: include/modules.h
 * include/netacl.h include/netaddr.h CVS: include/netio.h include/options.h include/parser.h CVS: include/pidfile.h include/pool.h
 * include/pr-syslog.h CVS: include/privs.h include/proctitle.h include/proftpd.h CVS: include/regexp.h include/response.h
 * include/scoreboard.h CVS: include/sets.h include/support.h include/table.h CVS: include/timers.h include/trace.h include/utf8.h
 * include/var.h CVS: include/version.h include/xferlog.h lib/Makefile CVS: lib/Makefile.in lib/getopt.c lib/getopt.h lib/getopt1.c CVS:
 * lib/glibc-gai_strerror.c lib/glibc-glob.c CVS: lib/glibc-hstrerror.c lib/glibc-mkstemp.c lib/pr-syslog.c CVS: lib/pr_fnmatch.c
 * lib/pr_fnmatch_loop.c lib/pwgrent.c CVS: lib/sstrncpy.c lib/strsep.c lib/vsnprintf.c CVS: lib/libcap/Makefile
 * lib/libcap/Makefile.orig CVS: lib/libcap/_makenames.c lib/libcap/cap_alloc.c CVS: lib/libcap/cap_alloc.o lib/libcap/cap_extint.c CVS:
 * lib/libcap/cap_extint.o lib/libcap/cap_file.c CVS: lib/libcap/cap_flag.c lib/libcap/cap_flag.o CVS: lib/libcap/cap_names.h
 * lib/libcap/cap_names.sed CVS: lib/libcap/cap_proc.c lib/libcap/cap_proc.o CVS: lib/libcap/cap_sys.c lib/libcap/cap_sys.o CVS:
 * lib/libcap/cap_text.c lib/libcap/cap_text.o CVS: lib/libcap/libcap.a lib/libcap/libcap.h CVS: lib/libcap/include/sys/capability.h
 * lib/libltdl/COPYING.LIB CVS: lib/libltdl/Makefile.am lib/libltdl/Makefile.in CVS: lib/libltdl/README lib/libltdl/acinclude.m4 CVS:
 * lib/libltdl/aclocal.m4 lib/libltdl/config-h.in CVS: lib/libltdl/config.guess lib/libltdl/config.sub CVS: lib/libltdl/configure
 * lib/libltdl/configure.ac CVS: lib/libltdl/install-sh lib/libltdl/ltdl.c lib/libltdl/ltdl.h CVS: lib/libltdl/ltmain.sh
 * lib/libltdl/missing locale/Makefile CVS: locale/Makefile.in locale/files.txt locale/install-sh CVS: modules/Makefile
 * modules/Makefile.in modules/mod_auth.c CVS: modules/mod_auth_file.c modules/mod_auth_pam.c CVS: modules/mod_auth_unix.c
 * modules/mod_cap.c modules/mod_core.c CVS: modules/mod_ctrls.c modules/mod_delay.c modules/mod_dso.c CVS: modules/mod_facl.c
 * modules/mod_lang.c modules/mod_log.c CVS: modules/mod_ls.c modules/mod_site.c modules/mod_xfer.c CVS: modules/module_glue.c
 * sample-configurations/PFTEST.conf.in CVS: sample-configurations/PFTEST.group CVS: sample-configurations/PFTEST.install CVS:
 * sample-configurations/PFTEST.passwd CVS: sample-configurations/PFTEST.shadow CVS: sample-configurations/anonymous.conf CVS:
 * sample-configurations/basic.conf CVS: sample-configurations/complex-virtual.conf CVS: sample-configurations/mod_sql.conf CVS:
 * sample-configurations/virtual.conf src/Makefile src/auth.c CVS: src/bindings.c src/child.c src/class.c src/ctrls.c src/data.c CVS:
 * src/dirtree.c src/display.c src/env.c src/event.c src/feat.c CVS: src/fsio.c src/ftpdctl.8 src/ftpdctl.8.in src/ftpdctl.c CVS:
 * src/help.c src/ident.c src/inet.c src/lastlog.c src/log.c CVS: src/main.c src/mkhome.c src/modules.c src/netacl.c CVS: src/netaddr.c
 * src/netio.c src/parser.c src/pidfile.c CVS: src/pool.c src/proctitle.c src/proftpd.8 src/proftpd.8.in CVS: src/regexp.c
 * src/response.c src/scoreboard.c src/sets.c CVS: src/support.c src/table.c src/timers.c src/trace.c src/utf8.c CVS: src/var.c
 * src/xferlog.5 src/xferlog.5.in src/xferlog.c CVS: utils/Makefile utils/ftpcount.1 utils/ftpcount.c CVS: utils/ftpshut.8
 * utils/ftpshut.c utils/ftptop.1 utils/ftptop.c CVS: utils/ftpwho.1 utils/ftpwho.c utils/misc.c utils/scoreboard.c CVS: utils/utils.h
 *
 * Revision 1.1  2003/01/03 02:16:17  jwm
 *
 * Turning mod_linuxprivs into a core module, mod_cap. This is by no means
 * complete.
 *
 * Revision 1.2  1999/09/07 23:14:19  macgyver
 * Updated capabilities library and model.
 *
 * Revision 1.2  1999/04/18 20:50:01  morgan
 * reliable behavior when trying to talk with a kernel that has a more
 * modern capability implementation than the one the library was compiled
 * with.
 *
 * Revision 1.1.1.1  1999/04/17 22:16:31  morgan
 * release 1.0 of libcap
 *
 * Revision 1.5  1998/05/24 22:54:09  morgan
 * updated for 2.1.104
 *
 * Revision 1.4  1997/05/14 05:17:13  morgan
 * bug-fix from zefram (errno no set on success)
 *
 * Revision 1.3  1997/05/04 05:35:46  morgan
 * fixed errno setting. syscalls do this part
 *
 * Revision 1.2  1997/04/28 00:57:11  morgan
 * fixes and zefram's patches
 *
 * Revision 1.1  1997/04/21 04:32:52  morgan
 * Initial revision
 *
 */
