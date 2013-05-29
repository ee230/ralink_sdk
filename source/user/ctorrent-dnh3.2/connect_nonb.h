#ifndef CONNECT_NONB_H
#define CONNECT_NONB_H

#include "./def.h"

#ifdef WINDOWS
#include <Winsock2.h>
#else
#include <stdio.h>   // autoconf manual: Darwin + others prereq for stdlib.h
#include <stdlib.h>  // autoconf manual: Darwin prereq for sys/socket.h
#include <sys/types.h>
#include <sys/socket.h>
#endif

int connect_nonb(SOCKET sk,struct sockaddr *psa);

#endif
