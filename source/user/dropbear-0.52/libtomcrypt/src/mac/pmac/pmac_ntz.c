/* LibTomCrypt, modular cryptographic library -- Tom St Denis
 *
 * LibTomCrypt is a library that provides various cryptographic
 * algorithms in a highly modular and flexible manner.
 *
 * The library is free for all purposes without any express
 * guarantee it works.
 *
 * Tom St Denis, tomstdenis@gmail.com, http://libtomcrypt.com
 */
#include "tomcrypt.h"

/** 
   @file pmac_ntz.c
   PMAC implementation, internal function, by Tom St Denis 
*/

#ifdef LTC_PMAC

/**
  Internal PMAC function
*/
int pmac_ntz(unsigned long x)
{
   int c;
   x &= 0xFFFFFFFFUL;
   c = 0;
   while ((x & 1) == 0) {
      ++c;
      x >>= 1;
   }
   return c;
}

#endif

/* $Source: /home/cvsroot/RT288x_SDK/source/user/dropbear-0.52/libtomcrypt/src/mac/pmac/pmac_ntz.c,v $ */
/* $Revision: 1.1 $ */
/* $Date: 2010-07-02 06:29:15 $ */
