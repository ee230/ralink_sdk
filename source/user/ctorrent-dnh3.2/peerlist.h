#ifndef PEERLIST_H
#define PEERLIST_H

#include "./def.h"
#include <sys/types.h>
#include <time.h>

#include "./peer.h"
#include "./rate.h"

typedef struct _peernode{
  btPeer *peer;
  struct _peernode *next;
}PEERNODE;

class PeerList
{
 private:
  SOCKET m_listen_sock;
  PEERNODE *m_head, *m_dead;
  size_t m_peers_count, m_seeds_count, m_conn_count;
  size_t m_max_unchoke;
  time_t m_unchoke_check_timestamp, m_keepalive_check_timestamp,
         m_last_progress_timestamp, m_opt_timestamp, m_interval_timestamp;
  time_t m_unchoke_interval, m_opt_interval;
  size_t m_defer_count, m_missed_count, m_upload_count, m_up_opt_count;
  int m_prev_limit_up;
  char m_listen[22];

  unsigned char m_ul_limited:1;
  unsigned char m_f_pause:1;
  unsigned char m_f_limitd:1;
  unsigned char m_f_limitu:1;
  unsigned char m_f_dlate:1;
  unsigned char m_f_ulate:1;
  unsigned char m_reserved:2;
  
  int Accepter();
  int UnChokeCheck(btPeer* peer,btPeer *peer_array[]);
  int FillFDSet(fd_set *rfd, fd_set *wfd, int f_keepalive_check,
    int f_unchoke_check, btPeer **UNCHOKER);
  
 public:
  PeerList();
  ~PeerList();

  // TotalPeers() is now GetPeersCount() for consistency
  int Initial_ListenPort();
  const char *GetListen() const { return m_listen; }

  int IsEmpty() const { return m_peers_count ? 0 : 1; }


  void PrintOut() const;

  int NewPeer(struct sockaddr_in addr, SOCKET sk);

  void CloseAllConnectionToSeed();
  void CloseAll();

  int IntervalCheck(fd_set *rfd, fd_set *wfd);

  void SetUnchokeIntervals();
  void AnyPeerReady(fd_set *rfdp, fd_set *wfdp, int *nready,
    fd_set *rfdnextp, fd_set *wfdnextp);

  int BandWidthLimitUp() { return BandWidthLimitUp(0); }
  int BandWidthLimitUp(double when);
  int BandWidthLimitDown() { return BandWidthLimitDown(0); }
  int BandWidthLimitDown(double when);
  double WaitBW() const;
  void DontWaitBW() { Self.OntimeUL(0); Self.OntimeDL(0); }

  void Tell_World_I_Have(size_t idx);
  btPeer* Who_Can_Abandon(btPeer *proposer);
  size_t What_Can_Duplicate(BitField &bf, btPeer *proposer, size_t idx) const;
  void FindValuedPieces(BitField &bf, btPeer *proposer, int initial) const;
  btPeer *WhoHas(size_t idx) const;
  void CancelSlice(size_t idx, size_t off, size_t len);
  void CancelPiece(size_t idx);
  void CheckBitField(BitField &bf);
  int AlreadyRequested(size_t idx) const;
  size_t Pieces_I_Can_Get() const;
  size_t Pieces_I_Can_Get(BitField *ptmpBitField) const;
  void CheckInterest();
  btPeer* GetNextPeer(btPeer *peer) const;
  int Endgame() const;

  size_t GetSeedsCount() const { return m_seeds_count; }
  size_t GetPeersCount() const { return m_peers_count; }
  size_t GetConnCount() const { return m_conn_count; }
  void AdjustPeersCount();  // passthrough to tracker function

  size_t GetUnchoked() const;
  size_t GetSlowestUp(size_t minimum) const;
  size_t GetDownloads() const;
  size_t GetUnchokeInterval() const { return m_unchoke_interval; }

  void Defer() { m_defer_count++; }
  void Upload() { m_upload_count++; }

  int IsIdle();
  void UnLate() { m_f_dlate = m_f_ulate = 0; }
  void Pause();
  void Resume();
  int IsPaused() const { return m_f_pause ? 1 : 0; }
  void StopDownload();

  void UnchokeIfFree(btPeer *peer);
};

extern PeerList WORLD;

#endif
