#ifndef WINDOWS
#include "sigint.h"  // def.h

#include <sys/types.h>
#include <signal.h>

#include "btcontent.h"
#include "tracker.h"
#include "peerlist.h"
#include "btconfig.h"
#include "console.h"

extern "C" {

RETSIGTYPE sig_catch(int sig_no)
{
  if(SIGINT == sig_no || SIGTERM == sig_no){
    Tracker.ClearRestart();
    Tracker.SetStoped();
    signal(sig_no,sig_catch2);
  }
}

static RETSIGTYPE sig_catch2(int sig_no)
{
  if(SIGINT == sig_no || SIGTERM == sig_no){
    if( cfg_cache_size ) BTCONTENT.FlushCache();
    BTCONTENT.SaveBitfield();
    WORLD.CloseAll();
    signal(sig_no,SIG_DFL);
    raise(sig_no);
  }
}


// Handler for other signals
RETSIGTYPE signals(int sig_no)
{
  return CONSOLE.Signal(sig_no);
}

} // extern "C"

#endif


void sig_setup()
{
#ifndef WINDOWS
  signal(SIGPIPE,SIG_IGN);
  signal(SIGINT,sig_catch);
  signal(SIGTERM,sig_catch);

  // Don't let printf restart a write after SIGTTOU, we will hard-loop!
  struct sigaction handler;
    handler.sa_handler = signals;
    sigemptyset(&(handler.sa_mask));
    handler.sa_flags = 0;   // SA_RESTART is not set
  sigaction(SIGTTOU, &handler, (struct sigaction *)0);

  // Likewise with input after SIGTTIN
  sigaction(SIGTTIN, &handler, (struct sigaction *)0);

  signal(SIGCONT,signals);
  signal(SIGTSTP,signals);
#endif
}

