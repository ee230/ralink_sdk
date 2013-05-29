/* $Header: /home/cvsroot/RT288x_SDK/source/user/watchdog-5.6/src/shutdown.c,v 1.1 2009-10-14 06:32:06 steven Exp $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define _XOPEN_SOURCE 500      /* for getsid(2) */
#define _BSD_SOURCE            /* for acct(2) */

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <mntent.h>
#include <netdb.h>
#include <paths.h>
#include <setjmp.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <utmp.h>
#include <sys/mman.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <rpc/types.h>         /* for caddr_t */

#include "watch_err.h"
#include "extern.h"

#if defined __GLIBC__
#include "ext2_mnt.h"
#include <sys/quota.h>
#include <sys/swap.h>
#include <sys/reboot.h>
#else				/* __GLIBC__ */
#include <linux/quota.h>
#endif				/* __GLIBC__ */

#include <unistd.h>

#if USE_SYSLOG
#include <syslog.h>
#endif				/* USE_SYSLOG */

#ifndef NSIG
#define NSIG _NSIG
#endif

extern void umount_all(void *);
extern int ifdown(void);
#if 0
extern int mount_one(char *, char *, char *, char *, int, int);
static struct mntent rootfs;
#endif

#if defined(_POSIX_MEMLOCK)
extern int mlocked, realtime;
#endif /* _POSIX_MEMLOCK */
extern volatile sig_atomic_t _running;
extern int devtimeout; /* From watchdog.c */

jmp_buf ret2dog;

/* Info about a process. */
typedef struct _proc_ {
    pid_t pid;			/* Process ID.                    */
    int sid;			/* Session ID.                    */
    struct _proc_ *next;	/* Pointer to next struct.        */
} PROC;

/* write a log entry on exit */
static void log_end()
{
#if USE_SYSLOG
    /* Log the closing message */
    syslog(LOG_INFO, "stopping daemon (%d.%d)", MAJOR_VERSION, MINOR_VERSION);
    closelog();

    sleep(5);			/* make sure log is written */
#endif				/* USE_SYSLOG */
    return;
}

/* close the device and check for error */
static void close_all()
{
    if (watchdog != -1) {
        if (write(watchdog, "V", 1) < 0 ) {
		int err = errno;
#if USE_SYSLOG
		syslog(LOG_ERR, "write watchdog device gave error %d = '%m'!", err);
#else				/* USE_SYSLOG */
		perror(progname);
#endif				/* USE_SYSLOG */
	}

        if (close(watchdog) == -1) {
#if USE_SYSLOG
            syslog(LOG_ALERT, "cannot close %s (errno = %d)", devname, errno);
#else				/* USE_SYSLOG */
            perror(progname);
#endif				/* USE_SYSLOG */
        }
    }
    
    if (load != -1 && close(load) == -1) {
#if USE_SYSLOG
	syslog(LOG_ALERT, "cannot close /proc/loadavg (errno = %d)", errno);
#else				/* USE_SYSLOG */
	perror(progname);
#endif				/* USE_SYSLOG */
    }
    
    if (mem != -1 && close(mem) == -1) {
#if USE_SYSLOG
	syslog(LOG_ALERT, "cannot close /proc/meminfo (errno = %d)", errno);
#else				/* USE_SYSLOG */
	perror(progname);
#endif				/* USE_SYSLOG */
    }
    
    if (temp != -1 && close(temp) == -1) {
#if USE_SYSLOG
	syslog(LOG_ALERT, "cannot close /dev/temperature (errno = %d)", errno);
#else				/* USE_SYSLOG */
	perror(progname);
#endif				/* USE_SYSLOG */
    }

    if (hb != NULL && fclose(hb) == -1) {
#if USE_SYSLOG
	syslog(LOG_ALERT, "cannot close %s (errno = %d)", heartbeat, errno);
#else				/* USE_SYSLOG */
	perror(progname);
#endif				/* USE_SYSLOG */
    }
}


void sigterm_handler(int arg)
{
    _running = 0;
}


/* on exit we close the device and log that we stop */
void terminate(void)
{
#if defined(_POSIX_MEMLOCK)
    if (realtime == TRUE && mlocked == TRUE)
    {
	/* unlock all locked pages */
	if (munlockall() != 0) {
#if USE_SYSLOG
		syslog(LOG_ERR, "cannot unlock realtime memory (errno = %d)", errno);
#else				/* USE_SYSLOG */
		perror(progname);
#endif				/* USE_SYSLOG */
	}
    }
#endif		/* _POSIX_MEMLOCK */
    close_all();
    log_end();
    if (timestamps != NULL)
	    free(timestamps);
    exit(0);
}

/* panic: we're still alive but shouldn't */
static void panic(void)
{
    /* if we are still alive, we just exit */
    close_all();
    fprintf(stderr, "WATCHDOG PANIC: Still alive after sleeping %d seconds!\n", 4 * devtimeout);
#if USE_SYSLOG
    openlog(progname, LOG_PID, LOG_DAEMON);
    syslog(LOG_ALERT, "still alive after sleeping %d seconds", 4 * devtimeout);
    closelog();
#endif
    exit(1);
}

static void mnt_off()
{
    FILE *fp;
    struct mntent *mnt;

    fp = setmntent(MNTTAB, "r");
    /* in some rare cases fp might be NULL so be careful */
    while (fp != NULL && ((mnt = getmntent(fp)) != (struct mntent *) 0)) {
	/* First check if swap */
	if (!strcmp(mnt->mnt_type, MNTTYPE_SWAP))
	    if (swapoff(mnt->mnt_fsname) < 0)
		perror(mnt->mnt_fsname);

	/* quota only if mounted at boot time && filesytem=ext2 */
	if (hasmntopt(mnt, MNTOPT_NOAUTO) || strcmp(mnt->mnt_type, MNTTYPE_EXT2))
	    continue;

	/* group quota? */
	if (hasmntopt(mnt, MNTOPT_GRPQUOTA))
	    if (quotactl(QCMD(Q_QUOTAOFF, GRPQUOTA), mnt->mnt_fsname, 0, (caddr_t) 0) < 0)
		perror(mnt->mnt_fsname);

	/* user quota */
	if (hasmntopt(mnt, MNTOPT_USRQUOTA))
	    if (quotactl(QCMD(Q_QUOTAOFF, USRQUOTA), mnt->mnt_fsname, 0, (caddr_t) 0) < 0)
		perror(mnt->mnt_fsname);

#if 0
	/* not needed anymore */
	/* while we're at it we add the remount option */
	if (strcmp(mnt->mnt_dir, "/") == 0) {
		/* save entry if root partition */
		rootfs.mnt_freq = mnt->mnt_freq;
		rootfs.mnt_passno = mnt->mnt_passno;

		rootfs.mnt_fsname = strdup(mnt->mnt_fsname);
		rootfs.mnt_dir = strdup(mnt->mnt_dir);
		rootfs.mnt_type = strdup(mnt->mnt_type);

		/* did we get enough memory? */
		if (rootfs.mnt_fsname == NULL || rootfs.mnt_dir == NULL || rootfs.mnt_type == NULL) {
#if USE_SYSLOG
		    syslog(LOG_ERR, "out of memory");
#else				/* USE_SYSLOG */
		    fprintf(stderr, "%s: out of memory\n", progname);
#endif
		}

		if ((rootfs.mnt_opts = malloc(strlen(mnt->mnt_opts) + strlen("remount,ro") + 2)) == NULL) {
#if USE_SYSLOG
			syslog(LOG_ERR, "out of memory");
#else				/* USE_SYSLOG */
			fprintf(stderr, "%s: out of memory\n", progname);
#endif
		} else
			sprintf(rootfs.mnt_opts, "%s,remount,ro", mnt->mnt_opts);
	}
#endif
    }
    endmntent(fp);
}

/* Parts of the following two functions are taken from Miquel van */
/* Smoorenburg's killall5 program. */

static PROC *plist;

/* get a list of all processes */
static int readproc()
{
    DIR *dir;
    struct dirent *d;
    pid_t act_pid;
    PROC *p;

    /* Open the /proc directory. */
    if ((dir = opendir("/proc")) == NULL) {
#if USE_SYSLOG
	syslog(LOG_ERR, "cannot opendir /proc");
#else				/* USE_SYSLOG */
	perror(progname);
#endif
	return (-1);
    }
    plist = NULL;

    /* Walk through the directory. */
    while ((d = readdir(dir)) != NULL) {

	/* See if this is a process */
	if ((act_pid = atoi(d->d_name)) == 0)
	    continue;

	/* Get a PROC struct . */
	if ((p = (PROC *) calloc(1, sizeof(PROC))) == NULL) {
#if USE_SYSLOG
	    syslog(LOG_ERR, "out of memory");
#else				/* USE_SYSLOG */
	    fprintf(stderr, "%s: out of memory\n", progname);
#endif
	    return (-1);
	}
	p->sid = getsid(act_pid);
	p->pid = act_pid;

	/* Link it into the list. */
	p->next = plist;
	plist = p;
    }
    closedir(dir);

    /* Done. */
    return (0);
}

static void killall5(int sig)
{
    PROC *p;
    int sid = -1;

    /*
     *    Ignoring SIGKILL and SIGSTOP do not make sense, but
     *    someday kill(-1, sig) might kill ourself if we don't
     *    do this. This certainly is a valid concern for SIGTERM-
     *    Linux 2.1 might send the calling process the signal too.
     */

    /* Since we ignore all signals, we don't have to worry here. MM */
    /* Now stop all processes. */
    kill(-1, SIGSTOP);

    /* Find out our own 'sid'. */
    if (readproc() < 0) {
	kill(-1, SIGCONT);
	return;
    }
    for (p = plist; p; p = p->next)
	if (p->pid == pid) {
	    sid = p->sid;
	    break;
	}
    /* Now kill all processes except our session. */
    for (p = plist; p; p = p->next)
	if (p->pid != pid && p->sid != sid)
	    kill(p->pid, sig);

    /* And let them continue. */
    kill(-1, SIGCONT);
}

/* shut down the system */
void do_shutdown(int errorcode)
{
    int i = 0, fd;
    char *seedbck = RANDOM_SEED;

    /* soft-boot the system */
    /* do not close open files here, they will be closed later anyway */
    /* close_all(); */

    /* if we will halt the system we should try to tell a sysadmin */
    if (admin != NULL) {
	/* send mail to the system admin */
	FILE *ph;
	char exe[128];
        struct stat buf;

	/* Only can send an email if sendmail binary exists so check
	 * that first, or else we will get a broken pipe in pclose.
	 * We cannot let the shell check, because a non-existant or
	 * non-executable sendmail binary means that the pipe is closed faster
	 * than we can write to it. */
	if ((stat(PATH_SENDMAIL, &buf) != 0) || ((buf.st_mode&S_IXUSR) == 0))
#if USE_SYSLOG
		syslog(LOG_ERR, "%s does not exist or is not executable (errno = %d)", PATH_SENDMAIL, errno);
#endif				/* USE_SYSLOG */
	else
	{
		sprintf(exe, "%s -i %s", PATH_SENDMAIL, admin);
		ph = popen(exe, "w");
		if (ph == NULL) {
#if USE_SYSLOG
		    syslog(LOG_ERR, "cannot start %s (errno = %d)", PATH_SENDMAIL, errno);
#endif				/* USE_SYSLOG */
		} else {
		    char myname[MAXHOSTNAMELEN + 1];
		    struct hostent *hp;

		    /* get my name */
		    gethostname(myname, sizeof(myname));

		    fprintf(ph, "To: %s\n", admin);
		    if (ferror(ph) != 0) {
#if USE_SYSLOG
			syslog(LOG_ERR, "cannot send mail (errno = %d)", errno);
#endif				/* USE_SYSLOG */
		    }
		    /* if possible use the full name including domain */
		    if ((hp = gethostbyname(myname)) != NULL)
			fprintf(ph, "Subject: %s is going down!\n\n", hp->h_name);
		    else
			fprintf(ph, "Subject: %s is going down!\n\n", myname);
		    if (ferror(ph) != 0) {
#if USE_SYSLOG
			syslog(LOG_ERR, "cannot send mail (errno = %d)", errno);
#endif				/* USE_SYSLOG */
		    }

		    if (errorcode == ETOOHOT)
			fprintf(ph, "Message from watchdog:\nIt is too hot to keep on working. The system will be halted!\n");
		    else
			fprintf(ph, "Message from watchdog:\nThe system will be rebooted because of error %d!\n", errorcode);
		    if (ferror(ph) != 0) {
#if USE_SYSLOG
			syslog(LOG_ERR, "cannot send mail (errno = %d)", errno);
#endif				/* USE_SYSLOG */
		    }
		    if (pclose(ph) == -1) {
#if USE_SYSLOG
			syslog(LOG_ERR, "cannot finish mail (errno = %d)", errno);
#endif				/* USE_SYSLOG */
		    }
		    /* finally give the system a little bit of time to deliver */
		}
	}
    }

#if USE_SYSLOG
    /* now tell syslog what's happening */
    syslog(LOG_ALERT, "shutting down the system because of error %d", errorcode);
    closelog();
#endif				/* USE_SYSLOG */

    keep_alive();
    sleep(10);			/* make sure log is written and mail is send */
    keep_alive();

    /* We cannot start shutdown, since init might not be able to fork. */
    /* That would stop the reboot process. So we try rebooting the system */
    /* ourselves. Note, that it is very likely we cannot start any rc */
    /* script either, so we do it all here. */

    /* Close all files except the watchdog device. */
    for (i = 0; i < 3; i++)
	if (!isatty(i))
	    close(i);
    for (i = 3; i < 20; i++)
	if (i != watchdog)
		close(i);
    close(255);

    /* Ignore all signals. */
    for (i = 1; i < NSIG; i++)
	signal(i, SIG_IGN);

    /* Stop init; it is insensitive to the signals sent by the kernel. */
    kill(1, SIGTSTP);

    /* Kill all processes. */
    (void) killall5(SIGTERM);
    sleep(5);
    (void) killall5(SIGKILL);
    keep_alive();

    /* Record the fact that we're going down */
    if ((fd = open(_PATH_WTMP, O_WRONLY | O_APPEND)) >= 0) {
	time_t t;
	struct utmp wtmp;

	time(&t);
	strcpy(wtmp.ut_user, "shutdown");
	strcpy(wtmp.ut_line, "~");
	strcpy(wtmp.ut_id, "~~");
	wtmp.ut_pid = 0;
	wtmp.ut_type = RUN_LVL;
	wtmp.ut_time = t;
	write(fd, (char *) &wtmp, sizeof(wtmp));
	close(fd);
    }
    
    /* save the random seed if a save location exists */
    /* don't worry about error messages, we react here anyway */
    if (strlen(seedbck) != 0) {
	int fd_seed;

	if ((fd_seed = open("/dev/urandom", O_RDONLY)) >= 0) {
	    int fd_bck;

	    if ((fd_bck = creat(seedbck, S_IRUSR | S_IWUSR)) >= 0) {
		char buf[512];

		if (read(fd_seed, buf, 512) == 512)
		    write(fd_bck, buf, 512);
		close(fd_bck);
	    }
	    close(fd_seed);
	}
    }
    
    /* Turn off accounting */
    if (acct(NULL) < 0)
	perror(progname);

    keep_alive();

    /* Turn off quota and swap */
    mnt_off();

    /* umount all partitions */
    if (setjmp(ret2dog) == 0)
	umount_all(NULL);

#if 0
    /* with the more recent version of mount code, this is not needed anymore */
    /* remount / read-only */
    if (setjmp(ret2dog) == 0)
	mount_one(rootfs.mnt_fsname, rootfs.mnt_dir, rootfs.mnt_type,
		  rootfs.mnt_opts, rootfs.mnt_freq, rootfs.mnt_passno);
#endif

    /* shut down interfaces (also taken from sysvinit source */
    ifdown();

    /* finally reboot */
    if (errorcode != ETOOHOT) {
#ifdef __GLIBC__
	reboot(RB_AUTOBOOT);
#else				/* __GLIBC__ */
	reboot(0xfee1dead, 672274793, 0x01234567);
#endif				/* __GLIBC__ */
    } else {
	/* rebooting makes no sense if it's too hot */
	/* Turn on hard reboot, CTRL-ALT-DEL will reboot now */
#ifdef __GLIBC__
	reboot(RB_ENABLE_CAD);
#else				/* __GLIBC__ */
	reboot(0xfee1dead, 672274793, 0x89abcdef);
#endif				/* __GLIBC__ */

	/* And perform the `halt' system call. */
#ifdef __GLIBC__
	reboot(RB_HALT_SYSTEM);
#else				/* __GLIBC__ */
	reboot(0xfee1dead, 672274793, 0xcdef0123);
#endif
    }

    /* okay we should never reach this point, */
    /* but if we do we will cause the hard reset */

    sleep(devtimeout * 4);

    /* unbelievable: we're still alive */
    panic();
}


