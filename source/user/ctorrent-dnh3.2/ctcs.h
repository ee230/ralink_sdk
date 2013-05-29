#ifndef CTCS_H
#define CTCS_H

#include "./def.h"
#include <sys/types.h>

#ifdef WINDOWS
#include <Winsock2.h>
#else
#include <unistd.h>
#include <netdb.h>   // Solaris defines MAXHOSTNAMELEN here.
#include <stdio.h>   // autoconf manual: Darwin + others prereq for stdlib.h
#include <stdlib.h>  // autoconf manual: Darwin prereq for sys/socket.h
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/param.h>
#endif

#include <inttypes.h>
#include <time.h>

#include "bufio.h"

#define CTCS_BUFSIZE (200+MAXPATHLEN)
#define CTCS_PASS_SIZE 21

struct ctstatus {
  size_t seeders, leechers, nhave, ntotal, dlrate, ulrate,
    dlimit, ulimit, cacheused;
  uint64_t dltotal, ultotal;

  ctstatus(){
    seeders=leechers=nhave=ntotal=dlrate=ulrate=dltotal=
    ultotal=dlimit=ulimit=cacheused = 0;
  }
};

class Ctcs
{
 private:
  char m_host[MAXHOSTNAMELEN];
  int m_port;
  char m_pass[CTCS_PASS_SIZE];
  int m_protocol;

  struct sockaddr_in m_sin;

  unsigned char m_status:2;

  time_t m_interval;
  time_t m_last_timestamp;
  time_t m_sent_ctstatus_time;
  time_t m_statustime;

  SOCKET m_sock;
  BufIo in_buffer;
  BufIo out_buffer;
  struct ctstatus m_ctstatus;
  int m_sent_ctstatus;
  int m_sent_ctbw;

  int _s2sin(char *h,int p,struct sockaddr_in *psin);
  int SendMessage(const char *buf);
  char *ConfigMsg(const char *name, const char *type, const char *range,    
    const char *value, const char *short_desc, const char *long_desc);

 public:
  Ctcs();
  ~Ctcs();

  void Reset(time_t new_interval);
  int Initial();
  int Connect();
  int CheckMessage();
  int Send_Protocol();
  int Send_Auth();
  int Send_Torrent(const unsigned char *peerid, char *torrent);
  int Report_Status(size_t seeders, size_t leechers, size_t nhave,
    size_t ntotal, size_t dlrate, size_t ulrate,
    uint64_t dltotal, uint64_t ultotal, size_t dlimit, size_t ulimit,
    size_t cacheused);
  int Send_Status();
  int Send_bw();
  int Send_Config();
  int Set_Config(char *msgbuf);
  int Send_Detail();
  int Send_Peers();
  int Send_Info(int sev, const char *info);
  int IntervalCheck(fd_set *rfdp, fd_set *wfdp);
  int SocketReady(fd_set *rfdp, fd_set *wfdp, int *nfds,
    fd_set *rfdnextp, fd_set *wfdnextp);
  void RestartTracker();

  SOCKET GetSocket() { return m_sock; }
  unsigned char GetStatus() { return m_status;}
};

extern Ctcs CTCS;

#endif

