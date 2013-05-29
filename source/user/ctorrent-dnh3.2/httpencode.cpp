#include "./def.h"
#include <sys/types.h>

#include "./httpencode.h"

#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "./config.h"

#if !defined(HAVE_STRNSTR) || !defined(HAVE_STRNCASECMP)
#include "compat.h"
#endif

static void url_encode_char(char *b,char c)
{
  char HEX_TABLE[] = "0123456789ABCDEF";
  b[0] = '%';
  b[1] = HEX_TABLE[(c >> 4) & 0x0F];
  b[2] = HEX_TABLE[c & 0x0F];
}

char* Http_url_encode(char *s,const char *b,size_t n)
{
  size_t r,i;
  for(r = 0,i = 0 ; i < n; i++){
    if( isalpha(b[i]) || isdigit(b[i]) ){
      s[r] = b[i];
      r++;
    }else{
      url_encode_char(s + r, b[i]);
      r += 3;
    }
  }
  s[r] = '\0';
  return s;
}

int Http_url_analyse(const char *url,char *host,int *port,char *path)
{
  const char *p;
  int r;
  *port = 80;	/* default port 80 */
  p = strstr(url,"://");
  if( !p ) 
    p = url;
  else
    p += 3;

  /* host */
  for(; *p && (isalnum(*p) || *p == '.' || *p == '-'); p++, host++) 
    *host = *p;
  *host = '\0';

  if( *p == ':' ){
    /* port */
    p++;
    for( r = 0; p[r] >= '0' && p[r] <= '9' && r < 6; r++) ;

    if( !r ) return -1;
    *port = atoi(p);
    if(*port > 65536) return -1;
    p += r;
  }

  /* path */
  if( *p != '/' ) return -1;
  for( ; *p; p++,path++) *path = *p;
  *path = '\0';
  return 0;
}

size_t Http_split(char *b,size_t n,char **pd,size_t *dlen)
{
  char *p;
  size_t addtion, hlen;

  hlen = 0;

  if( n < 16 ) return 0;	// 长度太小，不可能是一个HTML报文

  if(strncasecmp(b,"HTTP/",5) != 0){
    return 0;			// 没有HTML首部????
    /* message without http header */
    //*pd = b;
    //*dlen = n;
  }else{
    if( p = strnstr(b,"\r\n\r\n",n) ) addtion = 4;
    else if( p = strnstr(b,"\n\n",n) ) addtion = 2;

    if( p ){
      hlen = p - b;
      *pd = ( p + addtion );
      *dlen = n - hlen - addtion;
    }else{		// 只有首部信息????
      hlen = n;
      *pd = (char*) 0;
      *dlen = 0;
    }
  }
  return hlen;
}

int Http_reponse_code(const char *b,size_t n)
{
  int r = -1;

  for(; n && *b != ' ' && *b !='\r' && *b != '\n'; b++,n--) ;
  if( !n || *b != ' ') r = -1;
  else{
          r = atoi(b);
          if( r < 100 || r > 600 ) r = -1;
  }
  return r;
}

int Http_get_header(const char *b,int n,const char *header,char *v)
{
  const char *e;
  char h[64];
  int r,header_len;

  strcpy(h,header);
  strcat(h,": ");
  header_len = strlen(h);

  /* remove status line. */
  e = strchr(b,'\n');
  if( !e ) return -1;
  e++;
  n -= (e - b);
  b = e;

  for(; n >= 0; ){
    e = strchr(b,'\n');  
    if( !e ) r = n;		/* last line */
    else{r = e - b ; r++;}
  
    if( r > header_len ){
      if( strncasecmp(b, h, header_len) == 0){
        /* header found */
        b += header_len;
        for(; *b != '\r' && *b != '\n'; v++,b++) *v = *b;
        *v = '\0';
        return 0;
      }
    }
    b += r;
    n -= r;
  } /* end for */
  return -1;
}
