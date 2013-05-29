/* 	$Id: inet_aton.c,v 1.1 2007-09-06 07:00:05 winfred Exp $	*/

#include "totd.h"

#if !HAVE_INET_ATON
int inet_aton(const char *cp, struct in_addr *inp)
{
  in_addr_t lala;

  if((lala = inet_addr(cp)) < 0)
    return(0);
  inp->s_addr = lala;
  return(1);
}
#endif
