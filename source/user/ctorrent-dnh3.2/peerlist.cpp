#include "peerlist.h"  // def.h

#include <sys/types.h>

#include <stdlib.h>

#include <stdio.h>
#include <errno.h>

#include <string.h>

#include <sys/time.h>

#include "btconfig.h"
#include "connect_nonb.h"
#include "setnonblock.h"
#include "btcontent.h"
#include "msgencode.h"

#include "iplist.h"
#include "tracker.h"
#include "ctcs.h"
#include "bttime.h"
#include "console.h"

#if !defined(HAVE_CLOCK_GETTIME) || !defined(HAVE_SNPRINTF)
#include "compat.h"
#endif

#define MIN_UNCHOKES 3
#define MIN_OPT_CYCLE 3
#define MIN_UNCHOKE_INTERVAL 10

#define KEEPALIVE_INTERVAL 117

#define PEER_IS_SUCCESS(peer) (P_SUCCESS == (peer)->GetStatus())
#define PEER_IS_FAILED(peer) (P_FAILED == (peer)->GetStatus())
#define NEED_MORE_PEERS() (m_peers_count < cfg_max_peers)

const char LIVE_CHAR[4] = {'-', '\\','|','/'};

PeerList WORLD;

PeerList::PeerList()
{
  m_unchoke_check_timestamp =
    m_keepalive_check_timestamp =
    m_opt_timestamp =
    m_interval_timestamp = time((time_t*) 0);
  m_unchoke_interval = MIN_UNCHOKE_INTERVAL;
  m_opt_interval = MIN_OPT_CYCLE * MIN_UNCHOKE_INTERVAL;

  m_head = m_dead = (PEERNODE*) 0;
  m_listen_sock = INVALID_SOCKET;
  m_peers_count = m_seeds_count = m_conn_count = 0;
  m_f_pause = m_f_dlate = m_f_ulate = 0;
  m_max_unchoke = MIN_UNCHOKES;
  m_defer_count = m_missed_count = 0;
  m_upload_count = m_up_opt_count = 0;
  m_prev_limit_up = cfg_max_bandwidth_up;
}

PeerList::~PeerList()
{
  PEERNODE *p,*pnext;
  for( p = m_head; p ; ){
    pnext = p->next;
    delete p->peer;
    delete p;
    p = pnext;
  }
  for( p = m_dead; p ; ){
    pnext = p->next;
    delete p->peer;
    delete p;
    p = pnext;
  }
}

void PeerList::CloseAll()
{
  PEERNODE *p;
  for( p = m_head; p; ){
    m_head = p->next;
    delete (p->peer);
    delete p;
    p = m_head;
  }
}

int PeerList::NewPeer(struct sockaddr_in addr, SOCKET sk)
{
  PEERNODE *p, *pp, *pnext;
  btPeer *peer = (btPeer*) 0;
  int r;

  if( m_peers_count >= cfg_max_peers ){
    if( INVALID_SOCKET != sk ) CLOSE_SOCKET(sk);
    return -4;
  }

  if( INVALID_SOCKET != sk && Self.IpEquiv(addr) ){
    if(arg_verbose)
      CONSOLE.Debug("Connection from myself %s", inet_ntoa(addr.sin_addr));
    Tracker.AdjustPeersCount();
    if( INVALID_SOCKET != sk ) CLOSE_SOCKET(sk);
    return -3;
  }

  for( p = m_head; p; p = p->next ){
    if(PEER_IS_FAILED(p->peer)) continue;
    if( p->peer->IpEquiv(addr) ){  // already exist.
      if( INVALID_SOCKET != sk ){
        if(arg_verbose) CONSOLE.Debug("Connection from duplicate peer %s",
          inet_ntoa(addr.sin_addr));
        CLOSE_SOCKET(sk); 
      }
      return -3;
    }
  }

  // See if we've had this peer before, and maintain its stats.
  // Do it here instead of later to insure we purge old entries periodically.
  pp = (PEERNODE *)0;
  for( p = m_dead; p; p = pnext ){
    if( p->peer->IpEquiv(addr) ) break;
    else{
      pnext = p->next;
      if( p->peer->GetLastTimestamp() + 2 * Tracker.GetInterval() < now ){
        delete p->peer;
        if( pp ) pp->next = p->next;
        else m_dead = p->next;
        delete p;
      }else pp = p;
    }
  }

  if( INVALID_SOCKET == sk ){
    if( INVALID_SOCKET == (sk = socket(AF_INET,SOCK_STREAM,0)) ) return -1;

    if( setfd_nonblock(sk) < 0) goto err;

    if( -1 == (r = connect_nonb(sk,(struct sockaddr*)&addr)) ){
      if(arg_verbose) CONSOLE.Debug("Connect to peer at %s:%hu failed:  %s",
        inet_ntoa(addr.sin_addr), ntohs(addr.sin_port), strerror(errno));
      return -1;
    }

    peer = new btPeer;
#ifndef WINDOWS
    if( !peer ) goto err;
#endif

    peer->SetConnect();
    peer->SetAddress(addr);
    peer->stream.SetSocket(sk);
    peer->SetStatus( (-2 == r) ? P_CONNECTING : P_HANDSHAKE );
    if(arg_verbose) CONSOLE.Debug("Connecting to %s:%hu (peer %p)",
        inet_ntoa(addr.sin_addr), ntohs(addr.sin_port), peer);

  }else{
    if( setfd_nonblock(sk) < 0) goto err;

    peer = new btPeer;
#ifndef WINDOWS
    if( !peer ) goto err;
#endif

    peer->SetAddress(addr);
    peer->stream.SetSocket(sk);
    peer->SetStatus(P_HANDSHAKE);
    if(arg_verbose) CONSOLE.Debug("Connection from %s:%hu (peer %p)",
        inet_ntoa(addr.sin_addr), ntohs(addr.sin_port), peer);
  }

  if( !BTCONTENT.Seeding() &&
      peer->stream.in_buffer.SetSize(BUF_DEF_SIZ + cfg_req_slice_size) < 0 )
    goto err;

  if( P_HANDSHAKE == peer->GetStatus() )
    if( peer->Send_ShakeInfo() != 0 ) goto err;

  if( p ){   // resurrected! (reconnected with an old peer)
    if( pp ) pp->next = p->next;
    else m_dead = p->next;
    peer->CopyStats(p->peer);
    delete p->peer;
  }else{
    p = new PEERNODE;
#ifndef WINDOWS
    if( !p ) goto err;
#endif
  }

  m_peers_count++;
  p->peer = peer;
  p->next = m_head;
  m_head = p;
  return 0;

 err:
  if( peer ) delete peer;
  if( INVALID_SOCKET != sk ) CLOSE_SOCKET(sk);
  return -1;
}

int PeerList::IntervalCheck(fd_set *rfdp, fd_set *wfdp)
{
  int f_keepalive_check = 0;
  int f_unchoke_check = 0;
  int i = 0;
  btPeer **UNCHOKER;

  // No pause check here--stay ready by continuing to acquire peers.
  if( !Tracker.IsQuitting() ){
    struct sockaddr_in addr;
    for( ; NEED_MORE_PEERS() && !IPQUEUE.IsEmpty(); ){
      if(IPQUEUE.Pop(&addr) < 0) break;
      if(NewPeer(addr,INVALID_SOCKET) == -4) break;
    }
  }

  m_ul_limited = BandWidthLimitUp(Self.LateUL());

  // After seeding a while, disconnect uninterested peers & shrink in_buffers.
  if( now - BTCONTENT.GetSeedTime() <= 301 &&
      now - BTCONTENT.GetSeedTime() >= 300 )
    CloseAllConnectionToSeed();
    
  if( KEEPALIVE_INTERVAL <= now - m_keepalive_check_timestamp ){
    m_keepalive_check_timestamp = now;
    f_keepalive_check = 1;
  }

  if( m_unchoke_interval <= now - m_unchoke_check_timestamp && m_head &&
      !m_f_pause ){
    f_unchoke_check = 1;

    if( m_missed_count > m_upload_count && cfg_max_bandwidth_up ){
      size_t unchokes = GetUnchoked();  // already adds one (opt)
      if( unchokes < MIN_UNCHOKES ) m_max_unchoke = MIN_UNCHOKES;
      else{
        m_max_unchoke = unchokes;
        if(arg_verbose)
          CONSOLE.Debug("max unchokes up to %d", (int)m_max_unchoke);
      }
    }else if(arg_verbose) CONSOLE.Debug("UL missed %d sending %d",
      (int)m_missed_count, (int)m_upload_count);
    m_up_opt_count += m_upload_count;
    m_missed_count = m_upload_count = 0;

    if( m_opt_interval && m_opt_interval <= now - m_opt_timestamp ){
      m_opt_timestamp = 0;
      if( m_defer_count > m_up_opt_count &&
          m_max_unchoke > MIN_UNCHOKES && cfg_max_bandwidth_up ){
        m_max_unchoke--;
        if(arg_verbose)
          CONSOLE.Debug("max unchokes down to %d", (int)m_max_unchoke);
      }else if(arg_verbose) CONSOLE.Debug("UL deferred %d sending %d",
        (int)m_defer_count, (int)m_up_opt_count);
      m_defer_count = m_up_opt_count = 0;
    }

    if( 0==cfg_max_bandwidth_up ) m_max_unchoke = MIN_UNCHOKES;

    UNCHOKER = new btPeer *[m_max_unchoke + 1];
    if( UNCHOKER ) memset(UNCHOKER, 0, (m_max_unchoke + 1) * sizeof(btPeer*));
    else CONSOLE.Warning(1, "warn, failed to allocate unchoke array.");

    SetUnchokeIntervals();
  }else{  // no unchoke check
    if( now < m_unchoke_check_timestamp ) m_unchoke_check_timestamp = now;
    if( MIN_UNCHOKE_INTERVAL <= now - m_interval_timestamp ){
      m_interval_timestamp = now;
      // If up bw limit has changed enough, recompute the intervals.
      // This is primarily to prevent a low limit from delaying an unchoke for
      // a long time even after the limit has been increased.
      if( !BandWidthLimitUp() ||
          ( m_prev_limit_up &&
            abs((int)cfg_max_bandwidth_up - (int)m_prev_limit_up) /
                (double)m_prev_limit_up  >
              1 / (double)m_unchoke_interval &&
            ( cfg_max_bandwidth_up < cfg_req_slice_size * (MIN_OPT_CYCLE-1) /
                                     (MIN_UNCHOKE_INTERVAL * MIN_OPT_CYCLE) ||
              m_prev_limit_up < cfg_req_slice_size * (MIN_OPT_CYCLE-1) /
                                (MIN_UNCHOKE_INTERVAL * MIN_OPT_CYCLE) ) ) ){
        SetUnchokeIntervals();
      }
    }else if( now < m_interval_timestamp ) m_interval_timestamp = now;
  }

  if( cfg_cache_size && !m_f_pause && IsIdle() ){
    int f_idle = 1;
    for( PEERNODE *p = m_head; p; p = p->next ){
      if( p->peer->NeedPrefetch() ){
        if( f_idle || IsIdle() ){
          p->peer->Prefetch(m_unchoke_check_timestamp + m_unchoke_interval);
          f_idle = 0;
        }else break;
      }
    }
  }

  return FillFDSet(rfdp, wfdp, f_keepalive_check, f_unchoke_check, UNCHOKER);
}

int PeerList::FillFDSet(fd_set *rfdp, fd_set *wfdp, int f_keepalive_check,
  int f_unchoke_check, btPeer **UNCHOKER)
{
  PEERNODE *p;
  PEERNODE *pp = (PEERNODE*) 0;
  int maxfd = -1;
  SOCKET sk = INVALID_SOCKET;

  m_f_limitu = BandWidthLimitUp(Self.LateUL());
  m_f_limitd = BandWidthLimitDown(Self.LateDL());

 again:
  m_seeds_count = 0;
  m_conn_count = 0;
  size_t interested_count = 0;
  for( p = m_head; p; ){
    sk = p->peer->stream.GetSocket();
    if( PEER_IS_FAILED(p->peer) ){
      if( sk != INVALID_SOCKET ){
        FD_CLR(sk,rfdp);
        FD_CLR(sk,wfdp);
      }
      if( p->peer->CanReconnect() ){ // connect to this peer again
        if(arg_verbose) CONSOLE.Debug("Adding %p for reconnect", p->peer);
        p->peer->Retry();
        struct sockaddr_in addr;
        p->peer->GetAddress(&addr);
        IPQUEUE.Add(&addr);
      }
      if( pp ) pp->next = p->next; else m_head = p->next;
      if( p->peer->TotalDL() || p->peer->TotalUL() ){  // keep stats
        p->peer->SetLastTimestamp();
        p->next = m_dead;
        m_dead = p;
      }else{
        delete p->peer;
        delete p;
      }
      m_peers_count--;
      if( pp ) p = pp->next; else p = m_head;
      continue;
    }else{
      if( !PEER_IS_SUCCESS(p->peer) ) m_conn_count++;
      else{
        if( p->peer->bitfield.IsFull() ) m_seeds_count++;
        if( p->peer->Is_Local_Interested() ) interested_count++;
      }
      if( f_keepalive_check ){
        if( 3 * KEEPALIVE_INTERVAL <= now - p->peer->GetLastTimestamp() ){
          if(arg_verbose) CONSOLE.Debug("close: keepalive expired");
          p->peer->CloseConnection();
          goto skip_continue;
        }
        if( PEER_IS_SUCCESS(p->peer) && 
            KEEPALIVE_INTERVAL <= now - p->peer->GetLastTimestamp() &&
            p->peer->AreYouOK() < 0 ){
          if(arg_verbose) CONSOLE.Debug("close: keepalive death");
          p->peer->CloseConnection();
          goto skip_continue;
        }
      }
      if( f_unchoke_check && PEER_IS_SUCCESS(p->peer) ){
        if( p->peer->Is_Remote_Interested() && p->peer->Need_Local_Data() ){
          if( UNCHOKER && UnChokeCheck(p->peer, UNCHOKER) < 0 )
            goto skip_continue;
        }else if(p->peer->SetLocal(M_CHOKE) < 0){
          if(arg_verbose) CONSOLE.Debug("close: Can't choke peer");
          p->peer->CloseConnection();
          goto skip_continue;
        }
      }

      if( PEER_IS_FAILED(p->peer) ) goto skip_continue;  // failsafe
      if(maxfd < sk) maxfd = sk;
      if( !FD_ISSET(sk,rfdp) && p->peer->NeedRead((int)m_f_limitd) )
        FD_SET(sk,rfdp);
      if( !FD_ISSET(sk,wfdp) && p->peer->NeedWrite((int)m_f_limitu) )
        FD_SET(sk,wfdp);

    skip_continue: 
      if( PEER_IS_FAILED(p->peer) ){
        FD_CLR(sk,rfdp);
        FD_CLR(sk,wfdp);
      }
      pp = p;
      p = p->next;
    }
  } // end for
  if( (m_f_limitu && !(m_f_limitu = BandWidthLimitUp(Self.LateUL()))) ||
      (m_f_limitd && !(m_f_limitd = BandWidthLimitDown(Self.LateDL()))) )
    goto again;

  if( 0==interested_count ) Self.StopDLTimer();

  if( INVALID_SOCKET != m_listen_sock && m_peers_count < cfg_max_peers){
    FD_SET(m_listen_sock, rfdp);
    if( maxfd < m_listen_sock ) maxfd = m_listen_sock;
  }

  if( f_unchoke_check && UNCHOKER ){
    m_unchoke_check_timestamp = now;  // time of the last unchoke check
    if (!m_opt_timestamp) m_opt_timestamp = now;

    if( !UNCHOKER[0] ) Self.StopULTimer();

    for( int i = 0; i < m_max_unchoke + 1; i++ ){
      if( !UNCHOKER[i] ) break;

      if( PEER_IS_FAILED(UNCHOKER[i]) ) continue;

      sk = UNCHOKER[i]->stream.GetSocket();

      if( UNCHOKER[i]->SetLocal(M_UNCHOKE) < 0 ){
        if(arg_verbose) CONSOLE.Debug("close: Can't unchoke peer");
        UNCHOKER[i]->CloseConnection();
        FD_CLR(sk,rfdp);
        FD_CLR(sk,wfdp);
        continue;
      }

      if( !FD_ISSET(sk,wfdp) && UNCHOKER[i]->NeedWrite((int)m_f_limitu) ){
        FD_SET(sk,wfdp);
        if( maxfd < sk) maxfd = sk;
      }
    } // end for
    delete []UNCHOKER;
  }

  return maxfd;
}

void PeerList::SetUnchokeIntervals()
{
  time_t old_unchoke_int = m_unchoke_interval, old_opt_int = m_opt_interval;

  // Unchoke peers long enough to have a chance at getting some data.
  if( BandWidthLimitUp() && BTCONTENT.Seeding() ){
    int optx = (int)( 1 / (1 - (double)MIN_UNCHOKE_INTERVAL *
                               cfg_max_bandwidth_up / cfg_req_slice_size) );
    if( optx < 0 ) optx = 0;
    if( optx < MIN_OPT_CYCLE ){
      optx = MIN_OPT_CYCLE;
      double interval = cfg_req_slice_size /
           (cfg_max_bandwidth_up * MIN_OPT_CYCLE / (double)(MIN_OPT_CYCLE-1));
      m_unchoke_interval = (size_t)interval;
      if( interval - (int)interval > 0 ) m_unchoke_interval++;
      if( m_unchoke_interval < MIN_UNCHOKE_INTERVAL )
        m_unchoke_interval = MIN_UNCHOKE_INTERVAL;
    }else{
      // Allow each peer at least 60 seconds unchoked.
      m_unchoke_interval = MIN_UNCHOKE_INTERVAL;
      if( m_max_unchoke+1 < 60 / m_unchoke_interval ){
        int maxopt = (int)( 1 / (1 - (double)(m_max_unchoke+1) *
                                     m_unchoke_interval / 60) );
        if( maxopt > MIN_OPT_CYCLE && optx > maxopt ) optx = maxopt;
      }
      if( optx > m_max_unchoke+2 ) optx = m_max_unchoke+2;
    }
    m_opt_interval = optx * m_unchoke_interval;
  }else if( BandWidthLimitUp() && !BTCONTENT.Seeding() ){
    // Need to be able to upload a slice per interval.
    double interval = cfg_req_slice_size / (double)cfg_max_bandwidth_up;
    m_unchoke_interval = (size_t)interval;
    if( interval - (int)interval > 0 ) m_unchoke_interval++;
    if( m_unchoke_interval < MIN_UNCHOKE_INTERVAL )
      m_unchoke_interval = MIN_UNCHOKE_INTERVAL;
    m_opt_interval = MIN_OPT_CYCLE * m_unchoke_interval;
  }else{
    m_unchoke_interval = MIN_UNCHOKE_INTERVAL;
    m_opt_interval = MIN_OPT_CYCLE * MIN_UNCHOKE_INTERVAL;
  }
  m_prev_limit_up = cfg_max_bandwidth_up;
  m_interval_timestamp = now;
  if( arg_verbose && (m_unchoke_interval != old_unchoke_int ||
      m_opt_interval != old_opt_int) )
    CONSOLE.Debug("ulimit %d, unchoke interval %d, opt interval %d",
      (int)cfg_max_bandwidth_up, (int)m_unchoke_interval, (int)m_opt_interval);
}

btPeer* PeerList::Who_Can_Abandon(btPeer *proposer)
{
  PEERNODE *p;
  btPeer *peer = (btPeer*) 0;
  for( p = m_head; p; p = p->next ){
    if(!PEER_IS_SUCCESS(p->peer) || p->peer == proposer ||
       p->peer->request_q.IsEmpty() ) continue;

    if(proposer->bitfield.IsSet(p->peer->request_q.GetRequestIdx())){
      if(!peer){
        if( p->peer->RateDL() < proposer->RateDL() ) peer = p->peer;
      }else{
        if( p->peer->RateDL() < peer->RateDL() ) peer = p->peer;
      }
    }
  }//end for
  return peer;
}

// This takes an index parameter to facilitate modification of the function to
// allow targeting of a specific piece.  It's currently only used as a flag to
// specify endgame or initial-piece mode though.
size_t PeerList::What_Can_Duplicate(BitField &bf, btPeer *proposer, size_t idx)
const
{
  PEERNODE *p;
  btPeer *peer = (btPeer*) 0;
  int endgame;
  PSLICE ps;
  size_t piece, piece1, qsize, mark, bench;
  unsigned long rndbits;
  int r=0;

  endgame = idx < BTCONTENT.GetNPieces();	// else initial-piece mode
  if(endgame) mark = 0;
  else mark = cfg_req_queue_length;
  bench = BTCONTENT.GetNPieces();

  // In initial mode, only dup a piece with trade value.
  // In endgame mode, dup any if there are no pieces with trade value.
  FindValuedPieces(bf, proposer, !endgame);
  if( bf.IsEmpty() ){
    if(endgame) bf = proposer->bitfield;
    else return BTCONTENT.GetNPieces();
  }

  /* In endgame mode, select from peers with the longest request queue.
     In initial mode, select from peers with the shortest non-empty request
     queue.  */
  piece1 = BTCONTENT.GetNPieces();
  for( p = m_head; p; p = p->next ){
    if( !PEER_IS_SUCCESS(p->peer) || p->peer == proposer ) continue;

    if( p->peer->request_q.IsEmpty() ) continue;

    piece = BTCONTENT.GetNPieces();
    ps = p->peer->request_q.GetHead();
    for( ; ps; ps = ps->next ){
      if( piece == ps->index || bench == ps->index || piece1 == ps->index ||
          !bf.IsSet(ps->index) || proposer->request_q.HasIdx(ps->index) )
        continue;
      piece = ps->index;

      qsize = p->peer->request_q.Qlen(piece);
      if( (endgame && qsize > mark) ||
          (!endgame && (qsize < mark || !peer)) ){
        mark = qsize;
        peer = p->peer;
        piece1 = piece;
      }else if( qsize == mark ){
        if( !r-- ){
          rndbits = random();
          r = 30;
        }
        if( bench != piece && (rndbits>>=1)&01 ){
          bench = piece1;
          peer = p->peer;
          piece1 = piece;
        }
      }
    }
  }
  return peer ? piece1 : BTCONTENT.GetNPieces();
}

void PeerList::FindValuedPieces(BitField &bf, btPeer *proposer, int initial)
const
{
  PEERNODE *p;
  BitField bf_all_have = bf, bf_int_have = bf,
    bf_others_have, bf_only_he_has = bf, bf_prefer;

  for( p = m_head; p; p = p->next ){
    if( !PEER_IS_SUCCESS(p->peer) || p->peer == proposer ) continue;
    if( p->peer->Need_Remote_Data() )
      bf_int_have.And(p->peer->bitfield);
    bf_all_have.And(p->peer->bitfield);
    if( !initial && !p->peer->bitfield.IsFull() )
      bf_only_he_has.Except(p->peer->bitfield);
    else bf_others_have.Comb(p->peer->bitfield);
  }
  /* bf_all_have is now pertinent pieces that all peers have
     bf_int_have is pertinent pieces that all peers in which I'm interested have
     We prefer to get pieces that those peers need, if we can.  Otherwise go
     for pieces that any peer needs in hopes of future reciprocation. */
  if( !bf_int_have.IsFull() )
    bf_all_have = bf_int_have;
  bf_all_have.Invert();
  bf.And(bf_all_have); // bf is now pertinent pieces that not everyone has

  bf_prefer = initial ? bf_others_have : bf_only_he_has;

  BitField tmpBitField = bf;
  tmpBitField.And(bf_prefer);
  /* If initial mode, tmpBitField is now pertinent pieces that more than one
     peer has, but not everyone.
     Otherwise, it's pertinent pieces that only the proposer has (not
     considering what other seeders have).
     In either case if there are no such pieces, revert to the simple answer.*/
  if( !tmpBitField.IsEmpty() ) bf = tmpBitField;
}

/* Find a peer with the given piece in its request queue.
   Duplicating a request queue that's in progress rather than creating a new
   one helps avoid requesting slices that we already have. */
btPeer *PeerList::WhoHas(size_t idx) const
{
  PEERNODE *p;
  btPeer *peer = (btPeer*) 0;

  for( p = m_head; p; p = p->next ){
    if( p->peer->request_q.HasIdx(idx) ){
      peer = p->peer;
      break;
    }
  }
  return peer;
}

void PeerList::CancelSlice(size_t idx, size_t off, size_t len)
{
  PEERNODE *p;
  PSLICE ps;

  for( p = m_head; p; p = p->next ){
    if( !PEER_IS_SUCCESS(p->peer) ) continue;

    if( p->peer->CancelSliceRequest(idx,off,len) < 0 ){
      if(arg_verbose) CONSOLE.Debug("close: CancelSlice");
      p->peer->CloseConnection();
    }
  }
}

void PeerList::CancelPiece(size_t idx)
{
  PEERNODE *p;
  PSLICE ps;

  for( p = m_head; p; p = p->next ){
    if( !PEER_IS_SUCCESS(p->peer) ) continue;

    if( p->peer->CancelPiece(idx) < 0 ){
      if(arg_verbose) CONSOLE.Debug("close: CancelPiece");
      p->peer->CloseConnection();
    }
  }
}

void PeerList::Tell_World_I_Have(size_t idx)
{
  PEERNODE *p;
  int f_seed = 0;

  if ( BTCONTENT.Seeding() ) f_seed = 1;

  for( p = m_head; p; p = p->next ){
    if( !PEER_IS_SUCCESS(p->peer) ) continue;

    // Don't send HAVE to seeders, except for our first piece.
    if( (!p->peer->bitfield.IsFull() || 1==BTCONTENT.pBF->Count()) &&
        p->peer->stream.Send_Have(idx) < 0) 
      p->peer->CloseConnection();

    else if( f_seed ){
      // request queue is emptied by setting not-interested state
      if( p->peer->SetLocal(M_NOT_INTERESTED) < 0 ){
        if(arg_verbose)
          CONSOLE.Debug("close: Can't set self not interested (T_W_I_H)");
        p->peer->CloseConnection();
      }
    }
  } // end for
}

int PeerList::Accepter()
{
  SOCKET newsk;
  socklen_t addrlen;
  struct sockaddr_in addr;
  addrlen = sizeof(struct sockaddr_in);
  newsk = accept(m_listen_sock,(struct sockaddr*) &addr,&addrlen);
//  CONSOLE.Debug("incoming! %s:%hu",
//    inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

  if( INVALID_SOCKET == newsk ) return -1;

  if( AF_INET != addr.sin_family || addrlen != sizeof(struct sockaddr_in) ){
    CLOSE_SOCKET(newsk);
    return -1;
  }

  if( Tracker.IsQuitting() ){
    CLOSE_SOCKET(newsk);
    return -1;
  }

  return NewPeer(addr,newsk);
}

int PeerList::Initial_ListenPort()
{
  int r = 0;
  struct sockaddr_in lis_addr;
  memset(&lis_addr,0, sizeof(sockaddr_in));
  lis_addr.sin_family = AF_INET;
  lis_addr.sin_addr.s_addr = INADDR_ANY;
  strcpy(m_listen, "n/a");

  m_listen_sock = socket(AF_INET,SOCK_STREAM,0);

  if( INVALID_SOCKET == m_listen_sock ) return -1;

  if ( cfg_listen_ip != 0 )
    lis_addr.sin_addr.s_addr = cfg_listen_ip;

  if(cfg_listen_port){
    lis_addr.sin_port = htons(cfg_listen_port);
    if( bind(m_listen_sock, (struct sockaddr*)&lis_addr,
        sizeof(struct sockaddr_in)) == 0 ) 
      r = 1;
    else
      CONSOLE.Warning(2, "warn, couldn't bind on specified port %d:  %s",
        cfg_listen_port, strerror(errno));
  }

  if( !r && (!cfg_listen_port || cfg_listen_port > 1025) ){
    r = -1;
    if(cfg_listen_port){
      cfg_min_listen_port = cfg_listen_port -
                            (cfg_max_listen_port - cfg_min_listen_port);
      if( cfg_min_listen_port < 1025 ) cfg_min_listen_port = 1025;
      cfg_max_listen_port = cfg_listen_port;
    }
    cfg_listen_port = cfg_max_listen_port;
    for( ; r != 0; ){
      lis_addr.sin_port = htons(cfg_listen_port);
      r = bind(m_listen_sock, (struct sockaddr*)&lis_addr,
        sizeof(struct sockaddr_in));
      if(r != 0){
        cfg_listen_port--;
        if(cfg_listen_port < cfg_min_listen_port){
          CLOSE_SOCKET(m_listen_sock);
          CONSOLE.Warning(1, "error, couldn't bind port from %d to %d:  %s",
            cfg_min_listen_port, cfg_max_listen_port, strerror(errno));
          return -1;
        }
      }
    } /* end for(; r != 0;) */
  }

  if(listen(m_listen_sock,5) == -1){
    CLOSE_SOCKET(m_listen_sock);
    CONSOLE.Warning(1, "error, couldn't listen on port %d: %s",
      cfg_listen_port,strerror(errno));
    return -1;
  }

  if( setfd_nonblock(m_listen_sock) < 0){
    CLOSE_SOCKET(m_listen_sock);
    CONSOLE.Warning(1, "error, couldn't set socket to nonblock mode.");
    return -1;
  }

  snprintf(m_listen, sizeof(m_listen), "%s:%d",
    inet_ntoa(lis_addr.sin_addr), ntohs(lis_addr.sin_port));
  CONSOLE.Print("Listening on %s", m_listen);

  return 0;
}

size_t PeerList::Pieces_I_Can_Get() const
{
  BitField tmpBitField;
  return Pieces_I_Can_Get(&tmpBitField);
}

size_t PeerList::Pieces_I_Can_Get(BitField *ptmpBitField) const
{
  if( m_seeds_count > 0 || BTCONTENT.IsFull() )
    ptmpBitField->SetAll();
  else{
    PEERNODE *p;

    *ptmpBitField = *BTCONTENT.pBF;

    for( p = m_head; p && !ptmpBitField->IsFull(); p = p->next ){
      if( PEER_IS_SUCCESS(p->peer) )
        ptmpBitField->Comb(p->peer->bitfield);
    }
  }
  return ptmpBitField->Count();
}

int PeerList::AlreadyRequested(size_t idx) const
{
  PEERNODE *p;
  for( p = m_head; p; p = p->next ){
    if( !PEER_IS_SUCCESS(p->peer) || p->peer->request_q.IsEmpty()) continue;
    if( p->peer->request_q.HasIdx(idx) ) return 1;
  }
  return 0;
}

void PeerList::CheckBitField(BitField &bf)
{
  PEERNODE *p;
  PSLICE ps;
  size_t idx;
  for( p = m_head; p ; p = p->next ){
    if( !PEER_IS_SUCCESS(p->peer) || p->peer->request_q.IsEmpty()) continue;
    ps = p->peer->request_q.GetHead();
    idx = BTCONTENT.GetNPieces();
    for( ; ps; ps = ps->next ){
      if( ps->index != idx ){
        bf.UnSet(ps->index);
        idx = ps->index;
      }
    }
  }
}

void PeerList::PrintOut() const
{
  PEERNODE *p = m_head;
  struct sockaddr_in sin;
  CONSOLE.Print("PEER LIST");
  for( ; p ; p = p->next ){
        if(PEER_IS_FAILED(p->peer)) continue;
        p->peer->dump();
  }
}

void PeerList::AnyPeerReady(fd_set *rfdp, fd_set *wfdp, int *nready,
  fd_set *rfdnextp, fd_set *wfdnextp)
{
  PEERNODE *p;
  btPeer *peer;
  SOCKET sk;
  int need_check_send = 0;

  if( FD_ISSET(m_listen_sock, rfdp) ){
    (*nready)--;
    if( !Self.OntimeDL() && !Self.OntimeUL() ){
      FD_CLR(m_listen_sock,rfdnextp);
      Accepter();
    }
  }

  for( p = m_head; p && (*nready || need_check_send) ; p = p->next ){
    if( PEER_IS_FAILED(p->peer) ) continue;

    peer = p->peer;
    sk = peer->stream.GetSocket();

    if( P_SUCCESS == peer->GetStatus() && FD_ISSET(sk,rfdp) ){
      (*nready)--;
      if( !Self.OntimeUL() ){
        FD_CLR(sk,rfdnextp);
        if( peer->RecvModule() < 0 ){
          if(arg_verbose) CONSOLE.Debug("close: receive");
          peer->CloseConnection();
        }else if( !Self.OntimeDL() && peer->HealthCheck() < 0 ){
          if(arg_verbose) CONSOLE.Debug("close: unhealthy");
          peer->CloseConnection();
        }
        if( PEER_IS_FAILED(peer) ){
          if( FD_ISSET(sk,wfdp) ) (*nready)--;
          FD_CLR(sk,wfdnextp);
        }
      }
    }
    if( P_SUCCESS == peer->GetStatus() ){
      if( FD_ISSET(sk,wfdp) ){
        (*nready)--;
        if( !Self.OntimeDL() ){
          FD_CLR(sk,wfdnextp);
          if( peer->SendModule() < 0 ){
            if(arg_verbose) CONSOLE.Debug("close: send");
            peer->CloseConnection();
            FD_CLR(sk,rfdnextp);
          }
          need_check_send = 1;
        }
      }else if( !Self.OntimeDL() )
        need_check_send = (peer->CheckSendStatus() && need_check_send);
    }
    else if( P_HANDSHAKE == peer->GetStatus() ){
      if( FD_ISSET(sk,rfdp) ){
        (*nready)--;
        if( !Self.OntimeDL() && !Self.OntimeUL() ){
          FD_CLR(sk,rfdnextp);
          if( peer->HandShake() < 0 ){
            if(arg_verbose) CONSOLE.Debug("close: bad handshake");
            peer->CloseConnection();
            FD_CLR(sk,wfdnextp);
          }
        }
      }
      if( FD_ISSET(sk,wfdp) ){
        (*nready)--;
        if( !Self.OntimeDL() && !Self.OntimeUL() ){
          FD_CLR(sk,wfdnextp);
          if( peer->SendModule() < 0 ){
            if(arg_verbose) CONSOLE.Debug("close: send handshake");
            peer->CloseConnection();
            FD_CLR(sk,rfdnextp);
          }
        }
      }
    }
    else if( P_CONNECTING == peer->GetStatus() ){
      if( FD_ISSET(sk,wfdp) ){
        (*nready)--; 
        if( !Self.OntimeDL() && !Self.OntimeUL() ){
          FD_CLR(sk,wfdnextp);
          if( peer->Send_ShakeInfo() < 0 ){
            if(arg_verbose) CONSOLE.Debug("close: Sending handshake");
            peer->CloseConnection();
            FD_CLR(sk,rfdnextp);
          }else peer->SetStatus(P_HANDSHAKE);
        }
        if( FD_ISSET(sk,rfdp) ) (*nready)--; 
      }else if( FD_ISSET(sk,rfdp) ){  // connect failed.
        (*nready)--; 
        if( !Self.OntimeDL() && !Self.OntimeUL() ){
          FD_CLR(sk,rfdnextp);
          if(arg_verbose) CONSOLE.Debug("close: connect failed");
          peer->CloseConnection();
          FD_CLR(sk,wfdnextp);
        }
      }
    }
  }// end for

  if( !m_ul_limited && !BandWidthLimitUp() ) m_missed_count++;
}

void PeerList::CloseAllConnectionToSeed()
{
  PEERNODE *p = m_head;
  for( ; p; p = p->next ){
    if( p->peer->bitfield.IsFull() ||
        /* Drop peers who remain uninterested, but keep recent connections.
           Peers who connected recently will resolve by bitfield exchange. */
        (PEER_IS_SUCCESS(p->peer) && !p->peer->Is_Remote_Interested() &&
          BTCONTENT.GetSeedTime() - now >= 300 &&
          !p->peer->ConnectedWhileSeed()) ){
      p->peer->DontWantAgain();
      if(arg_verbose) CONSOLE.Debug("close: seed<->seed");
      p->peer->CloseConnection();
    }
    else p->peer->stream.in_buffer.SetSize(BUF_DEF_SIZ);
  }
}

int PeerList::UnChokeCheck(btPeer* peer, btPeer *peer_array[])
{
  int i = 0;
  int cancel_idx = 0;
  btPeer *loster = (btPeer*) 0;
  int f_seed = BTCONTENT.Seeding();
  int no_opt = 0;
  unsigned long rndbits;
  int r=0;
  int retval = 0;

  if(m_opt_timestamp) no_opt = 1;
  if(f_seed) no_opt = 1 - no_opt;

// Find my 3 or 4 fastest peers.
// The m_max_unchoke+1 (4th) slot is for the optimistic unchoke when it happens.

  // Find a slot for the candidate--the slowest peer, or an available slot.
  for( cancel_idx = i = 0; i < m_max_unchoke + no_opt; i++ ){
    if((btPeer*) 0 == peer_array[i] ||
        PEER_IS_FAILED(peer_array[i]) ){	// сп©ун╩
      cancel_idx = i; 
      break;
    }else{
      if(cancel_idx == i) continue;

      if(f_seed){
        // compare time unchoked
        if( (!peer_array[i]->Is_Local_UnChoked() &&
            (peer_array[cancel_idx]->Is_Local_UnChoked() ||
              peer_array[cancel_idx]->GetLastUnchokeTime() <
                peer_array[i]->GetLastUnchokeTime())) ||
            (peer_array[i]->Is_Local_UnChoked() &&
             peer_array[cancel_idx]->Is_Local_UnChoked() &&
             peer_array[i]->GetLastUnchokeTime() <
               peer_array[cancel_idx]->GetLastUnchokeTime()) )
          cancel_idx = i;
      }else{
        // compare download rate.
        if( peer_array[cancel_idx]->RateDL() > peer_array[i]->RateDL()
          //if equal, reciprocate to the peer we've sent less to, proportionally
          ||(peer_array[cancel_idx]->RateDL() == peer_array[i]->RateDL()
            && peer_array[cancel_idx]->TotalUL()
                / (peer_array[cancel_idx]->TotalDL()+.001)
              < peer_array[i]->TotalUL() / (peer_array[i]->TotalDL()+.001)) )
          cancel_idx = i;
      }
    }
  } // end for

  if( (btPeer*) 0 != peer_array[cancel_idx] &&
      PEER_IS_SUCCESS(peer_array[cancel_idx]) ){
    if(f_seed){
      if( (!peer_array[cancel_idx]->Is_Local_UnChoked() &&
           (peer->Is_Local_UnChoked() ||
             peer->GetLastUnchokeTime() <
               peer_array[cancel_idx]->GetLastUnchokeTime())) ||
           (peer_array[cancel_idx]->Is_Local_UnChoked() &&
            peer->Is_Local_UnChoked() &&
            peer_array[cancel_idx]->GetLastUnchokeTime() <
              peer->GetLastUnchokeTime()) ){
        loster = peer_array[cancel_idx];
        peer_array[cancel_idx] = peer;
      }else
        loster = peer;
    }else{
      if( peer->RateDL() > peer_array[cancel_idx]->RateDL()
        // If equal, reciprocate to the peer we've sent less to, proportionally
        ||(peer_array[cancel_idx]->RateDL() == peer->RateDL()
          && peer_array[cancel_idx]->TotalUL()
                / (peer_array[cancel_idx]->TotalDL()+.001)
            > peer->TotalUL() / (peer->TotalDL()+.001)) ){
        loster = peer_array[cancel_idx];
        peer_array[cancel_idx] = peer;
      }else
        loster = peer;
    }

    // opt unchoke
    if( no_opt ){
      if( loster->SetLocal(M_CHOKE) < 0 ){
        loster->CloseConnection();
        if( peer==loster ) retval = -1;
      }
    }
    else
    // The last slot is for the optimistic unchoke.
    if( (btPeer*) 0 == peer_array[m_max_unchoke] ||
        PEER_IS_FAILED(peer_array[m_max_unchoke]) )
      peer_array[m_max_unchoke] = loster;
    else {
      if( !r-- ){
        rndbits = random();
        r = 15;
      }
      // if loser is empty and current is not, loser gets 75% chance.
      if( loster->IsEmpty() && !peer_array[m_max_unchoke]->IsEmpty()
            && (rndbits>>=2)&3 ){
        btPeer* tmp = peer_array[m_max_unchoke];
        peer_array[m_max_unchoke] = loster;
        loster = tmp;
      } else
        // This mess chooses the loser:
        // if loser is choked and current is not
        // OR if both are choked and loser has waited longer
        // OR if both are unchoked and loser has had less time unchoked.
      if( (!loster->Is_Local_UnChoked() &&
            ( peer_array[m_max_unchoke]->Is_Local_UnChoked() ||
              loster->GetLastUnchokeTime() <
                peer_array[m_max_unchoke]->GetLastUnchokeTime() )) ||
          (loster->Is_Local_UnChoked() &&
            peer_array[m_max_unchoke]->Is_Local_UnChoked() &&
            peer_array[m_max_unchoke]->GetLastUnchokeTime() <
              loster->GetLastUnchokeTime()) ){
        // if current is empty and loser is not, loser gets 25% chance;
        //    else loser wins.
        // transformed to: if loser is empty or current isn't, or 25% chance,
        //    then loser wins.
        if( !peer_array[m_max_unchoke]->IsEmpty() || loster->IsEmpty()
            || !((rndbits>>=2)&3) ){
          btPeer* tmp = peer_array[m_max_unchoke];
          peer_array[m_max_unchoke] = loster;
          loster = tmp;
        }
      }
      if(loster->SetLocal(M_CHOKE) < 0){
        loster->CloseConnection();
        if( peer==loster ) retval = -1;
      }
    }
  }else //else if((btPeer*) 0 != peer_array[cancel_idx].....
    peer_array[cancel_idx] = peer;

  return retval;
}

// When we change what we're going after, we need to evaluate & set our
// interest with each peer appropriately.
void PeerList::CheckInterest()
{
  PEERNODE *p = m_head;
  for( ; p; p = p->next ){
    // Don't shortcut by checking Is_Local_Interested(), as we need to let
    // SetLocal() reset the m_standby flag.
    if( p->peer->Need_Remote_Data() ){
      if( p->peer->SetLocal(M_INTERESTED) < 0 )
        p->peer->CloseConnection();
    }else{
      if( p->peer->SetLocal(M_NOT_INTERESTED) < 0 )
        p->peer->CloseConnection();
    }
  }
}

btPeer* PeerList::GetNextPeer(btPeer *peer) const
{
  static PEERNODE *p = m_head;

  if( !peer ) p = m_head;
  else if( p && p->peer == peer ){
    p = p->next;
  }else{
    for( p=m_head; p && (p->peer != peer); p = p->next );
    if(p) p = p->next;
    else p = m_head;
  }
  for( ; p; p = p->next )
    if( p->peer && PEER_IS_SUCCESS(p->peer) ) break;

  if(p) return p->peer;
  else return (btPeer*) 0;
}

int PeerList::Endgame() const
{
  BitField tmpBitfield;

  tmpBitfield = *BTCONTENT.pBF;
  tmpBitfield.Invert();                       // what I don't have...
  tmpBitfield.Except(BTCONTENT.GetFilter());  // ...that I want
  if( tmpBitfield.Count() < m_peers_count ) return 1;

  Pieces_I_Can_Get(&tmpBitfield);             // what's available...
  tmpBitfield.Except(BTCONTENT.GetFilter());  // ...that I want...
  tmpBitfield.Except(*BTCONTENT.pBF);         // ...that I don't have
  if( tmpBitfield.Count() < m_peers_count ) return 1;

  return 0;
}

void PeerList::Pause()
{
  PEERNODE *p = m_head;

  m_f_pause = 1;
  StopDownload();
  for( ; p; p = p->next ){
    if( p->peer->Is_Local_UnChoked() && p->peer->SetLocal(M_CHOKE) < 0 )
      p->peer->CloseConnection();
  }
}

void PeerList::Resume()
{
  m_f_pause = 0;
  CheckInterest();
}

void PeerList::StopDownload()
{
  PEERNODE *p = m_head;

  for( ; p; p = p->next ){
    if( p->peer->Is_Local_Interested() ){
      if(p->peer->PutPending() < 0 || p->peer->SetLocal(M_NOT_INTERESTED) < 0)
        p->peer->CloseConnection();
    }
  }
}

size_t PeerList::GetUnchoked() const
{
  PEERNODE *p;
  size_t count = 0;

  for( p = m_head; p; p = p->next ){
    if( PEER_IS_SUCCESS(p->peer) && p->peer->Is_Local_UnChoked() ){
      count++;
      if( count > m_max_unchoke ) break;
    }
  }
  return count;
}

// This function returns 0 if it could not find an upload faster than the
// minimum and all peer upload rates are known (not zero).
size_t PeerList::GetSlowestUp(size_t minimum) const
{
  PEERNODE *p;
  size_t slowest = 0, zero = 0, unchoked = 0, rate;

  for( p = m_head; p; p = p->next ){
    if( PEER_IS_SUCCESS(p->peer) && p->peer->Is_Local_UnChoked() ){
      unchoked++;
      rate = p->peer->RateUL();
      if( 0==rate ) zero = 1;
      else if( rate >= minimum && (rate < slowest || 0==slowest) )
        slowest = rate;
    }
  }
  if( slowest > (rate = Self.RateUL()) ) slowest = rate;

  // We're looking for slow, so guess low when we must guess a rate.
  if( slowest ){
    if( zero ) return minimum ? minimum : ((slowest+1)/2);
    else return slowest;
  }else{
    if( 0==unchoked ) unchoked = 1;  // safeguard
    if( zero ) return minimum ? minimum :
                             ((rate = Self.RateUL()) ? rate / unchoked : 1);
    else return 0;
  }
}

size_t PeerList::GetDownloads() const
{
  PEERNODE *p;
  size_t count = 0;

  for( p = m_head; p; p = p->next ){
    if( PEER_IS_SUCCESS(p->peer) && p->peer->Is_Remote_UnChoked() ) count++;
  }
  return count;
}

int PeerList::BandWidthLimitUp(double when)
{
  int limited = 0;
  double nexttime;

  if( cfg_max_bandwidth_up <= 0 ) return 0;

  nexttime = Self.LastSendTime() +
             (double)(Self.LastSizeSent()) / cfg_max_bandwidth_up;
  if( nexttime >= now + 1 + when ) limited = 1;
  else if( nexttime < now + when ) limited = 0;
  else{
    struct timespec nowspec;
    double rightnow;

    clock_gettime(CLOCK_REALTIME, &nowspec);
    rightnow = nowspec.tv_sec + (double)(nowspec.tv_nsec)/1000000000;

    if( nexttime <= rightnow + when ) limited = 0;
    else limited = 1;
  }
  if( limited ) m_f_ulate = 1;
  return limited;
}

int PeerList::BandWidthLimitDown(double when)
{
  int limited = 0;
  double nexttime;

  // Don't check SeedOnly() here--need to let the input stream drain.
  if( cfg_max_bandwidth_down <= 0 ) return 0;

  nexttime = Self.LastRecvTime() +
             (double)(Self.LastSizeRecv()) / cfg_max_bandwidth_down;
  if( nexttime >= now + 1 + when ) limited = 1;
  else if( nexttime < now + when ) limited = 0;
  else{
    struct timespec nowspec;
    double rightnow;

    clock_gettime(CLOCK_REALTIME, &nowspec);
    rightnow = nowspec.tv_sec + (double)(nowspec.tv_nsec)/1000000000;

    if( nexttime <= rightnow + when ) limited = 0;
    else limited = 1;
  }
  if( limited ) m_f_dlate = 1;
  return limited;
}

int PeerList::IsIdle()
{
  int idle = 0, dlate=0, ulate=0;

  if(
    ( 0==cfg_max_bandwidth_down ||
      (dlate = (now > (time_t)(Self.LastRecvTime() + Self.LateDL() +
                     Self.LastSizeRecv() / (double)cfg_max_bandwidth_down))) ||
      BandWidthLimitDown(Self.LateDL()) )

    && !(dlate && m_f_dlate) &&

    ( 0==cfg_max_bandwidth_up ||
      (ulate = (now > (time_t)(Self.LastSendTime() + Self.LateUL() +
                     Self.LastSizeSent() / (double)cfg_max_bandwidth_up))) ||
      BandWidthLimitUp(Self.LateUL()) )
  ){
    idle = 1;
  }

  if( !dlate ) m_f_dlate = 1;
  else if( m_f_dlate ) idle = 0;
  if( !ulate ) m_f_ulate = 1;
  else if( m_f_ulate ) idle = 0;
  return idle;
}

// How long must we wait for bandwidth to become available in either direction?
double PeerList::WaitBW() const
{
  struct timespec nowspec;
  double rightnow, late;
  double maxwait = 0, nextwake = 0;
  double nextup = 0, nextdn = 0;
  int use_up = 0, use_dn = 0;

  if( cfg_max_bandwidth_up ){
    nextup = Self.LastSendTime() +
             (double)(Self.LastSizeSent()) / cfg_max_bandwidth_up;
  }
  if( cfg_max_bandwidth_down ){
    nextdn = Self.LastRecvTime() +
             (double)(Self.LastSizeRecv()) / cfg_max_bandwidth_down;
  }

  // could optimize away the clock call when maxwait will be > MAX_SLEEP
  if( now <= (time_t)nextup || now <= (time_t)nextdn ){
    clock_gettime(CLOCK_REALTIME, &nowspec);
    rightnow = nowspec.tv_sec + (double)(nowspec.tv_nsec)/1000000000;
  }else rightnow = (double)now;

  if( nextup >= rightnow ){
    if( nextdn < rightnow ) use_up = 1;
    else if( nextdn < nextup ) use_dn = 1;
    else use_up = 1;
  }else if( nextdn >= rightnow ) use_dn = 1;

  if( use_up ){
    nextwake = nextup;
    late = Self.LateUL();
  }else if( use_dn ){
    nextwake = nextdn;
    late = Self.LateDL();
  }else{
    nextwake = late = 0;
  }

  if( (m_f_limitd && nextdn <= rightnow + Self.LateDL()) ||
      (m_f_limitu && nextup <= rightnow + Self.LateUL()) ){
    // socket setup is outdated; send a problem indicator value back
    Self.OntimeUL(0);
    Self.OntimeDL(0);
    maxwait = -100;
  }else if( nextwake > rightnow ){
    maxwait = nextwake - rightnow - late;
    if( maxwait < 0 ){
      use_up = use_dn = 0;
    }
    Self.OntimeUL(use_up);
    Self.OntimeDL(use_dn);
//  CONSOLE.Debug("waitbw %f at %f", maxwait, rightnow);
  }else{
    Self.OntimeUL(0);
    Self.OntimeDL(0);
//  CONSOLE.Debug("nextwake %f at %f", nextwake, rightnow);
  }
  return maxwait;
}

void PeerList::UnchokeIfFree(btPeer *peer)
{
  PEERNODE *p;
  size_t count = 0;

  if( m_f_pause ) return;
  for( p = m_head; p; p = p->next ){
    if( PEER_IS_SUCCESS(p->peer) && p->peer->Is_Local_UnChoked() &&
        p->peer->Is_Remote_Interested() ){
      count++;
      if( m_max_unchoke < count ) return;
    }
  }
  if( peer->SetLocal(M_UNCHOKE) < 0 ) peer->CloseConnection();
}

void PeerList::AdjustPeersCount()
{
  Tracker.AdjustPeersCount();
}

