#ifndef SIGINT_H
#define SIGINT_H

#include "def.h"

#ifndef WINDOWS
extern "C" {

RETSIGTYPE sig_catch(int sig_no);
static RETSIGTYPE sig_catch2(int sig_no);
RETSIGTYPE signals(int sig_no);

} // extern "C"
#endif

void sig_setup();

#endif
