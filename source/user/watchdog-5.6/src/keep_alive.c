/* $Header: /home/cvsroot/RT288x_SDK/source/user/watchdog-5.6/src/keep_alive.c,v 1.1 2009-10-14 06:32:06 steven Exp $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <sys/syslog.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#include "extern.h"
#include "watch_err.h"

/* write a heartbeat file */
int write_heartbeat(void)
{
    time_t timenow;
    struct tm *tm;
    char tbuf[TS_SIZE + 1];
    char tbufw[TS_SIZE + 1];

    if (hb == NULL)
	    return (ENOERR);

    /* MJ 16/2/2001 keep a rolling buffer in a file of writes to the
       watchdog device, any gaps in this will indicate a reboot */

    timenow = time(NULL);
    if (timenow != -1) {
        tm = gmtime(&timenow);
        /* Get the seconds since seconds since 00:00:00, Jan 1, 1970 */
        strftime(tbuf, TS_SIZE - 1, "%s", tm);         
        /* Make it the right width */ 
        sprintf(tbufw, "%*s\n", TS_SIZE - 1, tbuf);
        /* copy it to the buffer */
        memcpy(timestamps + (lastts * TS_SIZE), tbufw, TS_SIZE);

        // success
        if (nrts < hbstamps) 
            nrts++;
        ++lastts;
        lastts = lastts % hbstamps;
        
        // write the buffer to the file
        rewind(hb);
        if (nrts == hbstamps) {
            // write from the logical start of the buffer to the physical end
            if (fwrite(timestamps + (lastts * TS_SIZE), TS_SIZE, hbstamps - lastts, hb) == 0) {
		    int err = errno;
#if USE_SYSLOG
		    syslog(LOG_ERR, "write heartbeat file gave error %d = '%m'!", err);
#else			/* USE_SYSLOG */
		    perror(progname);
#endif			/* USE_SYSLOG */
	    }
	    
            // write from the physical start of the buffer to the logical end
            if (fwrite(timestamps, TS_SIZE, lastts, hb) == 0) {
		    int err = errno;
#if USE_SYSLOG
		    syslog(LOG_ERR, "write heartbeat file gave error %d = '%m'!", err);
#else			/* USE_SYSLOG */
		    perror(progname);
#endif			/* USE_SYSLOG */
	    }
        }
        else {        
            // write from the physical start of the buffer to the logical end
            if (fwrite(timestamps, TS_SIZE, nrts, hb) == 0) {
		    int err = errno;
#if USE_SYSLOG
		    syslog(LOG_ERR, "write heartbeat file gave error %d = '%m'!", err);
#else			/* USE_SYSLOG */
		    perror(progname);
#endif			/* USE_SYSLOG */
	    }
        }
        fflush(hb);   
    } 
    return(ENOERR);
}

/* write to the watchdog device */
int keep_alive(void)
{
    if (watchdog == -1)
	return (ENOERR);

    if (write(watchdog, "\0", 1) < 0) {
	int err = errno;

#if USE_SYSLOG
	syslog(LOG_ERR, "write watchdog device gave error %d = '%m'!", err);
#else			/* USE_SYSLOG */
	perror(progname);
#endif			/* USE_SYSLOG */
	if (softboot)
	    return (err);
    }
    
    /* MJ 20/2/2001 write a heartbeat to a file outside the syslog, because:
       - there is no guarantee the system logger is up and running
       - easier and quicker to parse checkpoint information */
    write_heartbeat();
    
    return(ENOERR);
}
