/* $Header: /home/cvsroot/RT288x_SDK/source/user/watchdog-5.6/src/file_stat.c,v 1.1 2009-10-14 06:32:06 steven Exp $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <time.h>
#include <sys/stat.h>
#include "extern.h"
#include "watch_err.h"

#if USE_SYSLOG
#include <syslog.h>
#endif

int check_file_stat(struct list *file)
{
    struct stat buf;

    /* in filemode stat file */
    if (stat(file->name, &buf) == -1) {
	int err = errno;

#if USE_SYSLOG
	syslog(LOG_ERR, "cannot stat %s (errno = %d = '%m')", file->name, err);
#else				/* USE_SYSLOG */
	perror(progname);
#endif				/* USE_SYSLOG */
	/* on error ENETDOWN|ENETUNREACH we react as if we're in ping mode */
	if (softboot || err == ENETDOWN || err == ENETUNREACH)
	    return (err);
    } else if (file->parameter.file.mtime != 0) {

#if USE_SYSLOG
	/* do verbose logging */
	if (verbose && logtick && ticker == 1)
	    syslog(LOG_INFO, "file %s was last changed at %s.", file->name, ctime(&buf.st_mtime));
#endif

	if (time(NULL) - buf.st_mtime > file->parameter.file.mtime) {
	    /* file wasn't changed often enough */
#if USE_SYSLOG
	    syslog(LOG_ERR, "file %s was not changed in %d seconds.", file->name, file->parameter.file.mtime);
#else				/* USE_SYSLOG */
	    fprintf(stderr, "file %s was not changed in %d seconds.", file->name, file->parameter.file.mtime);
#endif				/* USE_SYSLOG */
	    return (ENOCHANGE);
	}
    }
    return (ENOERR);
}
