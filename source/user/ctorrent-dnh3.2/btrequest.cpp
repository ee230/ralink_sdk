#include "btrequest.h"  // def.h

#include <stdlib.h>

#include "btcontent.h"
#include "btconfig.h"
#include "console.h"


static void _empty_slice_list(PSLICE *ps_head)
{
  PSLICE p;
  for(; *ps_head;){
    p = (*ps_head)->next;
    delete (*ps_head);
    *ps_head = p;
  }
}

RequestQueue::~RequestQueue()
{
  if( rq_head ) _empty_slice_list(&rq_head);
}

RequestQueue::RequestQueue()
{
  rq_head = (PSLICE) 0;
  rq_send = rq_head;
}

void RequestQueue::Empty()
{
  if(rq_head) _empty_slice_list(&rq_head);
  rq_send = rq_head;
}

void RequestQueue::SetHead(PSLICE ps)
{
  if( rq_head ) _empty_slice_list(&rq_head);
  rq_head = ps;
  rq_send = rq_head;
}

void RequestQueue::operator=(RequestQueue &rq)
{
  PSLICE n, u = (PSLICE) 0;
  size_t idx;
  int flag = 0;

  if( rq_head ) _empty_slice_list(&rq_head);
  rq_head = rq.rq_head;
  rq_send = rq_head;

  // Reassign only the first piece represented in the queue.
  n = rq_head;
  idx = n->index;
  for( ; n ; u = n,n = u->next ){
    if( rq.rq_send == n ) flag = 1;
    if( n->index != idx ) break;
  }
  if(n){
    u->next = (PSLICE) 0;
    rq.rq_head = n;
    if(flag) rq.rq_send = rq.rq_head;
  }else{
    rq.rq_head = (PSLICE) 0;
    rq.rq_send = rq.rq_head;
  }
}

int RequestQueue::Copy(RequestQueue *prq)
{
  PSLICE n, u=(PSLICE)0, ps;
  size_t idx;

  if( prq->IsEmpty() ) return 0;

  ps = prq->GetHead();
  idx = ps->index;
  for( ; ps; ps = ps->next ){
    if( ps->index != idx ) break;
    if( Add(ps->index, ps->offset, ps->length) < 0 ) return -1;
  }
  return 0;
}

int RequestQueue::CopyShuffle(RequestQueue *prq, size_t piece)
{
  PSLICE n, u=(PSLICE)0, ps, start;
  size_t idx;
  unsigned long rndbits;
  int i=0;

  if( prq->IsEmpty() ) return 0;

  n = rq_head;
  for( ; n ; u = n,n = u->next ); // move to end

  ps = prq->GetHead();
  for( ; ps && ps->index != piece; ps = ps->next );
  start = ps;

  idx = ps->index;

  // First, skip to the slices that haven't been sent to the original peer.
  if( prq->rq_send && prq->rq_send->index == idx ){
    ps = prq->rq_send;
    for( ; ps; ps = ps->next ){
      if( ps->index != idx ) break;
      if( !i-- ){
        rndbits = random();
        i = 30;
      }
      if( (rndbits>>=1)&01 ){
        if( Add(ps->index, ps->offset, ps->length) < 0 ) return -1;
      }
      else if( Insert(u, ps->index, ps->offset, ps->length) < 0 ) return -1;
    }
    if(u) n = u->next;
    else n = rq_head;
    for( ; n ; u = n,n = u->next ); // move to end
  }

  // Now put the already-requested slices at the end.
  ps = start;
  for( ; ps && ps != prq->rq_send; ps = ps->next ){
    if( ps->index != idx ) break;
    if( !i-- ){
      rndbits = random();
      i = 30;
    }
    if( (rndbits>>=1)&01 ){
      if( Add(ps->index, ps->offset, ps->length) < 0 ) return -1;
    }
    else if( Insert(u, ps->index, ps->offset, ps->length) < 0 ) return -1;
  }
  return 0;
}

// Counts all queued slices.
size_t RequestQueue::Qsize() const
{
  size_t cnt = 0;
  PSLICE n = rq_head;
  PSLICE u = (PSLICE) 0;

  for( ; n ; u = n,n = u->next ) cnt++; // move to end
  return cnt;
}

// Counts only slices from one piece.
size_t RequestQueue::Qlen(size_t piece) const
{
  size_t cnt = 0;
  PSLICE n = rq_head;
  PSLICE u = (PSLICE) 0;
  size_t idx;

  for( ; n && n->index != piece; n = n->next );

  if(n) idx = n->index;
  for( ; n ; u = n,n = u->next ){
    if( n->index != idx ) break;
    cnt++;
  }
  return cnt;
}

int RequestQueue::LastSlice() const
{
  return ( rq_head &&
          (!rq_head->next || rq_head->index != rq_head->next->index) ) ? 1 : 0;
}

int RequestQueue::Insert(PSLICE ps,size_t idx,size_t off,size_t len)
{
  PSLICE n;

  n = new SLICE;

#ifndef WINDOWS
  if( !n ) return -1;
#endif

  n->index = idx;
  n->offset = off;
  n->length = len;
  n->reqtime = (time_t) 0;

  // ps is the slice to insert after; if 0, insert at the head.
  if(ps){
    n->next = ps->next;
    ps->next = n;
    if( rq_send == n->next ) rq_send = n;
  }else{
    n->next = rq_head;
    rq_head = n;
    rq_send = rq_head;
  }

  return 0;
}

int RequestQueue::Add(size_t idx,size_t off,size_t len)
{
  PSLICE n = rq_head;
  PSLICE u = (PSLICE) 0;

  for( ; n ; u = n,n = u->next ); // move to end

  n = new SLICE;

#ifndef WINDOWS
  if( !n ) return -1;
#endif

  n->next = (PSLICE) 0;
  n->index = idx;
  n->offset = off;
  n->length = len;
  n->reqtime = (time_t) 0;

  if( u ) u->next = n;
  else{
    rq_head = n;
    rq_send = rq_head;
  }

  if( !rq_send ) rq_send = n;

  return 0;
}

int RequestQueue::Append(PSLICE ps)
{
  PSLICE n = rq_head;
  PSLICE u = (PSLICE) 0;

  for( ; n ; u = n,n = u->next ); // move to end

  if(u) u->next = ps;
  else rq_head = ps;

  if( !rq_send ) rq_send = ps;

  return 0;
}

int RequestQueue::Remove(size_t idx,size_t off,size_t len)
{
  PSLICE n = rq_head;
  PSLICE u = (PSLICE) 0;

  for( ; n ; u = n, n = u->next ){
    if( n->index == idx && n->offset == off && n->length == len ) break;
  }

  if( !n ) return -1;	/* not found */

  if( u ) u->next = n->next; else rq_head = n->next;
  if( rq_send == n ) rq_send = n->next;
  delete n;

  return 0;
}

// Add a slice at an appropriate place in the queue.
// returns -1 if failed, 1 if request needs to be sent.
int RequestQueue::Requeue(size_t idx,size_t off,size_t len)
{
  int f_send, retval;
  PSLICE n = rq_head;
  PSLICE u = (PSLICE) 0;
  PSLICE save_send = rq_send;

  // find last slice of same piece
  if( rq_send ) f_send = 1;
  for( ; n ; u = n,n = u->next ){
    if( rq_send == u ) f_send = 0;
    if( u && idx == u->index && idx != n->index ) break;
  }
  if( !u ) f_send = 1;

  retval = ( Insert(u,idx,off,len) < 0 ) ? -1 : f_send;
  rq_send = save_send;
  return retval;
}

int RequestQueue::HasIdx(size_t idx) const
{
  PSLICE n = rq_head;

  for( ; n ; n = n->next ){
    if(n->index == idx) break;
  }

  return n ? 1 : 0;
}

time_t RequestQueue::GetReqTime(size_t idx,size_t off,size_t len) const
{
  PSLICE n = rq_head;

  for( ; n ; n = n->next ){
    if( n->index == idx && n->offset == off && n->length == len ) break;
  }

  if( !n ) return (time_t)0;	/* not found */

  return n->reqtime;
}

void RequestQueue::SetReqTime(PSLICE n,time_t t)
{
  n->reqtime = t;
}

int RequestQueue::Pop(size_t *pidx,size_t *poff,size_t *plen)
{
  PSLICE n;

  if( !rq_head ) return -1;

  n = rq_head->next;

  if(pidx) *pidx = rq_head->index;
  if(poff) *poff = rq_head->offset;
  if(plen) *plen = rq_head->length;

  if( rq_send == rq_head ) rq_send = n;
  delete rq_head;

  rq_head = n;

  return 0;
}

int RequestQueue::Peek(size_t *pidx,size_t *poff,size_t *plen) const
{
  if( !rq_head ) return -1;

  if(pidx) *pidx = rq_head->index;
  if(poff) *poff = rq_head->offset;
  if(plen) *plen = rq_head->length;

  return 0;
}

int RequestQueue::CreateWithIdx(size_t idx)
{
  size_t i,off,len,ns;
  
  ns = NSlices(idx);

  for( i = off = 0; i < ns; i++ ){
    len = Slice_Length(idx,i);
    if( Add(idx,off,len) < 0 ) return -1;
    off += len;
  }

  return 0;
}

size_t RequestQueue::Slice_Length(size_t idx,size_t sidx) const
{
  size_t plen = BTCONTENT.GetPieceLength(idx);

  return (sidx == ( plen / cfg_req_slice_size)) ?
    (plen % cfg_req_slice_size) : 
    cfg_req_slice_size;
}

size_t RequestQueue::NSlices(size_t idx) const
{
  size_t r,n;
  r = BTCONTENT.GetPieceLength(idx);
  n = r / cfg_req_slice_size;
  return ( r % cfg_req_slice_size ) ? n + 1 : n;
}

int RequestQueue::IsValidRequest(size_t idx,size_t off,size_t len) const
{
  return ( idx < BTCONTENT.GetNPieces() &&
           len &&
           (off + len) <= BTCONTENT.GetPieceLength(idx) &&
           len <= cfg_max_slice_size ) ?
    1 : 0;
}

// ****************************** PendingQueue ******************************

PendingQueue PENDINGQUEUE;

PendingQueue::PendingQueue()
{
  int i = 0;
  for(; i < PENDING_QUEUE_SIZE; i++) pending_array[i] = (PSLICE) 0;
  pq_count = 0;
}

PendingQueue::~PendingQueue()
{
  if(pq_count) Empty();
}

void PendingQueue::Empty()
{
  int i = 0;
  for ( ; i < PENDING_QUEUE_SIZE && pq_count; i++ )
    if( pending_array[i] != (PSLICE) 0 ){ 
      _empty_slice_list(&(pending_array[i])); 
      pq_count--; 
    }
}

int PendingQueue::Exist(size_t idx) const
{
  int i = 0;
  for ( ; i < PENDING_QUEUE_SIZE && pq_count; i++ ){
    if( (PSLICE) 0 != pending_array[i] && idx == pending_array[i]->index )
      return 1;
  }
  return 0;
}

// Sending an empty queue to this function WILL cause a crash.  This exposure
// is left open in order to help track down bugs that cause this condition.
int PendingQueue::Pending(RequestQueue *prq)
{
  int i = 0, j = -1;
  PSLICE n, u = (PSLICE) 0;
  size_t idx, off, len;
  RequestQueue tmprq;

  if( pq_count >= PENDING_QUEUE_SIZE ){
    prq->Empty();
    return -1;
  }
  if( prq->Qlen(prq->GetRequestIdx()) >=
      BTCONTENT.GetPieceLength() / cfg_req_slice_size ){
    // This shortcut relies on the fact that we don't add to a queue if it
    // already contains a full piece.
    prq->Empty();
    return 0;
  }

  for( ; i < PENDING_QUEUE_SIZE; i++ ){
    if( pending_array[i] == (PSLICE)0 ){
      // Find an empty slot in case we need it.
      if(j<0) j = i;
    }else if( prq->GetRequestIdx() == pending_array[i]->index ){
      // Don't add a piece to Pending more than once.
      while( !prq->IsEmpty() &&
          prq->GetRequestIdx() == pending_array[i]->index )
        prq->Pop(&idx,&off,&len);
      if( prq->IsEmpty() ) return 0;
      i = 0;
    }
  }
  pending_array[j] = prq->GetHead();
  prq->Release();
  pq_count++;

  // If multiple pieces are queued, break up the queue separately.
  n = pending_array[j];
  idx = n->index;
  for( ; n ; u = n, n = u->next ){
    if( n->index != idx ) break;
    n->reqtime = (time_t)0;
  }
  if(n){
    u->next = (PSLICE) 0;
    tmprq.SetHead(n);
    Pending(&tmprq);
    tmprq.Release();
  }

  return 0;
}

int PendingQueue::ReAssign(RequestQueue *prq, BitField &bf)
{
  int i = 0;
  size_t sc = pq_count;
  size_t idx;
  for( ; i < PENDING_QUEUE_SIZE && sc; i++ ){
    if( pending_array[i] != (PSLICE) 0){
      if( bf.IsSet(pending_array[i]->index) &&
          !prq->HasIdx(pending_array[i]->index) ){
        idx = pending_array[i]->index;
        prq->Append(pending_array[i]);
        pending_array[i] = (PSLICE) 0;
        pq_count--;
        Delete(idx); // delete any copies from Pending
        break;
      }
      sc--;
    }
  }
  // Return value now indicates whether a piece was assigned.
  return sc;
}

// This routine should no longer be necessary, but keeping it as a failsafe.
int PendingQueue::Delete(size_t idx)
{
  int i = 0;
  for ( ; i < PENDING_QUEUE_SIZE && pq_count; i++ ){
    if( (PSLICE) 0 != pending_array[i] && idx == pending_array[i]->index ){
      if(arg_verbose) CONSOLE.Debug("PQD found %d", (int)idx);
      _empty_slice_list(&(pending_array[i])); 
      pq_count--;
      break;
    }
  }
  return 0;
}

int PendingQueue::DeleteSlice(size_t idx, size_t off, size_t len)
{
  int i = 0;
  RequestQueue rq;
  for( ; i < PENDING_QUEUE_SIZE && pq_count; i++ ){
    if( (PSLICE) 0 != pending_array[i] && idx == pending_array[i]->index ){
      //check if off & len match any slice
      //remove the slice if so
      rq.SetHead(pending_array[i]);
      if( rq.Remove(idx, off, len) == 0 ){
        pending_array[i] = rq.GetHead();
        if( (PSLICE) 0 == pending_array[i] ) pq_count--;
        i = PENDING_QUEUE_SIZE;   // exit loop
      }
      rq.Release();
    }
  }
  return 0;
}

