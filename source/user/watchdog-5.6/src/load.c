/* $Header: /home/cvsroot/RT288x_SDK/source/user/watchdog-5.6/src/load.c,v 1.1 2009-10-14 06:32:06 steven Exp $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "extern.h"
#include "watch_err.h"

#if USE_SYSLOG
#include <syslog.h>
#endif

int check_load(void)
{
    int avg1, avg5, avg15;
    char buf[40], *ptr;

    /* is the load average file open? */
    if (load == -1 || maxload1 == 0 || maxload5 == 0 || maxload15 == 0)
	return (ENOERR);

    /* position pointer at start of file */
    if (lseek(load, 0, SEEK_SET) < 0) {
	int err = errno;

#if USE_SYSLOG
	syslog(LOG_ERR, "lseek /proc/loadavg gave errno = %d = '%m'", err);
#else				/* USE_SYSLOG */
	perror(progname);
#endif				/* USE_SYSLOG */
	if (softboot)
	    return (err);

	return (ENOERR);
    }

    /* read the line (there is only one) */
    if (read(load, buf, sizeof(buf)) < 0) {
	int err = errno;

#if USE_SYSLOG
	syslog(LOG_ERR, "read /proc/loadavg gave errno = %d = '%m'", err);
#else				/* USE_SYSLOG */
	perror(progname);
#endif				/* USE_SYSLOG */
	if (softboot)
	    return (err);

	return (ENOERR);
    }
    /* we only care about integer values */
    avg1 = atoi(buf);

    /* if we have incorrect data we might not be able to find */
    /* the blanks we're looking for */
    ptr = strchr(buf, ' ');
    if (ptr != NULL) {
	avg5 = atoi(ptr);
	ptr = strchr(ptr + 1, ' ');
    }
    if (ptr != NULL)
	avg15 = atoi(ptr);
    else {
#if USE_SYSLOG
	syslog(LOG_ERR, "/proc/loadavg does not contain any data (read = %s)", buf);
#else				/* USE_SYSLOG */
	perror(progname);
#endif				/* USE_SYSLOG */
	if (softboot)
	    return (ENOLOAD);

	return (ENOERR);
    }

#if USE_SYSLOG
    if (verbose && logtick && ticker == 1)
	syslog(LOG_INFO, "current load is %d %d %d", avg1, avg5, avg15);
#endif				/* USE_SYSLOG */

    if (avg1 > maxload1 || avg5 > maxload5 || avg15 > maxload15) {
#if USE_SYSLOG
	syslog(LOG_ERR, "loadavg %d %d %d is higher than the given threshold %d %d %d!", avg1, avg5, avg15,
	       maxload1, maxload5, maxload15);
#endif				/* USE_SYSLOG */
	return (EMAXLOAD);
    }

    return (ENOERR);
}
