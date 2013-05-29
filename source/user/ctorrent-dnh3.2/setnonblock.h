#ifndef SETNONBLOCK_H
#define SETNONBLOCK_H

#include "./def.h"
#include <sys/types.h>

#ifdef WINDOWS
#include <Winsock2.h>
#else
#include <stdio.h>   // autoconf manual: Darwin + others prereq for stdlib.h
#include <stdlib.h>  // autoconf manual: Darwin prereq for sys/socket.h
#include <sys/socket.h>
#endif

int setfd_nonblock(SOCKET socket);

#endif
