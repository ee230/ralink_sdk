/* $Header: /home/cvsroot/RT288x_SDK/source/user/watchdog-5.6/src/memory.c,v 1.1 2009-10-14 06:32:06 steven Exp $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include "extern.h"
#include "watch_err.h"

#if USE_SYSLOG
#include <syslog.h>
#endif

#define FREEMEM		"MemFree:"
#define FREESWAP	"SwapFree:"

int check_memory(void)
{
    char buf[1024], *ptr1, *ptr2;
    unsigned int free;

    /* is the memory file open? */
    if (mem == -1)
	return (ENOERR);

    /* position pointer at start of file */
    if (lseek(mem, 0, SEEK_SET) < 0) {
	int err = errno;

#if USE_SYSLOG
	syslog(LOG_ERR, "lseek /proc/meminfo gave errno = %d = '%m'", err);
#else				/* USE_SYSLOG */
	perror(progname);
#endif				/* USE_SYSLOG */
	if (softboot)
	    return (err);

	return (ENOERR);
    }
    
    /* read the file */
    if (read(mem, buf, sizeof(buf)) < 0) {
	int err = errno;

#if USE_SYSLOG
	syslog(LOG_ERR, "read /proc/meminfo gave errno = %d = '%m'", err);
#else				/* USE_SYSLOG */
	perror(progname);
#endif				/* USE_SYSLOG */
	if (softboot)
	    return (err);

	return (ENOERR);
    }
    
    ptr1 = strstr(buf, FREEMEM);
    ptr2 = strstr(buf, FREESWAP);
    
    if (!ptr1 || !ptr2) {
#if USE_SYSLOG
	syslog(LOG_ERR, "/proc/meminfo contains invalid data (read = %s)", buf);
#else				/* USE_SYSLOG */
	perror(progname);
#endif				/* USE_SYSLOG */
	if (softboot)
	    return (EINVMEM);

	return (ENOERR);
    }

    /* we only care about integer values */
    free = atoi(ptr1+strlen(FREEMEM)) + atoi(ptr2+strlen(FREESWAP));

#if USE_SYSLOG
    if (verbose && logtick && ticker == 1)
	syslog(LOG_INFO, "currently there are %d kB of free memory available", free);
#endif				/* USE_SYSLOG */

    if (free < minpages * (EXEC_PAGESIZE / 1024)) {
#if USE_SYSLOG
	syslog(LOG_ERR, "memory %d kB is less than %d pages", free, minpages);
#endif				/* USE_SYSLOG */
	return (ENOMEM);
    }
    
    return (ENOERR);
}
