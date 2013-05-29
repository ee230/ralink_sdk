#ifndef PEER_H
#define PEER_H

#include "./def.h"

#ifdef WINDOWS
#include <Winsock2.h>
#else
#include <unistd.h>
#include <stdio.h>   // autoconf manual: Darwin + others prereq for stdlib.h
#include <stdlib.h>  // autoconf manual: Darwin prereq for sys/socket.h
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#endif

#include <inttypes.h>
#include <time.h>
#include <string.h>

#include "btrequest.h"
#include "btstream.h"
#include "bitfield.h"
#include "rate.h"
#include "btconfig.h"

#define P_CONNECTING (unsigned char) 0		// connecting
#define P_HANDSHAKE  (unsigned char) 1		// handshaking
#define P_SUCCESS (unsigned char) 2		// successful
#define P_FAILED (unsigned char) 3		// failed

typedef struct _btstatus{
  unsigned char remote_choked:1;
  unsigned char remote_interested:1;
  unsigned char local_choked:1;
  unsigned char local_interested:1;

  unsigned char reserved:4;		/* unused */
}BTSTATUS;

int TextPeerID(const unsigned char *peerid, char *txtid);

class btBasic
{
protected:
  Rate rate_dl;
  Rate rate_ul;

private:
  struct sockaddr_in m_sin;

public:
  //IP地址相关函数
  int IpEquiv(struct sockaddr_in addr);
  void SetIp(struct sockaddr_in addr);
  void SetAddress(struct sockaddr_in addr);
  void GetAddress(struct sockaddr_in *psin) const {
    memcpy(psin,&m_sin,sizeof(struct sockaddr_in));
  }

  // 速率相关函数
  const Rate &GetDLRate() const { return rate_dl; }
  const Rate &GetULRate() const { return rate_ul; }
  void SetDLRate(Rate rate) { rate_dl = rate; StopDLTimer(); }
  void SetULRate(Rate rate) { rate_ul = rate; StopULTimer(); }
  
  uint64_t TotalDL() const { return rate_dl.Count(); }
  uint64_t TotalUL() const { return rate_ul.Count(); }

  void DataRecved(size_t nby) { rate_dl.CountAdd(nby); }
  void DataSended(size_t nby, double timestamp) { rate_ul.CountAdd(nby);
    rate_ul.RateAdd(nby, (size_t)cfg_max_bandwidth_up, timestamp); }

  size_t CurrentDL() { return rate_dl.CurrentRate(); }
  size_t CurrentUL() { return rate_ul.CurrentRate(); }
  size_t RateDL() { return rate_dl.RateMeasure(); }
  size_t RateUL() { return rate_ul.RateMeasure(); }

  void StartDLTimer() { rate_dl.StartTimer(); }
  void StartULTimer() { rate_ul.StartTimer(); }
  void StopDLTimer() { rate_dl.StopTimer(); }
  void StopULTimer() { rate_ul.StopTimer(); }
  void ResetDLTimer() { rate_dl.Reset(); }
  void ResetULTimer() { rate_ul.Reset(); }

  double LastSendTime() const { return rate_ul.LastRealtime(); }
  double LastRecvTime() const { return rate_dl.LastRealtime(); }
  size_t LastSizeSent() const { return rate_ul.LastSize(); }
  size_t LastSizeRecv() const { return rate_dl.LastSize(); }

  Rate *DLRatePtr() { return &rate_dl; }
  Rate *ULRatePtr() { return &rate_ul; }

  double LateDL() const { return rate_dl.Late(); }
  double LateUL() const { return rate_ul.Late(); }
  int OntimeDL() const { return rate_dl.Ontime(); }
  int OntimeUL() const { return rate_ul.Ontime(); }
  void OntimeDL(int yn) { rate_dl.Ontime(yn); }
  void OntimeUL(int yn) { rate_ul.Ontime(yn); }
};

class btPeer:public btBasic
{
 private:
  time_t m_last_timestamp, m_unchoke_timestamp;

  unsigned char m_f_keepalive:1;
  unsigned char m_status:4;
  unsigned char m_bad_health:1;
  unsigned char m_standby:1;
  unsigned char m_want_again:1;  // attempt reconnect if lost

  unsigned char m_connect:1;     // we initiated the connection
  unsigned char m_retried:1;     // already retried connecting
  unsigned char m_connect_seed:1; // connected while I am seed
  unsigned char m_requested:1;   // received a request since unchoke
  unsigned char m_prefetch_completion:2; // prefetched for piece completion
  unsigned char m_reserved:2;

  BTSTATUS m_state;

  size_t m_cached_idx;
  size_t m_err_count;
  size_t m_req_send;  // target number of outstanding requests
  size_t m_req_out;   // actual number of outstanding requests
  size_t m_latency;
  size_t m_prev_dlrate;
  time_t m_latency_timestamp;
  time_t m_health_time, m_receive_time, m_next_send_time;
  char m_lastmsg;
  time_t m_choketime;
  time_t m_prefetch_time;
  time_t m_cancel_time;

  static btPeer *g_next_up, *g_next_dn;
  static unsigned char g_defer_up;
  
  int PieceDeliver(size_t mlen);
  int ReportComplete(size_t idx);
  int RequestCheck();
  int SendRequest();
  int CancelPiece();
  int CancelRequest(PSLICE ps);
  int ReponseSlice();
  int RequestPiece();
  int MsgDeliver();
  int CouldReponseSlice();
  int RequestSlice(size_t idx,size_t off,size_t len);

 public:
  unsigned char id[PEER_ID_LEN];
  BitField bitfield;
  btStream stream;
  RequestQueue request_q;
  RequestQueue reponse_q;

  btPeer();

  void CopyStats(btPeer *peer);

  int RecvModule();
  int SendModule();
  int HealthCheck();
  int CheckSendStatus();

  time_t SetLastTimestamp() { return time(&m_last_timestamp); }
  time_t GetLastTimestamp() const { return m_last_timestamp; }
  time_t SetLastUnchokeTime() { return time(&m_unchoke_timestamp); }
  time_t GetLastUnchokeTime() const { return m_unchoke_timestamp; }

  int Is_Remote_Interested() const { return m_state.remote_interested ? 1 : 0; }
  int Is_Remote_UnChoked() const { return m_state.remote_choked ? 0 : 1; }
  int Is_Local_Interested() const { return m_state.local_interested ? 1 : 0;}
  int Is_Local_UnChoked() const { return m_state.local_choked ? 0 : 1; }
  int SetLocal(unsigned char s);

  int IsEmpty() const;

  int CancelSliceRequest(size_t idx, size_t off, size_t len);
  int CancelPiece(size_t idx);
  
  void SetStatus(unsigned char s){ m_status = s; }
  unsigned char GetStatus() const { return m_status; }
  int NeedWrite(int limited);
  int NeedRead(int limited);

  
  void CloseConnection();
  int CanReconnect() const { return (m_connect && m_want_again && !m_retried) ? 1 : 0; }
  int WantAgain() const { return m_want_again ? 1 : 0; }
  void DontWantAgain() { m_want_again = 0; }
  void SetConnect() { m_connect = 1; }
  void Retry() { m_retried = 1; }
  int Retried() const { return m_retried ? 1 : 0; }

  int ConnectedWhileSeed() const { return m_connect_seed ? 1 : 0; }
  
  int AreYouOK();
  int Send_ShakeInfo();
  int HandShake();

  int Need_Remote_Data() const;
  int Need_Local_Data() const;

  int PutPending();

  int NeedPrefetch() const;
  void Prefetch(time_t deadline);

  void dump();
};

extern btBasic Self;

#endif
