#ifndef COMPAT_H
#define COMPAT_H
/* compat.h:  Copyright 2007 Dennis Holmes  (dholmes@rahul.net) */

#ifdef __cplusplus
extern "C" {
#endif


#if !defined(HAVE_SYS_TIME_H) || defined(TIME_WITH_SYS_TIME)
#include <time.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif


#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME 0
#endif

#ifndef HAVE_CLOCKID_T
typedef int clockid_t;
#endif

#ifndef HAVE_CLOCK_GETTIME
int clock_gettime(clockid_t clock_id, struct timespec *tp);
#endif


#ifndef HAVE_VSNPRINTF
#include <stdarg.h>
int vsnprintf(char *str, size_t size, const char *format, va_list ap);
#endif

#ifndef HAVE_SNPRINTF
#include <stdarg.h>
int snprintf(char *str, size_t size, const char *format, ...);
#endif


#ifndef HAVE_STRNSTR
char *strnstr(const char *haystack, const char *needle, size_t haystacklen);
#endif

#ifndef HAVE_STRNCASECMP
int strncasecmp(const char *s1, const char *s2, size_t len);
#endif

#ifndef HAVE_STRCASECMP
int strcasecmp(const char *s1, const char *s2);
#endif


#ifdef __cplusplus
}
#endif
#endif  // COMPAT_H

