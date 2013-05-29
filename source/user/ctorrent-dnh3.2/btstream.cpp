#include "btstream.h"  // def.h

#include <arpa/inet.h>
#include <inttypes.h>
#include <sys/time.h>
#include <string.h>	// memcpy

#include "msgencode.h"
#include "btconfig.h"

#ifndef HAVE_CLOCK_GETTIME
#include "compat.h"
#endif

size_t get_nl(char *from)
{
  // assumes H_INT_LEN==H_LEN==4
  uint32_t t = 0;
  memcpy(&t, from, H_INT_LEN);
  return (size_t)ntohl((unsigned long)t);
}

void set_nl(char *to, size_t from)
{
  // assumes H_INT_LEN==H_LEN==4
  uint32_t from32 = (uint32_t)htonl((unsigned long)from);
  memcpy(to, &from32, H_INT_LEN);
}

ssize_t btStream::Flush()
{
  return out_buffer.FlushOut(sock);
}

ssize_t btStream::Send_State(unsigned char state)
{
  char msg[H_LEN + H_BASE_LEN];

  set_nl(msg, H_BASE_LEN);
  msg[H_LEN] = (char)state;
  return out_buffer.Put(sock,msg,H_LEN + H_BASE_LEN);
}

ssize_t btStream::Send_Have(size_t idx)
{
  char msg[H_LEN + H_HAVE_LEN];

  set_nl(msg, H_HAVE_LEN);
  msg[H_LEN] = (char)M_HAVE;
  set_nl(msg + H_LEN + H_BASE_LEN, idx);

  return out_buffer.Put(sock,msg,H_LEN + H_HAVE_LEN);
}

ssize_t btStream::Send_Bitfield(char *bit_buf,size_t len)
{
  size_t q = htonl(len + H_BASE_LEN);
  unsigned char t = M_BITFIELD;
  ssize_t r = out_buffer.Put(sock,(char*)&q,H_LEN);
  if(r < 0) return r;
  r = out_buffer.Put(sock,(char*)&t,H_BASE_LEN);
  if(r < 0) return r;
  return out_buffer.Put(sock,bit_buf,len);
}

ssize_t btStream::Send_Cancel(size_t idx,size_t off,size_t len)
{
  char msg[H_LEN + H_CANCEL_LEN];

  set_nl(msg, H_CANCEL_LEN);
  msg[H_LEN] = M_CANCEL;
  set_nl(msg + H_LEN + H_BASE_LEN, idx);
  set_nl(msg + H_LEN + H_BASE_LEN + H_INT_LEN, off);
  set_nl(msg + H_LEN + H_BASE_LEN + H_INT_LEN * 2, len);
  return out_buffer.Put(sock,msg,H_LEN + H_CANCEL_LEN);
}

ssize_t btStream::Send_Piece(size_t idx,size_t off,char *piece_buf,size_t len)
{
  size_t q = htonl(len + H_PIECE_LEN);
  unsigned char t = M_PIECE;
  ssize_t r;

  idx = htonl(idx);
  off = htonl(off);
  if( (r = out_buffer.Put(sock,(char*)&q,H_LEN)) < 0 ) return r;
  if( (r = out_buffer.Put(sock,(char*)&t,H_BASE_LEN)) < 0 ) return r;
  if( (r = out_buffer.Put(sock,(char*)&idx,H_INT_LEN)) < 0) return r;
  if( (r = out_buffer.Put(sock,(char*)&off,H_INT_LEN)) < 0) return r;
  return out_buffer.PutFlush(sock,piece_buf,len);
}

ssize_t btStream::Send_Request(size_t idx, size_t off,size_t len)
{
  char msg[H_LEN + H_REQUEST_LEN];

  set_nl(msg, H_REQUEST_LEN);
  msg[H_LEN] = (char)M_REQUEST;
  set_nl(msg + H_LEN + H_BASE_LEN, idx);
  set_nl(msg + H_LEN + H_BASE_LEN + H_INT_LEN, off);
  set_nl(msg + H_LEN + H_BASE_LEN + H_INT_LEN * 2, len);
  return out_buffer.Put(sock,msg,H_LEN + H_REQUEST_LEN);
}

ssize_t btStream::Send_Keepalive()
{
  size_t i = 0;
  return out_buffer.Put(sock,(char*)&i,H_LEN);
}

int btStream::HaveMessage()
{
  // if message arrived.
  size_t r;
  if( H_LEN <= in_buffer.Count() ){
    r = get_nl(in_buffer.BasePointer());
    if( cfg_max_slice_size + H_LEN + H_PIECE_LEN < r )
      return -1;  //message too long
    if( r + H_LEN <= in_buffer.Count() ) return 1;
  }
  return 0; //no message arrived
}

ssize_t btStream::Feed()
{
  return in_buffer.FeedIn(sock);
}

ssize_t btStream::Feed(Rate *rate)
{
  return Feed(0, rate);
}

ssize_t btStream::Feed(size_t limit, Rate *rate)
{
  ssize_t retval;
  struct timespec nowspec;

  clock_gettime(CLOCK_REALTIME, &nowspec);
  retval = in_buffer.FeedIn(sock, limit);

  if( H_LEN + H_PIECE_LEN < in_buffer.Count() &&
      M_PIECE == in_buffer.BasePointer()[H_LEN] ){
    size_t r = get_nl(in_buffer.BasePointer());
    if( r > H_PIECE_LEN ){
      size_t change;
      if( in_buffer.Count() >= r + H_LEN ){  // have the whole message
        change = r - H_PIECE_LEN - m_oldbytes;
        m_oldbytes = 0;
      }else{
        size_t nbytes = in_buffer.Count() - H_LEN - H_PIECE_LEN;
        change = nbytes - m_oldbytes;
        m_oldbytes = nbytes;
      }
      rate->RateAdd(change, (size_t)cfg_max_bandwidth_down,
        nowspec.tv_sec + (double)(nowspec.tv_nsec)/1000000000);
    }
  }
  return retval;
}

ssize_t btStream::PickMessage()
{
  return in_buffer.PickUp( get_nl(in_buffer.BasePointer()) + H_LEN );
}

ssize_t btStream::Send_Buffer(char *buf, size_t len)
{
  return out_buffer.Put(sock,buf,len);
}

// Does not distinguish between keepalive, choke (msg 0), and no message.
char btStream::PeekMessage()
{
  return ( H_LEN < in_buffer.Count() && get_nl(in_buffer.BasePointer()) ) ?
           in_buffer.BasePointer()[H_LEN] : 0;
}

// Is the next message known to match m?
int btStream::PeekMessage(char m)
{
  return ( H_LEN < in_buffer.Count() && m == in_buffer.BasePointer()[H_LEN] &&
           get_nl(in_buffer.BasePointer()) ) ? 1 : 0;
}

