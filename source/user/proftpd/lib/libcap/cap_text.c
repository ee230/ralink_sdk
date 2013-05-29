/*
 * $Id: cap_text.c,v 1.3 2009-11-13 06:05:55 steven Exp $
 *
 * Copyright (c) 1997-8 Andrew G Morgan <morgan@linux.kernel.org>
 * Copyright (c) 1997 Andrew Main <zefram@dcs.warwick.ac.uk>
 *
 * See end of file for Log.
 *
 * This file deals with exchanging internal and textual
 * representations of capability sets.
 */

#define LIBCAP_PLEASE_INCLUDE_ARRAY
#include "libcap.h"

#include <ctype.h>
#include <stdio.h>

/* Maximum output text length (16 per cap) */
#define CAP_TEXT_SIZE    (16*__CAP_BITS)

#define LIBCAP_EFF   01
#define LIBCAP_INH   02
#define LIBCAP_PER   04

/*
 * Parse a textual representation of capabilities, returning an internal
 * representation.
 */

#define setbits(A,B) _setbits((__cap_s *)A, (__cap_s *)B)
static void _setbits(__cap_s *a, __cap_s *b)
{
    int n;
    for (n = __CAP_BLKS; n--; )
	a->_blk[n] |= b->_blk[n];
}

#define clrbits(A,B) _clrbits((__cap_s *)A, (__cap_s *)B)
static void _clrbits(__cap_s *a, __cap_s *b)
{
    int n;
    for (n = __CAP_BLKS; n--; )
	a->_blk[n] &= ~b->_blk[n];
}

static char const *namcmp(char const *str, char const *nam)
{
    while (*nam && tolower((unsigned char)*str) == *nam) {
	str++;
	nam++;
    }
    if (*nam || isalnum((unsigned char)*str) || *str == '_')
	return NULL;
    return str;
}

static int lookupname(char const **strp)
{
    char const *str = *strp;
    if (isdigit(*str)) {
	unsigned long n = strtoul(str, (char **)&str, 0);
	if (n >= __CAP_BITS)
	    return -1;
	*strp = str;
	return n;
    } else {
	char const *s;
	int n;
	for (n = __CAP_BITS; n--; )
	    if (_cap_names[n] && (s = namcmp(str, _cap_names[n]))) {
		*strp = s;
		return n;
	    }
	return -1;
    }
}

cap_t cap_from_text(const char *str)
{
    cap_t res;
    __cap_s allones;
    int n;

    if (str == NULL) {
	_cap_debug("bad argument");
	errno = EINVAL;
	return NULL;
    }

    if (!(res = cap_init()))
	return NULL;
    for (n = __CAP_BLKS; n--; )
	allones._blk[n] = -1;
    _cap_debug("%s", str);

    for (;;) {
	char op;
	int flags = 0, listed=0;
	__cap_s list = {{0}};

	/* skip leading spaces */
	while (isspace((unsigned char)*str))
	    str++;
	if (!*str) {
	    _cap_debugcap("e = ", &res->set.effective);
	    _cap_debugcap("i = ", &res->set.inheritable);
	    _cap_debugcap("p = ", &res->set.permitted);
	    return res;
	}

	/* identify caps specified by this clause */
	if (isalnum((unsigned char)*str) || *str == '_') {
	    for (;;) {
		if (namcmp(str, "all")) {
		    str += 3;
		    list = allones;
		} else {
		    n = lookupname(&str);
		    if (n == -1)
			goto bad;
		    list.raise_cap(n);
		}
		if (*str != ',')
		    break;
		if (!isalnum((unsigned char)*++str) && *str != '_')
		    goto bad;
	    }
	    listed = 1;
	} else if (*str == '+' || *str == '-')
	    goto bad;                    /* require a list of capabilities */
	else
	    list = allones;

	/* identify first operation on list of capabilities */
	op = *str++;
	if (op == '=' && (*str == '+' || *str == '-')) {
	    if (!listed)
		goto bad;
	    op = (*str++ == '+' ? 'P':'M'); /* skip '=' and take next op */
	} else if (op != '+' && op != '-' && op != '=')
	    goto bad;

	/* cycle through list of actions */
	do {
	    _cap_debug("next char = `%c'", *str);
	    if (*str && !isspace(*str)) {
		switch (*str++) {    /* Effective, Inheritable, Permitted */
		case 'e':
		    flags |= LIBCAP_EFF;
		    break;
		case 'i':
		    flags |= LIBCAP_INH;
		    break;
		case 'p':
		    flags |= LIBCAP_PER;
		    break;
		default:
		    goto bad;
		}
	    } else if (op != '=') {
		_cap_debug("only '=' can be followed by space");
		goto bad;
	    }

	    _cap_debug("how to read?");
	    switch (op) {               /* how do we interpret the caps? */
	    case '=':
	    case 'P':                                              /* =+ */
	    case 'M':                                              /* =- */
		clrbits(&res->set.effective,   &list);
		clrbits(&res->set.inheritable, &list);
		clrbits(&res->set.permitted,   &list);
		/* fall through */
		if (op == 'M')
		    goto minus;
	    case '+':
		if (flags & LIBCAP_EFF)
		    setbits(&res->set.effective,   &list);
		if (flags & LIBCAP_INH)
		    setbits(&res->set.inheritable, &list);
		if (flags & LIBCAP_PER)
		    setbits(&res->set.permitted,   &list);
		break;
	    case '-':
	    minus:
	        if (flags & LIBCAP_EFF)
		    clrbits(&res->set.effective,   &list);
		if (flags & LIBCAP_INH)
		    clrbits(&res->set.inheritable, &list);
		if (flags & LIBCAP_PER)
		    clrbits(&res->set.permitted,   &list);
		break;
	    }

	    /* new directive? */
	    if (*str == '+' || *str == '-') {
		if (!listed) {
		    _cap_debug("for + & - must list capabilities");
		    goto bad;
		}
		flags = 0;                       /* reset the flags */
		op = *str++;
		if (!isalpha(*str))
		    goto bad;
	    }
	} while (*str && !isspace(*str));
	_cap_debug("next clause");
    }

bad:
    cap_free(&res);
    errno = EINVAL;
    return NULL;
}

/*
 * Convert an internal representation to a textual one. The textual
 * representation is stored in static memory. It will be overwritten
 * on the next occasion that this function is called.
 */

static int getstateflags(cap_t caps, int capno)
{
    int f = 0;

    if (isset_cap((__cap_s *)(&caps->set.effective),capno))
	f |= LIBCAP_EFF;
    if (isset_cap((__cap_s *)(&caps->set.inheritable),capno))
	f |= LIBCAP_INH;
    if (isset_cap((__cap_s *)(&caps->set.permitted),capno))
	f |= LIBCAP_PER;

    return f;
}

#define CAP_TEXT_BUFFER_ZONE 100

char *cap_to_text(cap_t caps, ssize_t *length_p)
{
    static char buf[CAP_TEXT_SIZE+CAP_TEXT_BUFFER_ZONE];
    char *p;
    int histo[8] = {0};
    int m, n, t;

    /* Check arguments */
    if (!good_cap_t(caps)) {
	errno = EINVAL;
	return NULL;
    }

    _cap_debugcap("e = ", &caps->set.effective);
    _cap_debugcap("i = ", &caps->set.inheritable);
    _cap_debugcap("p = ", &caps->set.permitted);

    for (n = __CAP_BITS; n--; )
	histo[getstateflags(caps, n)]++;

    for (m=t=7; t--; )
	if (histo[t] > histo[m])
	    m = t;

    /* blank is not a valid capability set */
    p = sprintf(buf, "=%s%s%s",
		(m & LIBCAP_EFF) ? "e" : "",
		(m & LIBCAP_INH) ? "i" : "",
		(m & LIBCAP_PER) ? "p" : "" ) + buf;

    for (t = 8; t--; )
	if (t != m && histo[t]) {
	    *p++ = ' ';
	    for (n = 0; n != __CAP_BITS; n++)
		if (getstateflags(caps, n) == t) {
		    if (_cap_names[n])
			p += sprintf(p, "%s,", _cap_names[n]);
		    else
			p += sprintf(p, "%d,", n);
		    if (p - buf > CAP_TEXT_SIZE) {
			errno = ERANGE;
			return NULL;
		    }
		}
	    p--;
	    n = t & ~m;
	    if (n)
		p += sprintf(p, "+%s%s%s",
			     (n & LIBCAP_EFF) ? "e" : "",
			     (n & LIBCAP_INH) ? "i" : "",
			     (n & LIBCAP_PER) ? "p" : "");
	    n = ~t & m;
	    if (n)
		p += sprintf(p, "-%s%s%s",
			     (n & LIBCAP_EFF) ? "e" : "",
			     (n & LIBCAP_INH) ? "i" : "",
			     (n & LIBCAP_PER) ? "p" : "");
	    if (p - buf > CAP_TEXT_SIZE) {
		errno = ERANGE;
		return NULL;
	    }
	}

    _cap_debug("%s", buf);
    if (length_p) {
	*length_p = p - buf;
    }

    return (_libcap_strdup(buf));
}

/*
 * $Log: cap_text.c,v $
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
 * Revision 1.2  2003/05/15 00:49:13  castaglia
 *
 * Bug#2000 - mod_cap should not use bundled libcap.  This patch updates the
 * bundled libcap; I won't be closing the bug report just yet.
 *
 * Revision 1.1  2003/01/03 02:16:17  jwm
 *
 * Turning mod_linuxprivs into a core module, mod_cap. This is by no means
 * complete.
 *
 * Revision 1.3  2000/07/11 13:36:52  macgyver
 * Minor updates and buffer cleanups.
 *
 * Revision 1.2  1999/09/07 23:14:19  macgyver
 * Updated capabilities library and model.
 *
 * Revision 1.2  1999/04/17 23:25:09  morgan
 * fixes from peeterj
 *
 * Revision 1.1.1.1  1999/04/17 22:16:31  morgan
 * release 1.0 of libcap
 *
 * Revision 1.4  1998/05/24 22:54:09  morgan
 * updated for 2.1.104
 *
 * Revision 1.3  1997/05/04 05:37:00  morgan
 * case sensitvity to capability flags
 *
 * Revision 1.2  1997/04/28 00:57:11  morgan
 * zefram's replacement file with a number of bug fixes from AGM
 *
 * Revision 1.1  1997/04/21 04:32:52  morgan
 * Initial revision
 *
 */
