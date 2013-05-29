#include "./bufio.h"

#ifndef WINDOWS
#include <unistd.h>
#include <stdio.h>   // autoconf manual: Darwin + others prereq for stdlib.h
#include <stdlib.h>  // autoconf manual: Darwin prereq for sys/socket.h
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#endif

#include <string.h>
#include <errno.h>

#include "btrequest.h"

#define _left_buffer_size (n - p)

BufIo::BufIo()
{
  f_socket_remote_closed = 0;
  b = new char[BUF_DEF_SIZ];
#ifndef WINDOWS
  if( !b ) throw 9;
#endif
  p = 0;
  n = BUF_DEF_SIZ;
}

ssize_t BufIo::_realloc_buffer()
{
  return SetSize(n + BUF_INC_SIZ);
}

ssize_t BufIo::SetSize(size_t len)
{
  char *tbuf;

  if( len > BUF_MAX_SIZ ) return -1; // buffer too long

  if( p > len) len = p;
  if( len == n ) return 0;

  tbuf = new char[len];
#ifndef WINDOWS
  if( !tbuf ) return -1;
#endif

  if(p) memcpy(tbuf, b, p);
  delete []b;
  b = tbuf;
  n = len;

  return 0;
}

// retval
// successful return bytes sended. otherwise -1;
ssize_t BufIo::_SEND(SOCKET sk,  char *buf, size_t len)
{
  ssize_t r;
  size_t t = 0;
  for(; len;){
    r = SEND(sk,buf,len);
    if(r < 0){
#ifndef WINDOWS
      if(errno == EINTR) continue;
#endif
      return (EWOULDBLOCK == errno || EAGAIN == errno) ? (ssize_t)t : -1;
    }else if( 0 == r ){
      return (ssize_t)t;			// no possible???
    }else{
      buf += r;
      len -= r;
      t += r;
    }
  }
  return (ssize_t)t;
}

ssize_t BufIo::_RECV(SOCKET sk, char *buf,size_t len)
{
  ssize_t r;
  size_t t = 0;
  for(; len;){
    r = RECV(sk,(char*)buf,len);
    if(r < 0){
#ifndef WINDOWS
      if(errno == EINTR) continue;
#endif
      return (EWOULDBLOCK == errno || EAGAIN == errno) ? (ssize_t)t : -1;
    }else if( 0 == r ){
      f_socket_remote_closed = 1;
      return (ssize_t)t;		//connection closed by remote.
    }else{
      buf += r;
      len -= r;
      t += r;
    }
  }
  return (ssize_t)t;
}

ssize_t BufIo::Put(SOCKET sk, char *buf,size_t len)
{
  ssize_t r;
  if( _left_buffer_size < len ){ //no enough space
    r = FlushOut(sk);
    if( r < 0 ) return r;
    for( ; _left_buffer_size < len; ) // still no enough space
      if(_realloc_buffer() < 0) return -3;
  }
  memcpy(b + p, buf, len);
  p += len;
  return 0;
}

ssize_t BufIo::FeedIn(SOCKET sk)
{
  return FeedIn(sk, _left_buffer_size);
}

ssize_t BufIo::FeedIn(SOCKET sk, size_t limit)
{
  ssize_t r;

  if(!_left_buffer_size)
    if(_realloc_buffer() < 0) return (ssize_t) -2;

  if( 0==limit || limit > _left_buffer_size ) limit = _left_buffer_size;
  r = _RECV(sk, b + p, limit);
  if( r < 0 ) return -1;
  else{
    if( r ) p += r;
    if( f_socket_remote_closed ) return -2; // connection closed by remote
  }
  return (ssize_t) p;
}

ssize_t BufIo::PutFlush(SOCKET sk, char *buf,size_t len)
{
  if( _left_buffer_size < len && p){
    if( FlushOut(sk) < 0) return -1;
  }

  for(; _left_buffer_size < len; )
    if( _realloc_buffer() < 0) return -3;

  memcpy(b + p, buf, len);
  p += len;
  return FlushOut(sk);
}

// retval
// >= 0 left bytes in buffer
// < 0 failed
ssize_t BufIo::FlushOut(SOCKET sk)
{
  ssize_t r;
  if( !p ) return 0;		// no data to be send
  
  r = _SEND(sk,b,p);
  if( r < 0 ) return r;
  else if( r > 0){
    p -= r;
    if( p ) memmove(b, b + r, p);
  }
  return (ssize_t)p;
}

ssize_t BufIo::PickUp(size_t len)
{
  if( p < len ) return -1;
  p -= len;
  if( p ) memmove(b, b + len, p);
  return 0;
}
