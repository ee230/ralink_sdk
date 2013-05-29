/* $Header: /home/cvsroot/RT288x_SDK/source/user/watchdog-5.6/src/file_table.c,v 1.1 2009-10-14 06:32:06 steven Exp $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/syslog.h>

#include "extern.h"
#include "watch_err.h"

int check_file_table(void)
{
    int fd;

    /* open a file */
    fd = open("/proc/uptime", O_RDONLY);
    if (fd == -1) {
	int err = errno;

	if (err == ENFILE) {
	    /* we need a reboot if ENFILE is returned (file table overflow) */
#if USE_SYSLOG
	    syslog(LOG_ERR, "file table overflow detected!\n");
#endif				/* USE_SYSLOG */
	    return (ENFILE);
	} else {
#if USE_SYSLOG
	    errno = err;
	    syslog(LOG_ERR, "cannot open /proc/uptime (errno = %d = '%m')", err);
#else				/* USE_SYSLOG */
	    perror(progname);
#endif				/* USE_SYSLOG */

	    if (softboot)
		return (err);
	}
    } else {
	if (close(fd) < 0) {
	    int err = errno;

#if USE_SYSLOG
	    syslog(LOG_ERR, "close /proc/uptime gave errno = %d = '%m'", err);
#else				/* USE_SYSLOG */
	    perror(progname);
#endif				/* USE_SYSLOG */
	    if (softboot)
		return (err);
	}
    }

    return (ENOERR);
}
