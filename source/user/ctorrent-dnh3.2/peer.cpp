#include "peer.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/time.h>
#include <errno.h>

#include "btstream.h"
#include "./btcontent.h"
#include "./msgencode.h"
#include "./peerlist.h"
#include "./btconfig.h"
#include "bttime.h"
#include "console.h"

#if !defined(HAVE_CLOCK_GETTIME) || !defined(HAVE_SNPRINTF)
#include "compat.h"
#endif

// Convert a peer ID to a printable string.
int TextPeerID(const unsigned char *peerid, char *txtid)
{
  int i, j;

  for(i=j=0; i < PEER_ID_LEN; i++){
    if( i==j && isprint(peerid[i]) && !isspace(peerid[i]) )
      txtid[j++] = peerid[i];
    else{
      if(i==j){ sprintf(txtid+j, "0x"); j+=2; }
      snprintf(txtid+j, 3, "%.2X", (int)(peerid[i]));
      j += 2;
    }
  }
  txtid[j] = '\0';

  return 0;
}


/* g_next_up is used to rotate uploading.  If we have the opportunity to
   upload to a peer but skip it due to bw limiting, the var is set to point to
   that peer and it will be given priority at the next opportunity.
   g_next_dn is similar, but for downloading.
   g_defer_up is used to let the g_next peer object know if it skipped, as the
   socket could go non-ready if other messages are sent while waiting for bw.
*/
btPeer *btPeer::g_next_up = (btPeer *)0;
btPeer *btPeer::g_next_dn = (btPeer *)0;
unsigned char btPeer::g_defer_up = 0;

btBasic Self;

void btBasic::SetIp(struct sockaddr_in addr)
{
  memcpy(&m_sin.sin_addr,&addr.sin_addr,sizeof(struct in_addr));
}

void btBasic::SetAddress(struct sockaddr_in addr)
{
  memcpy(&m_sin,&addr,sizeof(struct sockaddr_in));
}

int btBasic::IpEquiv(struct sockaddr_in addr)
{
//  CONSOLE.Debug_n("IpEquiv: %s <=> ", inet_ntoa(m_sin.sin_addr));
//  CONSOLE.Debug_n("%s", inet_ntoa(addr.sin_addr));
//  CONSOLE.Debug_n("");
  return (memcmp(&m_sin.sin_addr,&addr.sin_addr,sizeof(struct in_addr)) == 0) ? 
    1 : 0;
}

int btPeer::Need_Local_Data() const
{
  if( m_state.remote_interested && !bitfield.IsFull()){

    if( BTCONTENT.IsFull() ) return 1; // i am seed

    BitField tmpBitfield = *BTCONTENT.pBF;
    tmpBitfield.Except(bitfield);
    return tmpBitfield.IsEmpty() ? 0 : 1;

  }
  return 0;
}

int btPeer::Need_Remote_Data() const
{

  if( BTCONTENT.Seeding() ) return 0;
  else if( bitfield.IsFull() &&
    BTCONTENT.CheckedPieces() >= BTCONTENT.GetNPieces() ) return 1;
  else{
    BitField tmpBitfield = bitfield;                // what peer has
    tmpBitfield.Except(*BTCONTENT.pBF);             // what I have
    tmpBitfield.Except(*BTCONTENT.pBMasterFilter);  // what I don't want
    tmpBitfield.And(*BTCONTENT.pBChecked);          // what I've checked
    return tmpBitfield.IsEmpty() ? 0 : 1;
  }
  return 0;
}

btPeer::btPeer()
{
  m_f_keepalive = 0;
  m_status = P_CONNECTING;
  m_unchoke_timestamp = (time_t)0;
  m_last_timestamp = m_next_send_time = now;
  m_state.remote_choked = m_state.local_choked = 1;
  m_state.remote_interested = m_state.local_interested = 0;

  m_err_count = 0;
  m_cached_idx = BTCONTENT.GetNPieces();
  m_standby = 0;
  m_req_send = 5;
  m_req_out = 0;
  m_latency = 0;
  m_prev_dlrate = 0;
  m_health_time = m_receive_time = m_choketime = m_last_timestamp;
  m_cancel_time = m_latency_timestamp = (time_t)0;
  m_bad_health = 0;
  m_want_again = m_connect = m_retried = 0;
  m_connect_seed = 0;
  m_prefetch_time = (time_t)0;
  m_requested = 0;
  m_prefetch_completion = 0;

  rate_dl.SetSelf(Self.DLRatePtr());
  rate_ul.SetSelf(Self.ULRatePtr());
}

void btPeer::CopyStats(btPeer *peer)
{
  SetDLRate(peer->GetDLRate());
  SetULRate(peer->GetULRate());
  m_unchoke_timestamp = peer->GetLastUnchokeTime();
  m_retried = peer->Retried(); // don't try to reconnect over & over
}

int btPeer::SetLocal(unsigned char s)
{
  switch(s){
  case M_CHOKE:
    if( m_state.local_choked ) return 0;
    m_unchoke_timestamp = now;
//  if(arg_verbose) CONSOLE.Debug("Choking %p", this);
    if(arg_verbose) CONSOLE.Debug("Choking %p (D=%lluMB@%dK/s)", this,
      (unsigned long long)TotalDL() >> 20, (int)(RateDL() >> 10));
    m_state.local_choked = 1; 
    if( g_next_up == this ) g_next_up = (btPeer *)0;
    if( !reponse_q.IsEmpty()) reponse_q.Empty();
    StopULTimer();
    if( !m_requested && BTCONTENT.IsFull() ){
      // hasn't sent a request since unchoke
      if(arg_verbose) CONSOLE.Debug("%p inactive", this);
      return -1;
    }
    m_requested = 0;
    break;
  case M_UNCHOKE: 
    if( !reponse_q.IsEmpty() ) StartULTimer();
    if( !m_state.local_choked ) return 0;
    m_unchoke_timestamp = now;
//  if(arg_verbose) CONSOLE.Debug("Unchoking %p", this);
    if(arg_verbose) CONSOLE.Debug("Unchoking %p (D=%lluMB@%dK/s)", this,
      (unsigned long long)TotalDL() >> 20, (int)(RateDL() >> 10));
    m_state.local_choked = 0;
    // No data is queued, so rate cannot delay sending.
    m_next_send_time = now;
    break;
  case M_INTERESTED: 
    if( BTCONTENT.Seeding() ) return 0;
    m_standby = 0;
    if( m_state.local_interested ) return 0;
    if(arg_verbose) CONSOLE.Debug("Interested in %p", this);
    m_state.local_interested = 1;
    break;
  case M_NOT_INTERESTED:
    if( !m_state.local_interested ) return 0;
    if(arg_verbose) CONSOLE.Debug("Not interested in %p", this);
    m_state.local_interested = 0; 
    if( !request_q.IsEmpty() ){
      if( CancelRequest(request_q.GetHead()) < 0 ) return -1;
      request_q.Empty();
    }
    break;
  default:
    return -1;			// BUG ???
  }
  return stream.Send_State(s);
}

int btPeer::RequestPiece()
{
  size_t idx;
  BitField tmpBitfield, *pfilter;
  int endgame = 0;

  size_t qsize = request_q.Qsize();
  size_t psize = BTCONTENT.GetPieceLength() / cfg_req_slice_size;

  // See if there's room in the queue for a new piece.
  // Also, don't queue another piece if we still have a full piece queued.
  if( cfg_req_queue_length - qsize < psize || qsize >= psize ){
    m_req_send = m_req_out;   // don't come back until you receive something.
    return 0;
  }

  tmpBitfield = bitfield;
  tmpBitfield.Except(*BTCONTENT.pBMasterFilter);
  if( PENDINGQUEUE.ReAssign(&request_q, tmpBitfield) ){
    if(arg_verbose) CONSOLE.Debug("Assigning to %p from Pending", this);
    return SendRequest();
  }

  if( m_cached_idx < BTCONTENT.CheckedPieces() && !BTCONTENT.pBF->IsEmpty() ){
    // A HAVE msg already selected what we want from this peer
    // but ignore it in initial-piece mode.
    idx = m_cached_idx;
    m_cached_idx = BTCONTENT.GetNPieces();
    if( !BTCONTENT.pBF->IsSet(idx) &&
        (!BTCONTENT.GetFilter() || !BTCONTENT.GetFilter()->IsSet(idx)) &&
        !PENDINGQUEUE.Exist(idx) &&
        !WORLD.AlreadyRequested(idx) ){
      if(arg_verbose) CONSOLE.Debug("Assigning #%d to %p", (int)idx, this);
      return (request_q.CreateWithIdx(idx) < 0) ? -1 : SendRequest();
    }
  }

  // If we didn't want the cached piece, select another.
  if( BTCONTENT.pBF->IsEmpty() ){
    // If we don't have a complete piece yet, try to get one that's already
    // in progress.  (Initial-piece mode)
    pfilter = BTCONTENT.GetFilter();
    do{
      tmpBitfield = bitfield;
      if( pfilter ){
        tmpBitfield.Except(*pfilter);
        pfilter = BTCONTENT.GetNextFilter(pfilter);
      }
    }while( pfilter && tmpBitfield.IsEmpty() );
    idx = WORLD.What_Can_Duplicate(tmpBitfield, this, BTCONTENT.GetNPieces());
    if( idx < BTCONTENT.GetNPieces() ){
      if(arg_verbose) CONSOLE.Debug("Want to dup #%d to %p", (int)idx, this);
      btPeer *peer = WORLD.WhoHas(idx);
      if(peer){
        if(arg_verbose) CONSOLE.Debug("Duping: %p to %p (#%d)",
          peer, this, (int)idx);
        if(request_q.CopyShuffle(&peer->request_q, idx) < 0) return -1;
        else return SendRequest();
      }
    }else if(arg_verbose) CONSOLE.Debug("Nothing to dup to %p", this);
  }

  // Doesn't have a piece that's already in progress--choose another.
  pfilter = BTCONTENT.GetFilter();
  do{
    tmpBitfield = bitfield;
    tmpBitfield.Except(*BTCONTENT.pBF);
    if( pfilter ){
      tmpBitfield.Except(*pfilter);
      pfilter = BTCONTENT.GetNextFilter(pfilter);
    }
    // Don't go after pieces we might already have (but don't know yet)
    tmpBitfield.And(*BTCONTENT.pBChecked);
    // tmpBitfield tells what we need from this peer...
  }while( pfilter && tmpBitfield.IsEmpty() );

  if( tmpBitfield.IsEmpty() ){
    // We don't need anything from the peer.  How'd we get here?
    return SetLocal(M_NOT_INTERESTED);
  }

  BitField tmpBitfield2 = tmpBitfield;
  WORLD.CheckBitField(tmpBitfield2);
  // [tmpBitfield2] ...that we haven't requested from anyone.
  if( tmpBitfield2.IsEmpty() ){
    // Everything this peer has that I want, I've already requested.
    if( WORLD.Endgame() ){  // OK to duplicate a request.
//    idx = tmpBitfield.Random();
      idx = 0;  // flag for Who_Can_Duplicate()
      BitField tmpBitfield3 = tmpBitfield2;
      idx = WORLD.What_Can_Duplicate(tmpBitfield3, this, idx);
      if( idx < BTCONTENT.GetNPieces() ){
        if(arg_verbose) CONSOLE.Debug("Want to dup #%d to %p",
          (int)idx, this);
        btPeer *peer = WORLD.WhoHas(idx);
        if(peer){
          if(arg_verbose) CONSOLE.Debug("Duping: %p to %p (#%d)",
            peer, this, (int)idx);
          if(request_q.CopyShuffle(&peer->request_q, idx) < 0) return -1;
          else return SendRequest();
        }
      }else if(arg_verbose) CONSOLE.Debug("Nothing to dup to %p", this);
    }else{  // not endgame mode
      btPeer *peer = WORLD.Who_Can_Abandon(this); // slowest choice
      if(peer){
        // Cancel a request to the slowest peer & request it from this one.
        if(arg_verbose) CONSOLE.Debug("Reassigning %p to %p (#%d)",
          peer, this, (int)(peer->request_q.GetRequestIdx()));
        // RequestQueue class "moves" rather than "copies" in assignment!
        if( request_q.Copy(&peer->request_q) < 0 ) return -1;
        if(peer->CancelPiece() < 0 || peer->RequestCheck() < 0)
          peer->CloseConnection();
        return SendRequest();
      }else if( BTCONTENT.CheckedPieces() >= BTCONTENT.GetNPieces() ){
        if(arg_verbose) CONSOLE.Debug("%p standby", this);
        m_standby = 1;  // nothing to do at the moment
      }
    }
  }else{
    // Request something that we haven't requested yet (most common case).
    // Try to make it something that has good trade value.
    BitField tmpBitfield3 = tmpBitfield2;
    WORLD.FindValuedPieces(tmpBitfield3, this, BTCONTENT.pBF->IsEmpty());
    if( tmpBitfield3.IsEmpty() ) tmpBitfield3 = tmpBitfield2;
    idx = tmpBitfield3.Random();
    if(arg_verbose) CONSOLE.Debug("Assigning #%d to %p", (int)idx, this);
    return (request_q.CreateWithIdx(idx) < 0) ? -1 : SendRequest();
  }
  return 0;
}

int btPeer::MsgDeliver()
{
  size_t r,idx,off,len;
  int retval = 0;

  char *msgbuf = stream.in_buffer.BasePointer();

  r = get_nl(msgbuf);

  // Don't require keepalives if we're receiving other messages.
  m_last_timestamp = now;
  if( 0 == r ){
    if( !m_f_keepalive ) if( stream.Send_Keepalive() < 0 ) return -1;
    m_f_keepalive = 0;
    return 0;
  }else{
    char msg = msgbuf[H_LEN];
    switch(msg){
    case M_CHOKE:
      if(H_BASE_LEN != r) return -1;
      if(arg_verbose) CONSOLE.Debug("%p choked me", this);
      if( m_lastmsg == M_UNCHOKE && m_last_timestamp <= m_choketime+1 ){
        m_err_count+=2;
        if(arg_verbose) CONSOLE.Debug("err: %p (%d) Choke oscillation",
          this, m_err_count);
      }
      m_choketime = m_last_timestamp;
      m_state.remote_choked = 1;
      StopDLTimer();
      if( g_next_dn == this ) g_next_dn = (btPeer *)0;
      if( !request_q.IsEmpty() ){
        m_req_out = 0;
        PENDINGQUEUE.Pending(&request_q);
      }
      break;

    case M_UNCHOKE:
      if(H_BASE_LEN != r) return -1;
      if(arg_verbose) CONSOLE.Debug("%p unchoked me", this);
      if( m_lastmsg == M_CHOKE && m_last_timestamp <= m_choketime+1 ){
        m_err_count+=2;
        if(arg_verbose) CONSOLE.Debug("err: %p (%d) Choke oscillation",
          this, m_err_count);
      }
      m_choketime = m_last_timestamp;
      m_state.remote_choked = 0;
      m_prefetch_completion = 0;
      retval = RequestCheck();
      break;

    case M_INTERESTED:
      if(H_BASE_LEN != r) return -1;
      if(arg_verbose) CONSOLE.Debug("%p is interested", this);
      m_state.remote_interested = 1;
      if( Need_Local_Data() ) WORLD.UnchokeIfFree(this);
      break;

    case M_NOT_INTERESTED:
      if(H_BASE_LEN != r) return -1;
      if(arg_verbose) CONSOLE.Debug("%p is not interested", this);

      m_state.remote_interested = 0;

      /* remove peer's reponse queue */
      if( !reponse_q.IsEmpty()) reponse_q.Empty();

      /* if I've been seed for a while, nobody should be uninterested */
      if( BTCONTENT.IsFull() && BTCONTENT.GetSeedTime() - now >= 300 )
         return -2;
      break;

    case M_HAVE:
      if(H_HAVE_LEN != r) return -1;

      idx = get_nl(msgbuf + H_LEN + H_BASE_LEN);

      if( idx >= BTCONTENT.GetNPieces() || bitfield.IsSet(idx) ) return -1;

      bitfield.Set(idx);

      if( bitfield.IsFull() ){
        if( BTCONTENT.IsFull() ) return -2;
        else stream.out_buffer.SetSize(BUF_DEF_SIZ);
      }

      if( !BTCONTENT.pBF->IsSet(idx) && !BTCONTENT.pBMasterFilter->IsSet(idx) ){
        if( m_cached_idx >= BTCONTENT.GetNPieces() || m_standby ||
            (!BTCONTENT.GetFilter() || !BTCONTENT.GetFilter()->IsSet(idx)) )
          m_cached_idx = idx;
        if(arg_verbose && m_standby) CONSOLE.Debug("%p un-standby", this);
        m_standby = 0;
      }

      // see if we're Interested now
      if(!m_standby) retval = RequestCheck();
      break;

    case M_REQUEST:
      if(H_REQUEST_LEN != r || !m_state.remote_interested) return -1;

      idx = get_nl(msgbuf + H_LEN + H_BASE_LEN);
      
      if( !BTCONTENT.pBF->IsSet(idx) ) return -1;
      
      off = get_nl(msgbuf + H_LEN + H_BASE_LEN + H_INT_LEN);
      len = get_nl(msgbuf + H_LEN + H_BASE_LEN + H_INT_LEN * 2);

      if(arg_verbose) CONSOLE.Debug("%p is requesting %d/%d/%d",
        this, (int)idx, (int)off, (int)len);

      if( !reponse_q.IsValidRequest(idx, off, len) ) return -1;

      if( m_state.local_choked ){
        if( m_last_timestamp - m_unchoke_timestamp >
              (m_latency ? (m_latency*2) : 60) ){
          m_err_count++;
          if(arg_verbose) CONSOLE.Debug("err: %p (%d) choked request",
            this, m_err_count);
          if( stream.Send_State(M_CHOKE) < 0 ) return -1;
          // This will mess with the unchoke rotation (to this peer's
          // disadvantage), but otherwise we may spam them with choke msgs.
          m_unchoke_timestamp = m_last_timestamp;
        }
      }else{
        if( !m_requested ){
          m_requested = 1;
          if( stream.out_buffer.SetSize(BUF_DEF_SIZ +
              (len < DEFAULT_SLICE_SIZE) ? DEFAULT_SLICE_SIZE : len) < 0 )
            return -1;
          if( (!m_receive_time || BTCONTENT.Seeding()) &&
              now > m_unchoke_timestamp ){
            m_latency = (now <= m_unchoke_timestamp) ? 1 :
              (now - m_unchoke_timestamp);
            if(arg_verbose) CONSOLE.Debug("%p latency is %d sec (request)",
              this, (int)m_latency);
          }
        }
        retval = reponse_q.Add(idx, off, len);
      }
      break;

    case M_PIECE:
      if( H_PIECE_LEN >= r ) return -1;
      m_receive_time = m_last_timestamp;
      // PieceDeliver handles the error determination & DL counting
      retval = PieceDeliver(r);
      break;

    case M_BITFIELD:
      if( (r - H_BASE_LEN) != bitfield.NBytes() || !bitfield.IsEmpty() )
        return -1;
      bitfield.SetReferBuffer(msgbuf + H_LEN + H_BASE_LEN);
      if(bitfield.IsFull()){
        if(arg_verbose) CONSOLE.Debug("%p is a seed", this);
        if(BTCONTENT.IsFull()) return -2;
        else{
          stream.out_buffer.SetSize(BUF_DEF_SIZ);
          if( !m_want_again ) m_want_again = 1;
        }
      }

      // This is needed in order to set our Interested state.
      retval = RequestCheck(); // fixed client stall
      break;

    case M_CANCEL:
      if(H_CANCEL_LEN != r) return -1;

      idx = get_nl(msgbuf + H_LEN + H_BASE_LEN);
      off = get_nl(msgbuf + H_LEN + H_BASE_LEN + H_INT_LEN);
      len = get_nl(msgbuf + H_LEN + H_BASE_LEN + H_INT_LEN * 2);
      if( reponse_q.Remove(idx,off,len) < 0 ){
        if( m_state.local_choked &&
            m_last_timestamp - m_unchoke_timestamp >
              (m_latency ? (m_latency*2) : 60) ){
          m_err_count++;
          if(arg_verbose) CONSOLE.Debug("err: %p (%d) Bad cancel",
            this, m_err_count);
        }
      }else if( reponse_q.IsEmpty() && g_next_up == this )
        g_next_up = (btPeer *)0;
      break;

    default:
      if(arg_verbose)
        CONSOLE.Debug("Unknown message type %d from peer %p", (int)msg, this);
    } // switch

    if( retval >= 0 ) m_lastmsg = msg;
  }
  return retval;
}

int btPeer::ReponseSlice()
{
  size_t len = 0;
  struct timespec nowspec;

  ssize_t retval;
  size_t idx,off;
  reponse_q.Pop(&idx,&off,&len);

  retval = BTCONTENT.ReadSlice(BTCONTENT.global_piece_buffer,idx,off,len);
  if( retval < 0 ) return -1;
  else if( retval && cfg_cache_size ) Self.OntimeUL(0);  // disk read delay
  // If not using cache, need to always allow time for a disk read.

  size_t currentrate = CurrentUL();
  if(arg_verbose) CONSOLE.Debug("Sending %d/%d/%d to %p",
    (int)idx, (int)off, (int)len, this);
  // project the time to send another slice
  if( 0==currentrate ){  // don't know peer's rate; use best guess
    int rate = (int)(Self.RateUL());
    int unchoked = (int)(WORLD.GetUnchoked());
    if( unchoked < 1 ) unchoked = 1;
    if( 0==cfg_max_bandwidth_up ){
      if( 0==rate ) m_next_send_time = now;
      else m_next_send_time = now + len / (rate / unchoked);
    }else{
      m_next_send_time = now + len /
        ( ((int)cfg_max_bandwidth_up - rate >
           (int)cfg_max_bandwidth_up / unchoked) ?
        (cfg_max_bandwidth_up - rate) :
        ((cfg_max_bandwidth_up + unchoked-1) / unchoked) );
    }
  }else m_next_send_time = now + len /
    ( (currentrate < cfg_max_bandwidth_up || 0==cfg_max_bandwidth_up) ?
        currentrate : cfg_max_bandwidth_up );

  m_prefetch_time = (time_t)0;

  clock_gettime(CLOCK_REALTIME, &nowspec);
  retval = stream.Send_Piece(idx,off,BTCONTENT.global_piece_buffer,len);
  if( retval >= 0 ){
    WORLD.Upload();
    DataSended(len, nowspec.tv_sec + (double)(nowspec.tv_nsec)/1000000000);
    if( !m_want_again && BTCONTENT.Seeding() )
      m_want_again = 1;
  }
  else if(arg_verbose) CONSOLE.Debug("%p: %s", this, strerror(errno));

  return (int)retval;
}

int btPeer::SendRequest()
{
  int first = 1;
  PSLICE ps = request_q.NextSend();

  if( m_req_out > cfg_req_queue_length ){
    if(arg_verbose)
      CONSOLE.Debug("ERROR@5: %p m_req_out underflow, resetting", this);
    m_req_out = 0;
  }
  if( ps && m_req_out < m_req_send ){
    if(arg_verbose){
      CONSOLE.Debug_n("");
      CONSOLE.Debug_n("Requesting #%d from %p (%d left, %d slots):",
        (int)(ps->index), this, (int)(request_q.Qsize()), (int)m_req_send);
    }
    for( int i=0; ps && m_req_out < m_req_send && i<5; ps = ps->next, i++ ){
      if( first && (!RateDL() ||
          0 >= (m_req_out+1) * ps->length / (double)RateDL() - m_latency) ){
        request_q.SetReqTime(ps, now);
        first = 0;
      } else request_q.SetReqTime(ps, (time_t)0);
      if(arg_verbose) CONSOLE.Debug_n(".");
      if(stream.Send_Request(ps->index,ps->offset,ps->length) < 0){ return -1; }
      request_q.SetNextSend(ps->next);
      m_req_out++;
    }
    if(arg_verbose) CONSOLE.Debug_n("");
    m_receive_time = now;
  }
  return ( m_req_out < m_req_send ) ? RequestPiece() : 0;
}

int btPeer::CancelPiece()
{
  return CancelPiece(request_q.GetHead()->index);
}

int btPeer::CancelPiece(size_t idx)
{
  PSLICE ps = request_q.GetHead();
  PSLICE next;
  int cancel = 1;
  int retval;

  for( ; ps && ps->index != idx; ps=ps->next );  // find the piece

  for( ; ps; ps = next ){
    if( ps->index != idx ) break;
    if( ps == request_q.NextSend() ) cancel = 0;
    if( cancel ){
      if(arg_verbose) CONSOLE.Debug("Cancelling %d/%d/%d to %p",
        (int)(ps->index), (int)(ps->offset), (int)(ps->length), this);
      if(stream.Send_Cancel(ps->index,ps->offset,ps->length) < 0)
        return -1;
      m_req_out--;
      if( m_req_out > cfg_req_queue_length ){
        if(arg_verbose)
          CONSOLE.Debug("ERROR@1: %p m_req_out underflow, resetting", this);
        m_req_out = 0;
      }
      m_cancel_time = now;
    }
    next = ps->next;
    request_q.Remove(ps->index, ps->offset, ps->length);
  }
  if( !m_req_out && g_next_dn == this ) g_next_dn = (btPeer *)0;

  return 0;
}

int btPeer::CancelRequest(PSLICE ps)
{
  int retval;

  for( ; ps; ps = ps->next){
    if( ps == request_q.NextSend() ) break;
    if(arg_verbose) CONSOLE.Debug("Cancelling %d/%d/%d to %p",
      (int)(ps->index), (int)(ps->offset), (int)(ps->length), this);
    if(stream.Send_Cancel(ps->index,ps->offset,ps->length) < 0)
      return -1;
    m_req_out--;
    if( m_req_out > cfg_req_queue_length ){
      if(arg_verbose)
        CONSOLE.Debug("ERROR@2: %p m_req_out underflow, resetting", this);
      m_req_out = 0;
    }
    m_cancel_time = now;
  }
  if( !m_req_out && g_next_dn == this ) g_next_dn = (btPeer *)0;

  return 0;
}

int btPeer::CancelSliceRequest(size_t idx, size_t off, size_t len)
{
  PSLICE ps;
  int cancel = 1;
  int idxfound = 0;
  int retval;

  for(ps = request_q.GetHead(); ps; ps = ps->next){
    if( ps == request_q.NextSend() ) cancel = 0;
    if( idx == ps->index ){
      if( off == ps->offset && len == ps->length ){
        request_q.Remove(idx,off,len);
        if(cancel){
          if(arg_verbose) CONSOLE.Debug("Cancelling %d/%d/%d to %p",
            (int)idx, (int)off, (int)len, this);
          if(stream.Send_Cancel(idx,off,len) < 0)
            return -1;
          m_req_out--;
          if( m_req_out > cfg_req_queue_length ){
            if(arg_verbose)
              CONSOLE.Debug("ERROR@3: %p m_req_out underflow, resetting",this);
            m_req_out = 0;
          }
          if( !m_req_out && g_next_dn == this ) g_next_dn = (btPeer *)0;
          m_cancel_time = now;

          // Don't call RequestCheck() here since that could cause the slice
          // we're cancelling to be dup'd from another peer.
        }
        break;
      }
      idxfound = 1;
    }else if( idxfound ) break;
  }
  return 0;
}

int btPeer::ReportComplete(size_t idx)
{
  int r;

  if( (r = BTCONTENT.APieceComplete(idx)) > 0 ){
    if(arg_verbose) CONSOLE.Debug("Piece #%d completed", (int)idx);
    WORLD.Tell_World_I_Have(idx);
    BTCONTENT.CheckFilter();
    if( BTCONTENT.IsFull() )
      WORLD.CloseAllConnectionToSeed();
  }else if( 0 == r ){  // hash check failed
    // Don't count an error against the peer in initial or endgame mode, since
    // some slices may have come from other peers.
    if( BTCONTENT.pBF->Count() < 2 || WORLD.Endgame() ){
      m_err_count++;
      if(arg_verbose) CONSOLE.Debug("err: %p (%d) Bad complete",
        this, m_err_count);
      ResetDLTimer(); // set peer rate=0 so we don't favor for upload
    }
  }
  // Need to re-download entire piece if check failed, so cleanup in any case.
  m_prefetch_completion = 0;
  // We don't track request duplication accurately, so clean up just in case.
  WORLD.CancelPiece(idx);
  PENDINGQUEUE.Delete(idx);
  return r;
}

int btPeer::PieceDeliver(size_t mlen)
{
  size_t idx,off,len;
  char *msgbuf = stream.in_buffer.BasePointer();
  time_t t = (time_t)0;
  int f_requested = 0, f_success = 1, f_count = 1, f_want = 1;

  idx = get_nl(msgbuf + H_LEN + H_BASE_LEN);
  off = get_nl(msgbuf + H_LEN + H_BASE_LEN + H_INT_LEN);
  len = mlen - H_PIECE_LEN;

  if( !request_q.IsEmpty() ){
    t = request_q.GetReqTime(idx,off,len);
    // Verify whether this is an outstanding request (not for error counting).
    PSLICE ps = request_q.GetHead();
    for( ; ps; ps = ps->next){
      if( ps == request_q.NextSend() ) break;
      if( idx==ps->index && off==ps->offset && len==ps->length ){
        f_requested = 1;
        break;
      }
    }
  }

  Self.StartDLTimer();

  if( f_requested ){
    if(arg_verbose) CONSOLE.Debug("Receiving piece %d/%d/%d from %p",
      (int)idx, (int)off, (int)len, this);
    if( !BTCONTENT.pBF->IsSet(idx) &&
        BTCONTENT.WriteSlice(msgbuf + H_LEN + H_PIECE_LEN,idx,off,len) < 0 ){
      CONSOLE.Warning(2, "warn, WriteSlice failed; is filesystem full?");
      f_success = 0;
      // Re-queue the request, unless WriteSlice triggered flush failure
      // (then the request is already in Pending).
      if( !BTCONTENT.FlushFailed() ){
        // This removes only the first instance; re-queued request is safe.
        request_q.Remove(idx,off,len);
        m_req_out--;
        if( RequestSlice(idx,off,len) < 0 ){
          // At least it's still queued & will go to Pending at peer close.
          if( f_count ) DataRecved(len);
          return -1;
        }
      }
    }else{  // saved or had the data
      request_q.Remove(idx,off,len);
      m_req_out--;
      // Check for & cancel requests for this slice from other peers in initial
      // and endgame modes.
      int dup = 0;
      if( BTCONTENT.pBF->Count() < 2 || WORLD.Endgame() ) dup = 1;
      if( dup ) WORLD.CancelSlice(idx, off, len);
      if( dup || BTCONTENT.FlushFailed() )
        PENDINGQUEUE.DeleteSlice(idx, off, len);
    }
  }else{  // not requested--not saved
    if( m_last_timestamp - m_cancel_time > (m_latency ? (m_latency*2) : 60) ){
      m_err_count++;
      if(arg_verbose) CONSOLE.Debug("err: %p (%d) Unrequested piece %d/%d/%d",
        this, m_err_count, (int)idx, (int)off, (int)len, this);
      ResetDLTimer(); // set peer rate=0 so we don't favor for upload
      f_count = 0;
      f_want = 0;
    }else if(arg_verbose) CONSOLE.Debug("Unneeded piece %d/%d/%d from %p",
      (int)idx, (int)off, (int)len, this);
    f_success = 0;
  }
  if( !m_want_again && f_want ) m_want_again = 1;

  // Determine how many outstanding requests we should maintain, roughly:
  // (request turnaround latency) / (time to transmit one slice)
  if(t){
    m_latency = (m_last_timestamp <= t) ? 1 : (m_last_timestamp - t);
    if(arg_verbose) CONSOLE.Debug("%p latency is %d sec (receive)",
      this, (int)m_latency);
    m_latency_timestamp = m_last_timestamp;
  }
  size_t rate;
  if( (rate = RateDL()) > len/20 && m_latency_timestamp ){
    // 20==RATE_INTERVAL from rate.cpp.  This is really just a check to see if
    // rate is measurable/usable.
    m_req_send = (size_t)( m_latency / (len / (double)rate) + 1 );
    if( m_req_send < 2 ) m_req_send = 2;

    // If latency increases, we will see this as a dlrate decrease.
    if( rate < m_prev_dlrate ) m_req_send++;
    else if( m_last_timestamp - m_latency_timestamp >= 30 &&
        m_req_out == m_req_send - 1 ){
      // Try to force latency measurement every 30 seconds.
      m_req_send--;
      m_latency_timestamp = m_last_timestamp;
    }
    m_prev_dlrate = rate;
  }else if (m_req_send < 5) m_req_send = 5;

  /* if piece download complete. */
  if( f_success && (request_q.IsEmpty() || !request_q.HasIdx(idx)) &&
      !BTCONTENT.pBF->IsSet(idx) ){
    // Above WriteSlice may have triggered flush failure.  If data was saved,
    // slice was deleted from Pending.  If piece is incomplete, it's in Pending.
    if( !(BTCONTENT.FlushFailed() && PENDINGQUEUE.Exist(idx)) &&
        !ReportComplete(idx) )
      f_count = 0;
  }

  // Don't count the slice in our DL total if it was unsolicited or bad.
  // (We don't owe the swarm any UL for such data.)
  if( f_count ) DataRecved(len);
  return (P_FAILED == m_status) ? -1 : RequestCheck();
}

// This is for re-requesting unsuccessful slices.
// Use RequestPiece for normal request queueing.
int btPeer::RequestSlice(size_t idx,size_t off,size_t len)
{
  int r;
  r = request_q.Requeue(idx,off,len);
  if( r < 0 ) return -1;
  else if( r ){
    if(stream.Send_Request(idx,off,len) < 0){ return -1; }
    m_req_out++;
    m_receive_time = now;
  }
  return 0;
}

int btPeer::RequestCheck()
{
  if( BTCONTENT.Seeding() || WORLD.IsPaused() )
    return SetLocal(M_NOT_INTERESTED);

  if( Need_Remote_Data() ){
    if(!m_state.local_interested && SetLocal(M_INTERESTED) < 0) return -1;
    if( !m_state.remote_choked ){
      if( m_req_out > cfg_req_queue_length ){
        if(arg_verbose)
          CONSOLE.Debug("ERROR@4: %p m_req_out underflow, resetting", this);
        m_req_out = 0;
      }
      if( request_q.IsEmpty() && RequestPiece() < 0 ) return -1;
      else if( m_req_out < m_req_send &&
               (m_req_out < 2 || !RateDL() ||
                1 >= (m_req_out+1) * request_q.GetRequestLen() /
                     (double)RateDL() - m_latency)
      // above formula is to try to allow delay between sending batches of reqs
        && SendRequest() < 0 ) return -1;
    }
  }else
    if(m_state.local_interested && SetLocal(M_NOT_INTERESTED) < 0) return -1;
  
  if(!request_q.IsEmpty()) StartDLTimer();
  else StopDLTimer();
  return 0;
}

void btPeer::CloseConnection()
{
  if(arg_verbose) CONSOLE.Debug("%p closed", this);
  if( P_FAILED != m_status ){
    m_status = P_FAILED;
    StopDLTimer();
    StopULTimer();
    stream.Close();
    if( !request_q.IsEmpty() )
      PENDINGQUEUE.Pending(&request_q);
  }
  if( g_next_up == this ) g_next_up = (btPeer *)0;
  if( g_next_dn == this ) g_next_dn = (btPeer *)0;
}

int btPeer::HandShake()
{
  char txtid[PEER_ID_LEN*2+3];
  ssize_t r = stream.Feed();
  if( r < 0 ){
    if(arg_verbose) CONSOLE.Debug("%p: %s", this,
      (r==-2) ? "remote closed" : strerror(errno));
    return -1;
  }
  if( (r=stream.in_buffer.Count()) < 68 ){
    // If it's not BitTorrent, don't wait around for a complete handshake.
    if( r > 20 ){  // ignore 8 reserved bytes following protocol ID
      if( memcmp(stream.in_buffer.BasePointer()+20,
          BTCONTENT.GetShakeBuffer()+20, (r<28) ? (r-20) : 8) != 0 ){
        if(arg_verbose){
          CONSOLE.Debug_n("");
          CONSOLE.Debug_n("peer %p gave 0x", this);
          for(int i=20; i<r && i<27; i++) CONSOLE.Debug_n("%2.2hx",
            (unsigned short)(unsigned char)(stream.in_buffer.BasePointer()[i]));
          CONSOLE.Debug_n(" as reserved bytes (partial)");
        }
        memcpy(stream.in_buffer.BasePointer()+20, BTCONTENT.GetShakeBuffer()+20,
          (r<28) ? (r-20) : 8);
      }
    }
    if(r && memcmp(stream.in_buffer.BasePointer(),BTCONTENT.GetShakeBuffer(),
        (r<48) ? r : 48) != 0){
      if(arg_verbose){
        CONSOLE.Debug_n("");
        CONSOLE.Debug_n("mine: 0x");
        for(int i=0; i<r && i<48; i++) CONSOLE.Debug_n("%2.2hx",
          (unsigned short)(unsigned char)(BTCONTENT.GetShakeBuffer()[i]));
        CONSOLE.Debug_n("");
        CONSOLE.Debug_n("peer: 0x");
        for(int i=0; i<r && i<48; i++) CONSOLE.Debug_n("%2.2hx",
          (unsigned short)(unsigned char)(stream.in_buffer.BasePointer()[i]));
        if( r>48 ){
          TextPeerID((unsigned char *)(stream.in_buffer.BasePointer()+48),
            txtid);
          CONSOLE.Debug("peer is %s", txtid);
        }
      }
      return -1;
    }
    return 0;
  }

  if( memcmp(stream.in_buffer.BasePointer(), BTCONTENT.GetShakeBuffer(), 68)
        == 0 ){
    if(arg_verbose) CONSOLE.Debug("peer %p is myself", this);
    WORLD.AdjustPeersCount();
    return -1;
  }

  // If the reserved bytes differ, make them the same.
  // If they mean anything important, the handshake is likely to fail anyway.
  if( memcmp(stream.in_buffer.BasePointer()+20, BTCONTENT.GetShakeBuffer()+20,
      8) != 0 ){
    if(arg_verbose){
      CONSOLE.Debug_n("");
      CONSOLE.Debug_n("peer %p gave 0x", this);
      for(int i=20; i<27; i++) CONSOLE.Debug_n("%2.2hx",
        (unsigned short)(unsigned char)(stream.in_buffer.BasePointer()[i]));
      CONSOLE.Debug_n(" as reserved bytes" );
    }
    memcpy(stream.in_buffer.BasePointer()+20, BTCONTENT.GetShakeBuffer()+20, 8);
  }
  if( memcmp(stream.in_buffer.BasePointer(),
             BTCONTENT.GetShakeBuffer(),48) != 0 ){
    if(arg_verbose){
      CONSOLE.Debug_n("");
      CONSOLE.Debug_n("mine: 0x");
      for(int i=0; i<48; i++) CONSOLE.Debug_n("%2.2hx",
        (unsigned short)(unsigned char)(BTCONTENT.GetShakeBuffer()[i]));
      CONSOLE.Debug_n("");
      CONSOLE.Debug_n("peer: 0x");
      for(int i=0; i<48; i++) CONSOLE.Debug_n("%2.2hx",
        (unsigned short)(unsigned char)(stream.in_buffer.BasePointer()[i]));
    }
    return -1;
  }

  memcpy(id, stream.in_buffer.BasePointer()+48, PEER_ID_LEN);
  if(arg_verbose){
    TextPeerID((unsigned char *)(stream.in_buffer.BasePointer()+48), txtid);
    CONSOLE.Debug("Peer %p ID: %s", this, txtid);
  }

  // ignore peer id verify
  if( !BTCONTENT.pBF->IsEmpty()){
    char *bf = new char[BTCONTENT.pBF->NBytes()];
#ifndef WINDOWS
    if(!bf) return -1;
#endif
    BTCONTENT.pBF->WriteToBuffer(bf);
    r = stream.Send_Bitfield(bf,BTCONTENT.pBF->NBytes());
    delete []bf;
  }

  if( r >= 0){
    if( stream.in_buffer.PickUp(68) < 0 ) return -1;
    m_status = P_SUCCESS;
    m_retried = 0;  // allow reconnect attempt
    // When seeding, new peer starts at the end of the line.
    if( BTCONTENT.Seeding() ){	// i am seed
      // Allow resurrected peer to resume its place in line.
      if( 0 == m_unchoke_timestamp ) m_unchoke_timestamp = now;
      m_connect_seed = 1;
    }
    if( stream.HaveMessage() ) return RecvModule();
  }
  return r;
}

int btPeer::Send_ShakeInfo()
{
  return stream.Send_Buffer((char*)BTCONTENT.GetShakeBuffer(),68);
}

int btPeer::NeedWrite(int limited)
{
  int yn = 0;
  size_t r;

  if( stream.out_buffer.Count() )
    yn = 1;                                           // data in buffer to send
  else if( P_CONNECTING == m_status )
    yn = 1;                                               // peer is connecting
  else if( WORLD.IsPaused() ) yn = 0;         // paused--no up/download allowed
  else if( !m_state.local_choked && !reponse_q.IsEmpty() && !limited )
    yn = 1;                                                //can upload a slice
  else if( !m_state.remote_choked && m_state.local_interested &&
           request_q.IsEmpty() ){
    if( m_standby && WORLD.Endgame() ){
      if(arg_verbose) CONSOLE.Debug("%p un-standby (endgame)", this);
      m_standby = 0;
    }
    if( !m_standby ) yn = 1;                         // can request a new piece
  }
  if( !yn && request_q.NextSend() && m_req_out < m_req_send &&
     (m_req_out < 2 || !(r = RateDL()) ||
      1 >= (m_req_out+1) * request_q.GetRequestLen() / (double)r -
      m_latency) )
    yn = 1;                                        // can send a queued request

  return yn;
}

int btPeer::NeedRead(int limited)
{
  int yn = 1;

  if( P_SUCCESS == m_status && stream.PeekMessage(M_PIECE) &&
      ((g_next_dn && g_next_dn != this) || limited) ){
    yn = 0;
  }

  return yn;
}

int btPeer::CouldReponseSlice()
{
  // If the entire buffer isn't big enough, go ahead and let the put resize it.
  if( !m_state.local_choked &&
      (stream.out_buffer.LeftSize() >=
                             H_LEN + H_PIECE_LEN + reponse_q.GetRequestLen() ||
      stream.out_buffer.Count() + stream.out_buffer.LeftSize() <
                             H_LEN + H_PIECE_LEN + reponse_q.GetRequestLen()) )
    return 1;
  return 0;
}

int btPeer::AreYouOK()
{
  m_f_keepalive = 1;
  return stream.Send_Keepalive();
}

int btPeer::RecvModule()
{
  ssize_t r = 0;
  
  if ( 32 <= m_err_count ){
    m_want_again = 0;
    return -1;
  }

  if( stream.PeekMessage(M_PIECE) ){
    if( !g_next_dn || g_next_dn==this ){
      int limited = WORLD.BandWidthLimitDown(Self.LateDL());
      if( !limited ){
        if( g_next_dn ) g_next_dn = (btPeer *)0;
        r = stream.Feed(&rate_dl);  // feed full amount (can download)
//      if(r>=0) CONSOLE.Debug("%p fed piece, now has %d bytes", this, r);
        Self.OntimeDL(0);
      }
      else if( !g_next_dn ){
        if(arg_verbose) CONSOLE.Debug("%p waiting for DL bandwidth", this);
        g_next_dn = this;
      }
    }  // else deferring DL, unless limited.
  }else if( !stream.HaveMessage() ){  // could have been called post-handshake
    r = stream.Feed(BUF_DEF_SIZ, &rate_dl);
//  if(r>=0) CONSOLE.Debug("%p fed, now has %d bytes (msg=%d)",
//    this, r, (int)(stream.PeekMessage()));
  }
  if( r < 0 ){
    if(arg_verbose) CONSOLE.Debug("%p: %s", this,
      (r==-2) ? "remote closed" : strerror(errno));
    return -1;
  }

  while( r = stream.HaveMessage() ){
    if( r < 0 ) return -1;
    if( (r = MsgDeliver()) == -2 ){
      if(arg_verbose) CONSOLE.Debug("%p seed<->seed detected", this);
      m_want_again = 0;
    }
    if( r < 0 || stream.PickMessage() < 0 ) return -1;
  }

  return 0;
}

int btPeer::SendModule()
{
  if( stream.out_buffer.Count() && stream.Flush() < 0 ){
    if(arg_verbose) CONSOLE.Debug("%p: %s", this, strerror(errno));
    return -1;
  }

  if( !reponse_q.IsEmpty() && CouldReponseSlice() ){
    int limited = WORLD.BandWidthLimitUp(Self.LateUL());
    if( !g_next_up || g_next_up==this ){
      if( !limited ){
        if( g_next_up ) g_next_up = (btPeer *)0;
        StartULTimer();
        Self.StartULTimer();
        if( ReponseSlice() < 0 ) return -1;
        Self.OntimeUL(0);
      }
      else if( !g_next_up ){
        if(arg_verbose) CONSOLE.Debug("%p waiting for UL bandwidth", this);
        g_next_up = this;
        if( g_defer_up ) g_defer_up = 0;
      }
    }else if( !limited ){
      if(arg_verbose) CONSOLE.Debug("%p deferring UL to %p", this, g_next_up);
      if( !g_defer_up ) g_defer_up = 1;
      WORLD.Defer();
    }
  }else if( g_next_up == this ) g_next_up = (btPeer *)0;

  return (!m_state.remote_choked) ? RequestCheck() : 0;
}

// Prevent a peer object from holding g_next_up when it's not ready to write.
int btPeer::CheckSendStatus()
{
  if( g_next_up == this && !WORLD.BandWidthLimitUp(Self.LateUL()) ){
    if(arg_verbose){
      CONSOLE.Debug("%p is not write-ready", this);
      if( g_defer_up ) CONSOLE.Debug("%p skipped UL", this);
    }
    g_next_up = (btPeer *)0;
  }
  return g_next_up ? 1 : 0;
}

/* Detect if a peer ignored, discarded, or lost my request and we're waiting
   for a piece that may never arrive. */
int btPeer::HealthCheck()
{
  if( BTCONTENT.IsFull() ){
    // Catch seeders who suppress HAVE and don't disconnect other seeders,
    // or who just sit there and waste a connection.
    if( m_health_time <= now - 300 ){
      m_health_time = now;
      if( !m_state.remote_interested ){
        if( m_bad_health ) return -1;
        m_bad_health = 1;
      } else m_bad_health = 0;
    }
  }else if( m_health_time <= now - 60 ){
    m_health_time = now;
    if( !m_state.remote_choked && m_req_out &&
        m_receive_time < now - (!m_latency ? 300 :
                               ((m_latency < 30) ? 60 : (2*m_latency))) ){
      // if a repeat occurrence, get rid of the peer
      if( m_bad_health ) return -1;
      m_bad_health = 1;
      if(arg_verbose)
        CONSOLE.Debug("%p unresponsive; resetting request queue", this);
      int retval = CancelRequest(request_q.GetHead());
      PENDINGQUEUE.Pending(&request_q);
      m_req_out = 0;
      return (retval < 0) ? -1 : RequestCheck();
    } else m_bad_health = 0;
  }
  return 0;
}

// This handles peers that suppress HAVE messages so that we don't always think
// that they're empty.  If we've sent the peer an amount of data equivalent to
// two pieces, assume that they now have at least one complete piece.
int btPeer::IsEmpty() const
{
  return ( bitfield.IsEmpty() && TotalUL() < BTCONTENT.GetPieceLength()*2 ) ?
    1:0;
}

int btPeer::PutPending()
{
  int retval = 0;

  if( !request_q.IsEmpty() ){
    retval = CancelRequest(request_q.GetHead());
    PENDINGQUEUE.Pending(&request_q);
  }
  m_req_out = 0;
  return retval;
}

int btPeer::NeedPrefetch() const
{
  if( P_SUCCESS == m_status &&
      ( Is_Local_UnChoked() ||
        (!BTCONTENT.IsFull() && Is_Remote_UnChoked() &&
         m_prefetch_completion < 2 && request_q.LastSlice()) ) )
    return 1;
  else return 0;
}

// Call NeedPrefetch() first, which checks additional conditions!
void btPeer::Prefetch(time_t deadline)
{
  size_t rd, ru;
  size_t idx, off, len;
  time_t predict, next_chance;

  if( !BTCONTENT.IsFull() && Is_Remote_UnChoked() &&
      m_prefetch_completion < 2 && request_q.LastSlice() && (rd=RateDL()) > 0 &&
      request_q.Peek(&idx, &off, &len)==0 &&
      m_last_timestamp + len / rd < now + WORLD.GetUnchokeInterval() &&
      m_last_timestamp + len / rd <
        now + (cfg_cache_size*1024*1024 - BTCONTENT.GetPieceLength(idx)) /
              Self.RateDL() ){
    switch( BTCONTENT.CachePrep(idx) ){
    case -1:  // don't prefetch
      m_prefetch_completion = 2;
      break;
    case 0:  // ready, no data flushed
      if( m_prefetch_completion || off==0 ){
        if( off+len < BTCONTENT.GetPieceLength(idx) )
          BTCONTENT.ReadSlice(NULL, idx, off+len,
            BTCONTENT.GetPieceLength(idx)-off-len);
        m_prefetch_completion = 2;
      }else{
        BTCONTENT.ReadSlice(NULL, idx, 0, off);
        if( off+len < BTCONTENT.GetPieceLength(idx) )
          m_prefetch_completion = 1;
        else m_prefetch_completion = 2;
      }
      break;
    case 1:  // data was flushed (time used)
      break;
    }
  }
  else if( Is_Local_UnChoked() && reponse_q.Peek(&idx, &off, &len) == 0 ){
    if( cfg_max_bandwidth_up )
      next_chance = (time_t)( Self.LastSendTime() +
                    (double)(Self.LastSizeSent()) / cfg_max_bandwidth_up );
    else next_chance = now;

    if( g_next_up ){
      if( g_next_up != this ){
        // deferral pending; we'll get another chance to prefetch
        return;
      }else m_next_send_time = next_chance;  // I am the next sender
    }
    if( m_next_send_time < next_chance ) predict = next_chance;
    else predict = m_next_send_time;

    // Don't prefetch if it will expire from cache before being sent.
    if( predict < deadline && (0==(rd = Self.RateDL()) ||
        predict <= now + cfg_cache_size*1024*1024 / rd) ){
      // This allows re-prefetch if it might have expired from the cache.
      if( !m_prefetch_time || (0==rd && 0==(ru = Self.RateUL())) ||
          now - m_prefetch_time > BTCONTENT.CacheSize() / (rd + ru) ){
        BTCONTENT.ReadSlice(NULL, idx, off, len);
        m_prefetch_time = now;
      }
    }
  }
}

void btPeer::dump()
{
  struct sockaddr_in sin;

  GetAddress(&sin);
  CONSOLE.Print("%s: %d -> %d:%d   %llud:%lluu", inet_ntoa(sin.sin_addr), 
          bitfield.Count(),
          Is_Remote_UnChoked() ? 1 : 0,
          request_q.IsEmpty() ? 0 : 1,
          (unsigned long long)TotalDL(),
          (unsigned long long)TotalUL());
}

