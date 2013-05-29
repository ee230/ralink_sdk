#ifndef TRACKER_H
#define TRACKER_H

#include "./def.h"
#include <sys/types.h>

#include "./bufio.h"

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

#include <time.h>

#include "btconfig.h"

#define T_FREE 		0
#define T_CONNECTING	1
#define T_READY		2
#define T_FINISHED	3

class btTracker
{
 private:
  char m_host[MAXHOSTNAMELEN];
  char m_path[MAXPATHLEN];
  int m_port;
  char m_key[9];
  char m_trackerid[PEER_ID_LEN+1];

  struct sockaddr_in m_sin;

  unsigned char m_status:2;
  unsigned char m_f_started:1;
  unsigned char m_f_stoped:1;
  unsigned char m_f_completed:1;
  unsigned char m_f_restart:1;
  unsigned char m_f_boguspeercnt:1;
  unsigned char m_reserved:1;


  time_t m_interval;		// 与Tracker通信的时间间隔
  time_t m_default_interval;		// interval that the tracker tells us to wait
  time_t m_last_timestamp;	// 最后一次成功与Tracker通信的时间
  size_t m_connect_refuse_click;

  size_t m_ok_click;	// tracker ok response counter
  size_t m_peers_count;	// total number of peers
  size_t m_seeds_count;	// total number of seeds
  size_t m_prevpeers;	// number of peers previously seen

  SOCKET m_sock;
  BufIo m_request_buffer, m_reponse_buffer;
  
  int _IPsin(char *h, int p, struct sockaddr_in *psin);
  int _s2sin(char *h,int p,struct sockaddr_in *psin);
  int _UpdatePeerList(char *buf,size_t bufsiz);

  int BuildBaseRequest();
  int Connect();
  int SendRequest();
  int CheckReponse();
  void Restart();

 public:
  btTracker();
  ~btTracker();

  int Initial();

  void Reset(time_t new_interval);

  unsigned char GetStatus() { return m_status; }
  void SetStatus(unsigned char s) { m_status = s; }

  SOCKET GetSocket() { return m_sock; }

  void SetRestart() { m_f_restart = 1; }
  void ClearRestart() { m_f_restart = 0; }
  int IsRestarting() const { return m_f_restart; }
  int IsQuitting() const { return m_f_stoped; }
  void SetStoped();

  int IntervalCheck(fd_set* rfdp, fd_set *wfdp);
  int SocketReady(fd_set *rfdp, fd_set *wfdp, int *nfds,
    fd_set *rfdnextp, fd_set *wfdnextp);

  size_t GetRefuseClick() const { return m_connect_refuse_click; }
  size_t GetOkClick() const { return m_ok_click; }
  size_t GetPeersCount() const;
  size_t GetSeedsCount() const;
  void AdjustPeersCount() {
    if(m_f_boguspeercnt && m_peers_count) m_peers_count--; }
  time_t GetInterval() const { return m_default_interval; }
};

extern btTracker Tracker;

#endif
