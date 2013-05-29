/* $Header: /home/cvsroot/RT288x_SDK/source/user/watchdog-5.6/src/temp.c,v 1.1 2009-10-14 06:32:06 steven Exp $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <unistd.h>
#include "extern.h"
#include "watch_err.h"

#if USE_SYSLOG
#include <syslog.h>
#endif

int check_temp(void)
{
    unsigned char temperature;
#if USE_SYSLOG
    int templevel1 = maxtemp * 9 / 10, have1 = FALSE;
    int templevel2 = maxtemp * 95 / 100, have2 = FALSE;
    int templevel3 = maxtemp * 98 / 100, have3 = FALSE;
#endif				/* USE_SYSLOG */

    /* is the temperature device open? */
    if (temp == -1)
	return (ENOERR);

    /* read the line (there is only one) */
    if (read(temp, &temperature, sizeof(temperature)) < 0) {
	int err = errno;

#if USE_SYSLOG
	syslog(LOG_ERR, "read %s gave errno = %d = '%m'", tempname, err);
#else				/* USE_SYSLOG */
	perror(progname);
#endif				/* USE_SYSLOG */
	if (softboot)
	    return (err);

	return (ENOERR);
    }
#if USE_SYSLOG
    if (verbose && logtick && ticker == 1)
	syslog(LOG_INFO, "current temperature is %d", temperature);

    if (temperature > templevel3) {
	if (!have3) {
	    /* once we reach level3, issue a warning */
	    syslog(LOG_WARNING, "temperature increases %d", templevel3);
	    have1 = have2 = have3 = TRUE;
	}
    } else {
	have3 = FALSE;
	if (temperature > templevel2) {
	    if (!have2) {
		/* once we reach level2, issue a warning */
		syslog(LOG_WARNING, "temperature increases %d", templevel2);
		have1 = have2 = TRUE;
	    }
	} else {
	    have2 = have3 = FALSE;
	    if (temperature > templevel1) {
		if (!have1) {
		    /* once we reach level1, issue a warning */
		    syslog(LOG_WARNING, "temperature increases %d", templevel1);
		    have1 = TRUE;
		}
	    }
	}
    }
#endif				/* USE_SYSLOG */

    if (temperature >= maxtemp) {
#if USE_SYSLOG
	syslog(LOG_ERR, "it is too hot inside (temperature = %d)", temperature);
#else				/* USE_SYSLOG */
	fprintf(stderr, "%s: it is too hot inside (temperature = %d\n", progname, temperature);
#endif				/* USE_SYSLOG */

	return (ETOOHOT);
    }
    return (ENOERR);
}
