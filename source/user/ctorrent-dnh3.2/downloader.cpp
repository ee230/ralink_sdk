#include "def.h"

#include <sys/types.h>

#include <sys/time.h>
#include <time.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "peerlist.h"
#include "tracker.h"
#include "btcontent.h"
#include "ctcs.h"
#include "btconfig.h"
#include "bttime.h"
#include "console.h"

#define MAX_SLEEP 1

time_t now = time((time_t *)0);

void Downloader()
{
  int nfds = 0, maxfd, r;
  struct timeval timeout;
  fd_set rfd, rfdnext;
  fd_set wfd, wfdnext;
  int stopped = 0, f_idleused = 0, f_poll = 0;
  struct timespec nowspec;
  double maxsleep;
  time_t then, concheck = (time_t)0;

  FD_ZERO(&rfdnext); FD_ZERO(&wfdnext);

  time(&now);
  do{
    if( !stopped ){
      if( !Tracker.IsQuitting() && BTCONTENT.SeedTimeout() )
        Tracker.SetStoped();
      if( Tracker.IsQuitting() ){
        stopped = 1;
        if( arg_ctcs ) CTCS.Send_Status();
      }
    }

    maxfd = -1;
    maxsleep = -1;
    rfd = rfdnext;
    wfd = wfdnext;

    if( f_poll ){
      FD_ZERO(&rfd); FD_ZERO(&wfd);  // remove non-peers from sets
      maxsleep = 0;  // waited for bandwidth--poll now
    }else{
      WORLD.DontWaitBW();
      if( WORLD.IsIdle() ){
        f_idleused = 0;
        if( BTCONTENT.CheckedPieces() < BTCONTENT.GetNPieces() &&
            !BTCONTENT.NeedFlush() ){
          if( BTCONTENT.CheckNextPiece() < 0 ){
            CONSOLE.Warning(1, "Error while checking piece %d of %d",
              (int)(BTCONTENT.CheckedPieces()), (int)(BTCONTENT.GetNPieces()));
            Tracker.SetStoped();
            maxsleep = 2;
          }else maxsleep = 0;
          f_idleused = 1;
        }
        r = Tracker.IntervalCheck(&rfd, &wfd);
        if( r > maxfd ) maxfd = r;
        if( arg_ctcs ){
          r = CTCS.IntervalCheck(&rfd, &wfd);
          if( r > maxfd ) maxfd = r;
        }
        if( !f_idleused || concheck <= now-2 || WORLD.IsIdle() ){
          concheck = now;
          r = CONSOLE.IntervalCheck(&rfd, &wfd);
          if( r > maxfd ) maxfd = r;
        }
      }
    }
    r = WORLD.IntervalCheck(&rfd, &wfd);
    if( r > maxfd ) maxfd = r;

    while( !f_poll && BTCONTENT.NeedFlush() && WORLD.IsIdle() ){
      BTCONTENT.FlushQueue();
      maxsleep = 0;
    }

    rfdnext = rfd;
    wfdnext = wfd;

    if( maxsleep < 0 ){  //not yet set
      maxsleep = WORLD.WaitBW();  // must do after intervalchecks!
      if( maxsleep <= -100 ) maxsleep = 0;
      else if( maxsleep <= 0 || maxsleep > MAX_SLEEP ) maxsleep = MAX_SLEEP;
    }

    timeout.tv_sec = (long)maxsleep;
    timeout.tv_usec = (long)( (maxsleep-(long)maxsleep) * 1000000 );

    WORLD.UnLate();
    nfds = select(maxfd + 1,&rfd,&wfd,(fd_set*) 0,&timeout);

    if( f_poll ) f_poll = 0;
    else if( nfds > 0 ) WORLD.DontWaitBW();
    else if( maxsleep > 0 && maxsleep < MAX_SLEEP ) f_poll = 1;

    then = now;
    time(&now);
    if( now == then-1 ) now = then;

    if( !f_poll && nfds > 0 ){
      if(T_FREE != Tracker.GetStatus())
        Tracker.SocketReady(&rfd,&wfd,&nfds,&rfdnext,&wfdnext);
      if(nfds > 0 && T_FREE != CTCS.GetStatus())
        CTCS.SocketReady(&rfd,&wfd,&nfds,&rfdnext,&wfdnext);
      if(nfds > 0)
        CONSOLE.User(&rfd,&wfd,&nfds,&rfdnext,&wfdnext);
    }
    if(nfds > 0)
      WORLD.AnyPeerReady(&rfd,&wfd,&nfds,&rfdnext,&wfdnext);
  } while(Tracker.GetStatus() != T_FINISHED || Tracker.IsRestarting());
}
