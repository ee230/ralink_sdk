/* $Header: /home/cvsroot/RT288x_SDK/source/user/watchdog-5.6/src/net.c,v 1.1 2009-10-14 06:32:06 steven Exp $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <sys/time.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

#include "extern.h"
#include "watch_err.h"

#if USE_SYSLOG
#include <syslog.h>
#endif

/*
 * in_cksum --
 *      Checksum routine for Internet Protocol family headers (C Version)
 */
static int in_cksum(unsigned short *addr, int len)
{
    int nleft = len, sum = 0;
    unsigned short *w = addr, answer = 0;

    /*
       * Our algorithm is simple, using a 32 bit accumulator (sum), we add
       * sequential 16 bit words to it, and at the end, fold back all the
       * carry bits from the top 16 bits into the lower 16 bits.
     */
    while (nleft > 1) {
	sum += *w++;
	nleft -= 2;
    }				/* mop up an odd byte, if necessary */
    if (nleft == 1) {
	*(unsigned char *) (&answer) = *(unsigned char *) w;
	sum += answer;
    }
    /* add back carry outs from top 16 bits to low 16 bits */
    sum = (sum >> 16) + (sum & 0xffff);		/* add hi 16 to low 16 */
    sum += (sum >> 16);		/* add carry */
    answer = ~sum;		/* truncate to 16 bits */
    return (answer);
}

int check_net(char *target, int sock_fp, struct sockaddr to, unsigned char *packet, int time, int count)
{
    int i;
    unsigned char outpack[MAXPACKET];

    if (target == NULL)
	return (ENOERR);

    /* try "ping-count" times */
    for (i = 0; i < count; i++) {

	struct sockaddr_in from;
	int fdmask, j;
	socklen_t fromlen;
	struct timeval timeout, dtimeout;
	struct icmphdr *icp = (struct icmphdr *) outpack;

	/* setup a ping message */
	icp->type = ICMP_ECHO;
	icp->code = icp->checksum = icp->un.echo.sequence = 0;
	icp->un.echo.id = pid;	/* ID */

	/* compute ICMP checksum here */
	icp->checksum = in_cksum((unsigned short *) icp, DATALEN + 8);

	/* and send it out */
	j = sendto(sock_fp, (char *) outpack, DATALEN + 8, 0, &to,
		   sizeof(struct sockaddr));

	if (j < 0) {
	    int err = errno;

	    /* if our kernel tells us the network is unreachable we are done */
	    if (err == ENETUNREACH) {
#if USE_SYSLOG
		syslog(LOG_ERR, "network is unreachable (target: %s)", target);
#endif				/* USE_SYSLOG */

		return (ENETUNREACH);

	    } else {
#if USE_SYSLOG
		syslog(LOG_ERR, "sendto gave errno = %d = '%m'\n", err);
#else				/* USE_SYSLOG */
		perror(progname);
#endif				/* USE_SYSLOG */

		if (softboot)
		    return (err);
	    }

	} else {
            gettimeofday(&timeout, NULL);
	    /* set the timeout value */
            timeout.tv_sec += time / count;

	    /* wait for reply */
	    fdmask = 1 << sock_fp;
            while(1) {
               gettimeofday(&dtimeout, NULL);
               dtimeout.tv_sec = timeout.tv_sec - dtimeout.tv_sec;
               dtimeout.tv_usec = timeout.tv_usec - dtimeout.tv_usec;
               if (dtimeout.tv_usec < 0) {
	       		dtimeout.tv_usec += 1000000;
			dtimeout.tv_sec--;
               }
	       while (dtimeout.tv_usec >= 1000000)
	       {
	       		dtimeout.tv_usec -= 1000000;
			dtimeout.tv_sec++;
	       }
               if (dtimeout.tv_sec <= 0)
                   break;
#if USE_SYSLOG
               if (verbose && logtick && ticker == 1)
                   syslog(LOG_ERR, "ping select timeout = %ld seconds and %ld useconds\n", dtimeout.tv_sec, dtimeout.tv_usec);
#endif /* USE_SYSLOG */

            	   if (select(sock_fp + 1, (fd_set *) & fdmask, (fd_set *) NULL,
                      (fd_set *) NULL, &dtimeout) >= 1) {

                   /* read reply */
                   fromlen = sizeof(from);
                   if (recvfrom(sock_fp, (char *) packet, DATALEN + MAXIPLEN + MAXICMPLEN, 0,
			     (struct sockaddr *) &from, &fromlen) < 0) {
                       int err = errno;

                       if (err != EINTR)
#if USE_SYSLOG
                           syslog(LOG_ERR, "recvfrom gave errno = %d = '%m'\n", err);
#else				/* USE_SYSLOG */
                           perror(progname);
#endif				/* USE_SYSLOG */
                       if (softboot)
                           return (err);

                       continue;
                   }

                   /* check if packet is our ECHO */
                   icp = (struct icmphdr *) (packet + (((struct ip *) packet)->ip_hl << 2));

                   if (icp->type == ICMP_ECHOREPLY && icp->un.echo.id == pid) {
			if (from.sin_addr.s_addr == ((struct sockaddr_in *) &to)->sin_addr.s_addr) {
#if USE_SYSLOG
				if (verbose && logtick && ticker == 1)
					syslog(LOG_INFO, "got answer from target %s", target);
#endif
				return (ENOERR);
			}
                    }
		}
	    }
	}
    }
#if USE_SYSLOG
    syslog(LOG_ERR, "no response from ping (target: %s)", target);
#endif				/* USE_SYSLOG */
    return (ENETUNREACH);
}

