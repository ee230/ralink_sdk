/* small demo app that just includes a cipher/hash/prng */
#include <tomcrypt.h>

int main(void)
{
   register_cipher(&rijndael_enc_desc);
   register_prng(&yarrow_desc);
   register_hash(&sha256_desc);
   return 0;
}

/* $Source: /home/cvsroot/RT288x_SDK/source/user/dropbear-0.52/libtomcrypt/demos/small.c,v $ */
/* $Revision: 1.1 $ */
/* $Date: 2010-07-02 06:29:10 $ */
