#include "def.h"

/* compat.c:  Copyright 2007 Dennis Holmes  (dholmes@rahul.net)
 *            except as noted.
 */

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#include "compat.h"

#ifndef HAVE_CLOCK_GETTIME
int clock_gettime(clockid_t clock_id, struct timespec *tp)
{
  int r = 0;

#if defined(HAVE_GETTIMEOFDAY)
  struct timeval tv;
  if( (r = gettimeofday(&tv, (struct timezone *)0)) == 0 ){
    tp->tv_sec = tv.tv_sec;
    tp->tv_nsec = tv.tv_usec * 1000;
  }
#else
#error No suitable precision timing functions appear to be available!
#error Please report this problem and identify your system platform.
#endif

  return r;
}
#endif


#ifndef HAVE_VSNPRINTF
int vsnprintf(char *str, size_t size, const char *format, va_list ap)
{
  int r;
  char *buffer[4*MAXPATHLEN];  /* git-r-dun */

  if( (r = vsprintf(buffer, format, ap)) >= 0){
    strncpy(str, buffer, size-1);
    str[size] = '\0';
  }
  return r;
}
#endif


#ifndef HAVE_SNPRINTF
int snprintf(char *str, size_t size, const char *format, ...)
{
  int r;
  va_list ap;

  va_start(ap, format);
  r = vsnprintf(str, size, format, ap);
  va_end(ap);

  return r;
}
#endif


#ifndef HAVE_STRNSTR
#include <string.h>
/* FUNCTION PROGRAMER: Siberiaic Sang */
char *strnstr(const char *haystack, const char *needle, size_t haystacklen)
{
  char *p;
  ssize_t plen;
  ssize_t len = strlen(needle);

  if( *needle == '\0' )
    return (char *)haystack;

  plen = haystacklen;
  for( p = (char *)haystack;
       p != (char *)0;
       p = (char *)memchr(p + 1, *needle, plen-1) ){
    plen = haystacklen - (p - haystack);

    if( plen < len ) return (char *)0;

    if( strncmp(p, needle, len) == 0 )
      return (p);
  }
  return (char*) 0;
}
#endif


#ifndef HAVE_STRNCASECMP
#include <ctype.h>
int strncasecmp(const char *s1, const char *s2, size_t len)
{
  unsigned char c1, c2;
  int r = 0;

  for( ; r==0 && len && *s1 && *s2; s1++, s2++, len-- ){
    if( *s1 == *s2 ) continue;
    c1 = (unsigned char)tolower((int)*s1);
    c2 = (unsigned char)tolower((int)*s2);
    if( c1 == c2 ) continue;
    else if( c1 < c2 ) r = -1;
    else if( c1 > c2 ) r = 1;
  }
  if( r==0 && len ){
    if( !*s1 && !*s2 ) r = 0;
    else if( *s1 ) r = 1;
    else r = -1;
  }
  return r;
}
#endif

#ifndef HAVE_STRCASECMP
int strcasecmp(const char *s1, const char *s2)
{
  return strncasecmp(s1, s2, strlen(s1));
}
#endif

