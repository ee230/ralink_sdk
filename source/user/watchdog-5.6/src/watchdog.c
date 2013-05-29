/* $Header: /home/cvsroot/RT288x_SDK/source/user/watchdog-5.6/src/watchdog.c,v 1.1 2009-10-14 06:32:06 steven Exp $ */

/*************************************************************/
/* Original version was an example in the kernel source tree */
/*                                                           */
/* Most of the rest was written by me, Michael Meskes        */
/* meskes@debian.org                                         */
/*                                                           */
/*************************************************************/
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "extern.h"

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <netdb.h>
#include <sched.h>
#include <signal.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "watchdog.h"
#define __USE_GNU
#include <string.h>

#if !defined(__GLIBC__)		/* __GLIBC__ */
extern char *basename(const char *);
#endif				/* __GLIBC__ */

#include <unistd.h>

#include "watch_err.h"

#if USE_SYSLOG
#include <syslog.h>
#endif				/* USE_SYSLOG */

static int no_act = FALSE;

#if USE_SYSLOG
int verbose = FALSE;
#endif				/* USE_SYSLOG */

volatile sig_atomic_t _running = 1;

#define ADMIN		"admin"
#define CHANGE		"change"
#define DEVICE		"watchdog-device"
#define DEVICE_TIMEOUT	"watchdog-timeout"
#define	FILENAME	"file"
#define INTERFACE	"interface"
#define INTERVAL	"interval"
#define LOGTICK		"logtick"
#define MAXLOAD1	"max-load-1"
#define MAXLOAD5	"max-load-5"
#define MAXLOAD15	"max-load-15"
#define MAXTEMP		"max-temperature"
#define MINMEM		"min-memory"
#define SERVERPIDFILE	"pidfile"
#define PING		"ping"
#define PINGCOUNT	"ping-count"
#define PRIORITY	"priority"
#define REALTIME	"realtime"
#define REPAIRBIN	"repair-binary"
#define TEMP		"temperature-device"
#define TESTBIN		"test-binary"
#define TESTTIMEOUT	"test-timeout"
#define HEARTBEAT	"heartbeat-file"
#define HBSTAMPS	"heartbeat-stamps"

pid_t pid;
int softboot = FALSE, watchdog = -1, load = -1, mem = -1, temp = -1;
int tint = 10, logtick = 1, ticker = 1, schedprio = 1;
int maxload1 = 0, maxload5 = 0, maxload15 = 0, minpages = 0;
int maxtemp = 120, hbstamps = 300, lastts, nrts;
int pingcount = 3;
int devtimeout = TIMER_MARGIN;
char *tempname = NULL, *devname = NULL, *admin = "root", *progname;
char *timestamps, *heartbeat;
time_t timeout = 0;
FILE *hb;

#if defined(_POSIX_MEMLOCK)
int mlocked = FALSE, realtime = FALSE;
#endif

static void usage(void)
{
    fprintf(stderr, "%s version %d.%d, usage:\n", progname, MAJOR_VERSION, MINOR_VERSION);
#if USE_SYSLOG
    fprintf(stderr, "%s [-f] [-c <config_file>] [-v] [-s] [-b] [-q]\n", progname);
#else				/* USE_SYSLOG */
    fprintf(stderr, "%s [-f] [-c <config_file>] [-s] [-b] [-q]\n", progname);
#endif				/* USE_SYSLOG */
    exit(1);
}

/* Try to sync */
static int sync_system(int sync_it)
{
    if (sync_it) {
	sync();
	sync();
    }
    return (0);
}

/* execute repair binary */
static int repair(char *rbinary, int result, char *name)
{
    pid_t child_pid;
    char parm[5];
    int ret;

    /* no binary given, we have to reboot */
    if (rbinary == NULL)
	return (result);

    sprintf(parm, "%d", result);

    child_pid = fork();
    if (!child_pid) {
	/* Don't want the stdin and stdout of our test program
	 * to cause trouble
	 * So make stdout and stderr go to their respective files */
        if (!freopen("/var/log/watchdog/repair-bin.stdout", "a+", stdout))
        	exit (errno);
        if (!freopen("/var/log/watchdog/repair-bin.stderr", "a+", stderr))
        	exit (errno);

        /* else start binary */
	if (name == NULL)
		execl(rbinary, rbinary, parm, NULL);
	else
		execl(rbinary, rbinary, parm, name, NULL);

	/* execl should only return in case of an error */
	/* so we return the reboot code */
	return (errno);
    } else if (child_pid < 0) {	/* fork failed */
	int err = errno;

	if (errno == EAGAIN) {	/* process table full */
#if USE_SYSLOG
	    syslog(LOG_ERR, "process table is full!");
#endif				/* USE_SYSLOG */
	    return (EREBOOT);
	} else if (softboot)
	    return (err);
	else
	    return (ENOERR);
    }
    if (waitpid(child_pid, &result, 0) != child_pid) {
	int err = errno;

#if USE_SYSLOG
	syslog(LOG_ERR, "child %d does not exist (errno = %d = '%m')", child_pid, err);
#else				/* USE_SYSLOG */
	perror(progname);
#endif				/* USE_SYSLOG */
	if (softboot)
	    return (err);
    }
    
    /* check result */
    ret = WEXITSTATUS(result);
    if (ret != 0) {
#if USE_SYSLOG
	syslog(LOG_ERR, "repair binary returned %d", ret);
#endif				/* USE_SYSLOG */

	if (ret == ERESET) /* repair script says force hard reset, we give it a try */
		sleep(devtimeout * 4);
	
	/* for all other errors or if we still live, we let shutdown handle it */
	return (ret);
    }
    
    return (ENOERR);
}

static void wd_action(int result, char *rbinary, char *name)
{
    /* if no-action flag set, do nothing */
    /* no error, keep on working */
    if (result == ENOERR || no_act == TRUE) {
	printf("case 1\n");
	return;
    }

    /* error that might be repairable */
    if (result != EREBOOT) {
	printf("case 2\n");
	result = repair(rbinary, result, name);
    }

    /* if still error, reboot */
    if (result != ENOERR) {
	printf("case 3\n");
	do_shutdown(result);
    }

}

static void do_check(int res, char *rbinary, char *name)
{
    wd_action(res, rbinary, name);
    wd_action(keep_alive(), rbinary, NULL);
}

struct list *file = NULL, *target = NULL, *pidfile = NULL, *iface = NULL;
char *tbinary, *rbinary, *admin;

static void add_list(struct list **list, char *name)
{
    struct list *new, *act;

    if ((new = (struct list *) calloc(1, sizeof(struct list))) == NULL) {
	fprintf(stderr, "%s: out of memory\n", progname);
	exit(1);
    }
    new->name = name;
    memset((char *) (&(new->parameter)), '\0', sizeof(union wdog_options));

    if (*list == NULL)
	*list = new;
    else {
	for (act = *list; act->next != NULL; act = act->next);
	act->next = new;
    }
}

static int spool(char *line, int *i, int offset)
{
    for ((*i) += offset; line[*i] == ' ' || line[*i] == '\t'; (*i)++);
    if (line[*i] == '=')
	(*i)++;
    for (; line[*i] == ' ' || line[*i] == '\t'; (*i)++);
    if (line[*i] == '\0')
    	return(1);
    else
    	return(0);
}

static void read_config(char *filename, char *progname)
{
    FILE *wc;
    int gotload5 = FALSE, gotload15 = FALSE;

    if ((wc = fopen(filename, "r")) == NULL) {
	perror(progname);
	exit(1);
    }

    while (!feof(wc)) {
	char line[CONFIG_LINE_LEN];

	if (fgets(line, CONFIG_LINE_LEN, wc) == NULL) {
	    if (!ferror(wc))
		break;
	    else {
		perror(progname);
		exit(1);
	    }
	} else {
	    int i, j;

	    /* scan the actual line for an option */
	    /* first remove the leading blanks */
	    for (i = 0; line[i] == ' ' || line[i] == '\t'; i++);

	    /* if the next sign is a '#' we have a comment */
	    if (line[i] == '#')
		continue;

	    /* also remove the trailing blanks and the \n */
	    for (j = strlen(line) - 1; line[j] == ' ' || line[j] == '\t' || line[j] == '\n'; j--);
	    line[j + 1] = '\0';

	    /* if the line is empty now, we don't have to parse it */
	    if (strlen(line + i) == 0)
		continue;

	    /* now check for an option */
	    if (strncmp(line + i, FILENAME, strlen(FILENAME)) == 0) {
		if (spool(line, &i, strlen(FILENAME)))
		    fprintf(stderr, "Ignoring invalid line in config file:\n%s\n", line);
		else
		    add_list(&file, strdup(line + i));
	    } else if (strncmp(line + i, CHANGE, strlen(CHANGE)) == 0) {
		struct list *ptr;

		if (spool(line, &i, strlen(CHANGE)))
		    continue;

		if (!file) {	/* no file entered yet */
		    fprintf(stderr, "Ignoring invalid line in config file:\n%s\n", line);
		    continue;
		}
		for (ptr = file; ptr->next != NULL; ptr = ptr->next);
		if (ptr->parameter.file.mtime != 0)
		    fprintf(stderr, "Duplicate change interval option in config file. Ignoring first entry.\n");

		ptr->parameter.file.mtime = atoi(line + i);
	    } else if (strncmp(line + i, SERVERPIDFILE, strlen(SERVERPIDFILE)) == 0) {
		if (spool(line, &i, strlen(SERVERPIDFILE)))
		    fprintf(stderr, "Ignoring invalid line in config file:\n%s\n", line);
		else
		    add_list(&pidfile, strdup(line + i));
	    } else if (strncmp(line + i, PINGCOUNT, strlen(PINGCOUNT)) == 0) {
		if (spool(line, &i, strlen(PINGCOUNT)))
		    fprintf(stderr, "Ignoring invalid line in config file:\n%s\n", line);
		else
		    pingcount = atol(line + i);
	    } else if (strncmp(line + i, PING, strlen(PING)) == 0) {
		if (spool(line, &i, strlen(PING)))
		    fprintf(stderr, "Ignoring invalid line in config file:\n%s\n", line);
		else
		    add_list(&target, strdup(line + i));
	    } else if (strncmp(line + i, INTERFACE, strlen(INTERFACE)) == 0) {
		if (spool(line, &i, strlen(INTERFACE)))
		    fprintf(stderr, "Ignoring invalid line in config file:\n%s\n", line);
		else
		    add_list(&iface, strdup(line + i));
	    } else if (strncmp(line + i, REALTIME, strlen(REALTIME)) == 0) {
		(void)spool(line, &i, strlen(REALTIME));
		realtime = (strncmp(line + i, "yes", 3) == 0) ? TRUE : FALSE;
	    } else if (strncmp(line + i, PRIORITY, strlen(PRIORITY)) == 0) {
		if (spool(line, &i, strlen(PRIORITY)))
		    fprintf(stderr, "Ignoring invalid line in config file:\n%s\n", line);
		else
		    schedprio = atol(line + i);
	    } else if (strncmp(line + i, REPAIRBIN, strlen(REPAIRBIN)) == 0) {
		if (spool(line, &i, strlen(REPAIRBIN)))
			rbinary = NULL;
		else
			rbinary = strdup(line + i);
	    } else if (strncmp(line + i, TESTBIN, strlen(TESTBIN)) == 0) {
		if (spool(line, &i, strlen(TESTBIN)))
			tbinary = NULL;
		else
			tbinary = strdup(line + i);
	    } else if (strncmp(line + i, TESTTIMEOUT, strlen(TESTTIMEOUT)) == 0) {
		if (spool(line, &i, strlen(TESTTIMEOUT)))
			timeout = 0;
		else
			timeout = atol(line + i);
	    } else if (strncmp(line + i, HEARTBEAT, strlen(HEARTBEAT)) == 0) {
		if (spool(line, &i, strlen(HEARTBEAT)))
			heartbeat = NULL;
		else
			heartbeat = strdup(line + i);
	    } else if (strncmp(line + i, HBSTAMPS, strlen(HBSTAMPS)) == 0) {
		if (spool(line, &i, strlen(HBSTAMPS)))
		        fprintf(stderr, "Ignoring invalid line in config file:\n%s\n", line);
		else
			hbstamps = atol(line + i);
	    } else if (strncmp(line + i, ADMIN, strlen(ADMIN)) == 0) {
		if (spool(line, &i, strlen(ADMIN)))
			admin = NULL;
		else
			admin = strdup(line + i);
	    } else if (strncmp(line + i, INTERVAL, strlen(INTERVAL)) == 0) {
		if (spool(line, &i, strlen(INTERVAL)))
		    fprintf(stderr, "Ignoring invalid line in config file:\n%s\n", line);
		else
		    tint = atol(line + i);
	    } else if (strncmp(line + i, LOGTICK, strlen(LOGTICK)) == 0) {
		if (spool(line, &i, strlen(LOGTICK)))
		    logtick = ticker = 1;
		else
		    logtick = ticker = atol(line + i);
	    } else if (strncmp(line + i, DEVICE, strlen(DEVICE)) == 0) {
		if (spool(line, &i, strlen(DEVICE)))
			devname = NULL;
		else
			devname = strdup(line + i);
	    } else if (strncmp(line + i, DEVICE_TIMEOUT, strlen(DEVICE_TIMEOUT)) == 0) {
		if (spool(line, &i, strlen(DEVICE_TIMEOUT)))
			fprintf(stderr, "Ignoring invalid line in config file: %s ", line);
		else
			devtimeout = atol(line + i);
	    } else if (strncmp(line + i, TEMP, strlen(TEMP)) == 0) {
		if (spool(line, &i, strlen(TEMP)))
			tempname = NULL;
		else
			tempname = strdup(line + i);
	    } else if (strncmp(line + i, MAXTEMP, strlen(MAXTEMP)) == 0) {
		if (spool(line, &i, strlen(MAXTEMP)))
			fprintf(stderr, "Ignoring invalid line in config file:\n%s\n", line);
		else
			maxtemp = atol(line + i);
	    } else if (strncmp(line + i, MAXLOAD15, strlen(MAXLOAD15)) == 0) {
		if (spool(line, &i, strlen(MAXLOAD15)))
			fprintf(stderr, "Ignoring invalid line in config file:\n%s\n", line);
		else {
			maxload15 = atol(line + i);
			gotload15 = TRUE;
		}
	    } else if (strncmp(line + i, MAXLOAD1, strlen(MAXLOAD1)) == 0) {
		if (spool(line, &i, strlen(MAXLOAD1)))
			fprintf(stderr, "Ignoring invalid line in config file:\n%s\n", line);
		else {
			maxload1 = atol(line + i);
			if (!gotload5)
			    maxload5 = maxload1 * 3 / 4;
			if (!gotload15)
			    maxload15 = maxload1 / 2;
		}
	    } else if (strncmp(line + i, MAXLOAD5, strlen(MAXLOAD5)) == 0) {
		if (spool(line, &i, strlen(MAXLOAD5)))
			fprintf(stderr, "Ignoring invalid line in config file:\n%s\n", line);
		else {
			maxload5 = atol(line + i);
			gotload5 = TRUE;
		}
	    } else if (strncmp(line + i, MINMEM, strlen(MINMEM)) == 0) {
		if (spool(line, &i, strlen(MINMEM)))
			fprintf(stderr, "Ignoring invalid line in config file:\n%s\n", line);
		else {
			minpages = atol(line + i);
		}
	    } else {
		fprintf(stderr, "Ignoring invalid line in config file:\n%s\n", line);
	    }
	}
    }

    if (fclose(wc) != 0) {
	perror(progname);
	exit(1);
    }
}

static void old_option(int c, char *filename)
{
    fprintf(stderr, "Option -%c is no longer valid, please specify it in %s.\n", c, filename);
    usage();
}

int main(int argc, char *const argv[])
{
    FILE *fp;
    int c, force = FALSE, sync_it = FALSE;
    int hold;
    char *filename = CONFIG_FILENAME;
    struct list *act;
    pid_t child_pid;

#if USE_SYSLOG
    char *opts = "d:i:n:fsvbql:p:t:c:r:m:a:";
    struct option long_options[] =
    {
	{"config-file", required_argument, NULL, 'c'},
	{"force", no_argument, NULL, 'f'},
	{"sync", no_argument, NULL, 's'},
	{"no-action", no_argument, NULL, 'q'},
	{"verbose", no_argument, NULL, 'v'},
	{"softboot", no_argument, NULL, 'b'},
	{NULL, 0, NULL, 0}
    };
    long count = 0L;
#else				/* USE_SYSLOG */
    char *opts = "d:i:n:fsbql:p:t:c:r:m:a:";
    struct option long_options[] =
    {
	{"config-file", required_argument, NULL, 'c'},
	{"force", no_argument, NULL, 'f'},
	{"sync", no_argument, NULL, 's'},
	{"no-action", no_argument, NULL, 'q'},
	{"softboot", no_argument, NULL, 'b'},
	{NULL, 0, NULL, 0}
    };
#endif				/* USE_SYSLOG */

    progname = basename(argv[0]);

    /* check the options */
    /* there aren't that many any more */
    while ((c = getopt_long(argc, argv, opts, long_options, NULL)) != EOF) {
	if (c == -1)
	    break;

	switch (c) {
	case 'n':
	case 'p':
	case 'a':
	case 'r':
	case 'd':
	case 't':
	case 'l':
	case 'm':
	case 'i':
	    old_option(c, filename);
	    break;
	case 'c':
	    filename = optarg;
	    break;
	case 'f':
	    force = TRUE;
	    break;
	case 's':
	    sync_it = TRUE;
	    break;
	case 'b':
	    softboot = TRUE;
	    break;
	case 'q':
	    no_act = TRUE;
	    break;
#if USE_SYSLOG
	case 'v':
	    verbose = TRUE;
	    break;
#endif				/* USE_SYSLOG */
	default:
	    usage();
	}
    }

    read_config(filename, progname);

    if (tint < 0)
	usage();

    if (tint >= devtimeout && !force) {
	fprintf(stderr, "%s error:\n", progname);
	fprintf(stderr, "This interval length might reboot the system while the process sleeps!\n");
	fprintf(stderr, "To force this interval length use the -f option.\n");
	exit(1);
    }
    
    if (maxload1 > 0 && maxload1 < MINLOAD && !force) {
	fprintf(stderr, "%s error:\n", progname);
	fprintf(stderr, "Using this maximal load average might reboot the system to often!\n");
	fprintf(stderr, "To force this load average use the -f option.\n");
	exit(1);
    }
   
    /* make sure we get our own directory in /var/log */
    if (mkdir ("/var/log/watchdog", 0750) && errno != EEXIST) {
	fprintf(stderr, "%s error:\n", progname);
        fprintf(stderr, "Cannot create directory /var/log/watchdog\n");
	exit (1);
    }

    /* set up pinging if in ping mode */
    if (target != NULL) {
	for (act = target; act != NULL; act = act->next) {
	    struct protoent *proto;
	    struct pingmode *net = (struct pingmode *) calloc(1, sizeof(struct pingmode));

	    if (net == NULL) {
		fprintf(stderr, "%s: out of memory\n", progname);
		exit(1);
	    }
	    /* setup the socket */
	    memset(&(net->to), 0, sizeof(struct sockaddr));

	    ((struct sockaddr_in *) &(net->to))->sin_family = AF_INET;
	    if ((((struct sockaddr_in *) &(net->to))->sin_addr.s_addr = inet_addr(act->name)) == (unsigned int) -1) {
		(void) fprintf(stderr, "%s: unknown host %s\n", progname, act->name);
		exit(1);
	    }
	    if (!(net->packet = (unsigned char *) malloc((unsigned int) (DATALEN + MAXIPLEN + MAXICMPLEN)))) {
		fprintf(stderr, "%s: out of memory\n", progname);
		exit(1);
	    }
	    if (!(proto = getprotobyname("icmp"))) {
		(void) fprintf(stderr, "%s: unknown protocol icmp.\n", progname);
		exit(1);
	    }
	    if ((net->sock_fp = socket(AF_INET, SOCK_RAW, proto->p_proto)) < 0) {
		perror(progname);
		exit(1);
	    }
	    
	    /* this is necessary for broadcast pings to work */
	    (void) setsockopt(net->sock_fp, SOL_SOCKET, SO_BROADCAST, (char *)&hold, sizeof(hold));
    
	    hold = 48 * 1024;
	    (void) setsockopt(net->sock_fp, SOL_SOCKET, SO_RCVBUF, (char *) &hold,
			      sizeof(hold));

	    act->parameter.net = *net;
	}
    }
    
    /* make sure we're on the root partition */
    if (chdir("/") < 0) {
	perror(progname);
	exit(1);
    }
#if !defined(DEBUG)
    /* fork to go into the background */
    if ((child_pid = fork()) < 0) {
	perror(progname);
	exit(1);
    } else if (child_pid > 0) {
	/* fork was okay          */
	/* wait for child to exit */
	if (waitpid(child_pid, NULL, 0) != child_pid) {
	    perror(progname);
	    exit(1);
	}
	/* and exit myself */
	exit(0);
    }
    /* and fork again to make sure we inherit all rights from init */
    if ((child_pid = fork()) < 0) {
	perror(progname);
	exit(1);
    } else if (child_pid > 0)
	exit(0);
#endif				/* !DEBUG */

    /* now we're free */
#if USE_SYSLOG
#if !defined(DEBUG)
    /* Okay, we're a daemon     */
    /* but we're still attached to the tty */
    /* create our own session */
    setsid();

    /* with USE_SYSLOG we don't do any console IO */
    close(0);
    close(1);
    close(2);
#endif				/* !DEBUG */

    /* Log the starting message */
    openlog(progname, LOG_PID, LOG_DAEMON);
    syslog(LOG_INFO, "starting daemon (%d.%d):", MAJOR_VERSION, MINOR_VERSION);
    syslog(LOG_INFO, "int=%ds realtime=%s sync=%s soft=%s mla=%d mem=%d",
	    tint,
	    realtime ? "yes" : "no",
	    sync_it ? "yes" : "no",
	    softboot ? "yes" : "no",
	    maxload1, minpages);
	    
    if (target == NULL)
            syslog(LOG_INFO, "ping: no machine to check");
    else
            for (act = target; act != NULL; act = act->next)
            	syslog(LOG_INFO, "ping: %s", act->name);
	                                        
    if (file == NULL)
            syslog(LOG_INFO, "file: no file to check");
    else
            for (act = file; act != NULL; act = act->next)
                syslog(LOG_INFO, "file: %s:%d", act->name, act->parameter.file.mtime);

    if (pidfile == NULL)
            syslog(LOG_INFO, "pidfile: no server process to check");
    else
            for (act = pidfile; act != NULL; act = act->next)
                syslog(LOG_INFO, "pidfile: %s", act->name);                

    if (iface == NULL)
            syslog(LOG_INFO, "interface: no interface to check");
    else
            for (act = iface; act != NULL; act = act->next)
                syslog(LOG_INFO, "interface: %s", act->name);                

    syslog(LOG_INFO, "test=%s(%ld) repair=%s alive=%s heartbeat=%s temp=%s to=%s no_act=%s",
	    (tbinary == NULL) ? "none" : tbinary, timeout, 
	    (rbinary == NULL) ? "none" : rbinary,
	    (devname == NULL) ? "none" : devname,
	    (heartbeat == NULL) ? "none" : heartbeat,
	    (tempname == NULL) ? "none" : tempname,
	    (admin == NULL) ? "noone" : admin,
	    (no_act == TRUE) ? "yes" : "no");
#endif				/* USE_SYSLOG */


    /* open the device */
    if (devname != NULL && no_act == FALSE) {
	watchdog = open(devname, O_WRONLY);
	if (watchdog == -1) {
#if USE_SYSLOG
	    syslog(LOG_ERR, "cannot open %s (errno = %d = '%m')", devname, errno);
#else				/* USE_SYSLOG */
	    perror(progname);
#endif				/* USE_SYSLOG */
	    /* do not exit here per default */
	    /* we can use watchdog even if there is no watchdog device */
	}
	if (watchdog >= 0 && devtimeout > 0) {
	    /* Set the watchdog hard-stop timeout; default = unset (use
	       driver default) */
	    if (ioctl(watchdog, WDIOC_SETTIMEOUT, &devtimeout) < 0) {
#if USE_SYSLOG
            	syslog(LOG_ERR, "cannot set timeout %s (errno = %d = '%m')", strerror(errno), devtimeout, errno);
#else				
            	perror(progname);
#endif			   
	    }
	}
    }

    /* MJ 16/2/2000, need to keep track of the watchdog writes so that
       I can have a potted history of recent reboots */
    if ( heartbeat != NULL ) {        
        hb = ((hb = fopen(heartbeat, "r+")) == NULL) ? fopen(heartbeat, "w+") : hb;
        if ( hb == NULL ) {
#if USE_SYSLOG
            syslog(LOG_ERR, "cannot open %s (errno = %d = '%m')", heartbeat, errno);
#else				
            perror(progname);
#endif			   
        }
        else {
            char rbuf[TS_SIZE + 1];

            /* Allocate  memory for keeping the timestamps in */
            nrts = 0;
            lastts = 0;
            timestamps = (char *) calloc(hbstamps, TS_SIZE);
            if ( timestamps == NULL ) {
#if USE_SYSLOG
                syslog(LOG_ERR, "cannot allocate memory for timestamps (errno = %d = '%m')", errno);
#else				/* USE_SYSLOG */
                perror(progname);
#endif				/* USE_SYSLOG */
            }
            else {           
                /* read any previous timestamps */
                rewind(hb);
                while ( fgets(rbuf, TS_SIZE + 1, hb) != NULL ) {
                    memcpy(timestamps + (TS_SIZE * lastts), rbuf, TS_SIZE);
                    if (nrts < hbstamps) 
                        nrts++;
                    ++lastts;
                    lastts = lastts % hbstamps;
                }
                /* Write an indication that the watchdog has started to the heartbeat file */
                /* copy it to the buffer */
                sprintf(rbuf, "%*s\n", TS_SIZE - 1, "--restart--");
                memcpy(timestamps + (lastts * TS_SIZE), rbuf, TS_SIZE);

                // success
                if (nrts < hbstamps) 
                    nrts++;
                ++lastts;
                lastts = lastts % hbstamps;

            }
        }
    }

    if (maxload1 > 0) {
	/* open the load average file */
	load = open("/proc/loadavg", O_RDONLY);
	if (load == -1) {
#if USE_SYSLOG
		syslog(LOG_ERR, "cannot open /proc/loadavg (errno = %d = '%m')", errno);
#else				/* USE_SYSLOG */
		perror(progname);
#endif				/* USE_SYSLOG */
	}
    }

    if (minpages > 0) {
	/* open the memory info file */
	mem = open("/proc/meminfo", O_RDONLY);
	if (mem == -1) {
#if USE_SYSLOG
		syslog(LOG_ERR, "cannot open /proc/meminfo (errno = %d = '%m')", errno);
#else				/* USE_SYSLOG */
		perror(progname);
#endif				/* USE_SYSLOG */
	}
    }
    
    if (tempname != NULL && no_act == FALSE) {
	/* open the temperature file */
	temp = open(tempname, O_RDONLY);
	if (temp == -1) {
#if USE_SYSLOG
	    syslog(LOG_ERR, "cannot open %s (errno = %d = '%m')", tempname, errno);
#else				/* USE_SYSLOG */
	    perror(progname);
#endif				/* USE_SYSLOG */
	}
    }

    /* tuck my process id away */
    pid = getpid();
    fp = fopen(PIDFILE, "w");
    if (fp != NULL) {
	fprintf(fp, "%d\n", pid);
	(void) fclose(fp);
    }
    /* set signal term to set our run flag to 0 so that */
    /* we make sure watchdog device is closed when receiving SIGTERM */
    signal(SIGTERM, sigterm_handler);

#if defined(_POSIX_MEMLOCK)
    if (realtime == TRUE) {
	    /* lock all actual and future pages into memory */
	    if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0) {
#if USE_SYSLOG
		syslog(LOG_ERR, "cannot lock realtime memory (errno = %d = '%m')", errno);
#else				/* USE_SYSLOG */
		perror(progname);
#endif				/* USE_SYSLOG */
	    } else {
		struct sched_param sp;

		/* now set the scheduler */
		sp.sched_priority = schedprio;
		    if (sched_setscheduler(0, SCHED_RR, &sp) != 0) {
#if USE_SYSLOG
		    syslog(LOG_ERR, "cannot set scheduler (errno = %d = '%m')", errno);
#else				/* USE_SYSLOG */
		    perror(progname);
#endif				/* USE_SYSLOG */
		} else
		    mlocked = TRUE;
	    }
    }
#endif

    /* main loop: update after <tint> seconds */
    while (_running) {
	wd_action(keep_alive(), rbinary, NULL);

	/* sync system if we have to */
	do_check(sync_system(sync_it), rbinary, NULL);

	/* check file table */
	do_check(check_file_table(), rbinary, NULL);

	/* check load average */
	do_check(check_load(), rbinary, NULL);
	
	/* check free memory */
	do_check(check_memory(), rbinary, NULL);

	/* check temperature */
	do_check(check_temp(), rbinary, NULL);

	/* in filemode stat file */
	for (act = file; act != NULL; act = act->next)
	    do_check(check_file_stat(act), rbinary, act->name);
	    
	/* in pidmode kill -0 processes */
	for (act = pidfile; act != NULL; act = act->next)
	    do_check(check_pidfile(act), rbinary, act->name);

	/* in network mode check the given devices for input */
	for (act = iface; act != NULL; act = act->next)
	    do_check(check_iface(act), rbinary, act->name);
	    
	/* in ping mode ping the ip address */
	for (act = target; act != NULL; act = act->next)
	    do_check(check_net(act->name, act->parameter.net.sock_fp, act->parameter.net.to, act->parameter.net.packet, tint , pingcount), rbinary, act->name);

	/* in user mode execute the given binary or just test fork() call */
	do_check(check_bin(tbinary, timeout), rbinary, NULL);

	/* finally sleep some seconds */
	sleep((tint >> 1) + (tint % 2)); /* this should make watchdog sleep tint seconds alltogther */
	/* sleep(tint); */

#if USE_SYSLOG
	/* do verbose logging */
	if (verbose && logtick && (--ticker == 0)) {
		ticker = logtick;
	  	count += logtick;
	    	syslog(LOG_INFO, "still alive after %ld interval(s)", count);
	}
#endif				/* USE_SYSLOG */
    }

    terminate();
    /* not reached */
}
