/* $Header: /home/cvsroot/RT288x_SDK/source/user/watchdog-5.6/src/iface.c,v 1.1 2009-10-14 06:32:06 steven Exp $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "extern.h"
#include "watch_err.h"

#if USE_SYSLOG
#include <syslog.h>
#endif

#define NETDEV_LINE_LEN	128

int check_iface(struct list *dev)
{
    FILE *file = fopen ("/proc/net/dev", "r");
    
    if (file == NULL) {
        int err = errno;
        
#if USE_SYSLOG
        syslog(LOG_ERR, "cannot open /proc/net/dev (errno = %d = '%m')", err);
#else                           /* USE_SYSLOG */
        perror(progname);
#endif                          /* USE_SYSLOG */

        if (softboot)
		return (err);

	return(ENOERR);
    }

    /* read the file line by line */
    while (!feof(file)) {
	char line[NETDEV_LINE_LEN];
        
        if (fgets(line, CONFIG_LINE_LEN, file) == NULL) {
		if (!ferror(file))
		        break;
	        else {
	        	int err = errno;
#if USE_SYSLOG
		        syslog(LOG_ERR, "cannot read /proc/net/dev (errno = %d = '%m')", err);
#else                           /* USE_SYSLOG */
		        perror(progname);
#endif                          /* USE_SYSLOG */

			fclose(file);
		        if (softboot)
				return (err);

			return(ENOERR);
	        }
	} else {
		int i = 0;
			
		for (; line[i] == ' ' || line[i] == '\t'; i++);
		if (strncmp(line + i, dev->name, strlen(dev->name)) == 0) {
			unsigned long bytes = strtoul(line + i + strlen(dev->name) + 1, NULL, 10);
			
#if USE_SYSLOG
			/* do verbose logging */
			if (verbose && logtick && ticker == 1)
		            syslog(LOG_INFO, "device %s received %lu bytes", dev->name, bytes);
#endif   

			if (dev->parameter.iface.bytes == bytes) {
				fclose(file);
#if USE_SYSLOG
		            	syslog(LOG_ERR, "device %s did not receive anything since last check", dev->name);
#endif   

				return (ENETUNREACH);
			}
			else
				dev->parameter.iface.bytes = bytes;
		}
        }
    }

    if (fclose(file) != 0) {
        int err = errno;
        
#if USE_SYSLOG
        syslog(LOG_ERR, "cannot close /proc/net/dev (errno = %d = '%m')", err);
#else                           /* USE_SYSLOG */
        perror(progname);
#endif                          /* USE_SYSLOG */

        if (softboot)
		return (err);

	return(ENOERR);
    }

    return (ENOERR);
}
