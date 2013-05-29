#ifndef BTSTREAM_H
#define BTSTREAM_H

#include "./def.h"
#include "./bufio.h"

#ifdef WINDOWS
#include <Winsock2.h>
#else
#include <unistd.h>
#endif

#include "rate.h"

size_t get_nl(char *from);
void set_nl(char *to, size_t from);

class btStream
{
private:
  SOCKET sock, sock_was;
  size_t m_oldbytes;

public:
  BufIo in_buffer;
  BufIo out_buffer;

  btStream() { sock = sock_was = INVALID_SOCKET; m_oldbytes = 0; }
  ~btStream() { if( INVALID_SOCKET != sock) CLOSE_SOCKET(sock); }

  SOCKET GetSocket() { return (INVALID_SOCKET==sock) ? sock_was : sock; }
  void SetSocket(SOCKET sk){ sock = sk; }

  void Close(){
    if( INVALID_SOCKET != sock ){
      CLOSE_SOCKET(sock);
      sock_was = sock;
      sock = INVALID_SOCKET;
    }
    in_buffer.Close();
    out_buffer.Close();
  }

  ssize_t PickMessage(); //移除接收缓存中的一条消息
  ssize_t Feed();
  ssize_t Feed(Rate *rate);
  ssize_t Feed(size_t limit, Rate *rate);

  int HaveMessage();  // 返回值 1: 缓存中有消息 0: 暂无消息 -1: 失败
  char PeekMessage();
  int PeekMessage(char m);

  ssize_t Send_Keepalive();
  ssize_t Send_State(unsigned char state);
  ssize_t Send_Have(size_t idx);
  ssize_t Send_Piece(size_t idx,size_t off,char *piece_buf,size_t len);
  ssize_t Send_Bitfield(char *bit_buf,size_t len);
  ssize_t Send_Request(size_t idx,size_t off,size_t len);
  ssize_t Send_Cancel(size_t idx,size_t off,size_t len);
  ssize_t Send_Buffer(char *buf,size_t len);

  ssize_t Flush();
};

#endif
