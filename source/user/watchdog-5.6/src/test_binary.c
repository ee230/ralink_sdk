/* $Header: /home/cvsroot/RT288x_SDK/source/user/watchdog-5.6/src/test_binary.c,v 1.1 2009-10-14 06:32:06 steven Exp $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>

#include "extern.h"
#include "watch_err.h"

#if USE_SYSLOG
#include <syslog.h>
#endif

struct process
{
    pid_t pid;
    time_t time;
    struct process *next;
};

static struct process *process_head = NULL;

static void add_process (pid_t pid)
{
    struct process *node = (struct process *) malloc (sizeof (struct process));
    node->pid = pid;
    node->time = time (NULL);
    node->next = process_head;
    process_head = node;
}

static void remove_process (pid_t pid)
{
    struct process *last, *current;
    last = NULL;
    current = process_head;
    while (current != NULL && current->pid != pid) {
        last = current;
        current = current->next;
    }
    if (current != NULL) {
        if (last == NULL)
            process_head = current->next;
        else
            last->next = current->next;
        free (current);
    }
}

/* See if any test processes have exceeded the timeout */
static int check_processes (time_t timeout)
{
    struct process *current;
    time_t now = time (NULL);
    
    current = process_head;
    while (current != NULL) {
        if (now - current->time > timeout)
            return (ETOOLONG);
        current = current->next;
    }
    return (ENOERR);
}

/* execute test binary */
int check_bin(char *tbinary, time_t timeout)
{
    pid_t child_pid;
    int result, res = 0;

    if (timeout > 0)
	    res = check_processes(timeout);
    if (res == ETOOLONG) {
#if USE_SYSLOG
        syslog(LOG_ERR, "test-binary %s exceeded time limit %ld", tbinary, timeout);
#endif				/* USE_SYSLOG */
        return res;
    }

    child_pid = fork();
    if (!child_pid) {
	
	/* child, exit immediately, if no test binary given */
	if (tbinary == NULL)
	    exit(0);

	/* Don't want the stdin and stdout of our test program
	 * to cause trouble
	 * So make stdout and stderr go to their respective files */	
	if (!freopen("/var/log/watchdog/test-bin.stdout", "a+", stdout))
	    exit (errno);
	if (!freopen("/var/log/watchdog/test-bin.stderr", "a+", stderr))
	    exit (errno);
	
	/* else start binary */
	execl(tbinary, tbinary, NULL);

	/* execl should only return in case of an error */
	/* so we return that error */
	exit(errno);
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
    } else {
	int ret, err;

	/* fork was okay, add child to process list */
	add_process(child_pid);

	/* wait for child(s) to stop */
	/* but only after a short sleep */
	sleep(tint >> 1);

	do {
	    ret = waitpid(-1, &result, WNOHANG);
	    err = errno;
        if (ret > 0)
            remove_process(ret);
	} while (ret > 0 && WIFEXITED(result) != 0 && WEXITSTATUS(result) == 0);

	/* check result: */
	/* ret < 0 			=> error */
	/* ret == 0			=> no more child returned, however we may already have caught the actual child */
	/* WIFEXITED(result) == 0	=> child did not exit normally but was killed by signal which was not caught */
	/* WEXITSTATUS(result) != 0	=> child returned an error code */
	if (ret > 0) {
		if (WIFEXITED(result) != 0) {
			/* if one of the scripts returns an error code just return that code */
#if USE_SYSLOG
			syslog(LOG_ERR, "test binary returned %d", WEXITSTATUS(result));
#endif				/* USE_SYSLOG */
		    	return (WEXITSTATUS(result));
		} else if (WIFSIGNALED(result) != 0)  {
			/* if one of the scripts was killed return ECHKILL */
#if USE_SYSLOG
			syslog(LOG_ERR, "test binary was killed by uncaught signal %d", WTERMSIG(result));
#endif				/* USE_SYSLOG */
		    	return (ECHKILL);
		}
	} else {
		/* in case there are still old childs running due to an error */
		/* log that error */
		if (err != 0 && err != ECHILD) {
#if USE_SYSLOG
		    errno = err;
		    syslog(LOG_ERR, "child %d did not exit immediately (error = %d = '%m')", child_pid, err);
#else				/* USE_SYSLOG */
		    perror(progname);
#endif				/* USE_SYSLOG */
		    if (softboot)
			return (err);
		}
	}
    }
    return (ENOERR);
}
