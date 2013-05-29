/* header file for mpstat */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/dir.h>

#ifdef __2_4__
#include <linux/threads.h>
#else 
#include <linux/tasks.h>
#endif

/* obtain max num cpus from kernel multiply by 2 and add 2 */
#define MAX_NR_CPUS  ((NR_CPUS*2)+2)

#define FALSE 0
#define TRUE 1
#define VERSION "Version: 0.1.1, last modified 23 Feb 2001: INCOMPLETE" 
#define PROGNAME "mpstat"
#define REQUIRES "modified for 2.2.18+ kernels."
#define ONEBITE 1
#define TWOBITE 2
#define BUFFSIZE 1024
#define BUFFLEN 512
#define PROCESSOR "processor"

int winhi(void);
void showheader(void);
void usage(void);
void crash(char *);
int read_nr_cpus(int *); /* used to get number of cpus from /proc/cpuinfo */
void get_inter (unsigned *, int); /*gets interrupts per processor */
void getfaults(unsigned *i, unsigned *, int, int *); /* this is used to get major and minuor faults */
static char buff[BUFFSIZE]; /* used in the procedures */
