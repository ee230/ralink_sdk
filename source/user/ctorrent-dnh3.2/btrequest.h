#ifndef SLICE_H
#define SLICE_H

#include "def.h"

#include <sys/types.h>
#include <time.h>
#include "btcontent.h"
#include "bitfield.h"

typedef struct _slice{
   size_t index;
   size_t offset;
   size_t length;
   time_t reqtime;
   struct _slice *next;
}SLICE,*PSLICE;

class RequestQueue
{
 private:
  PSLICE rq_head;
 public:
  PSLICE rq_send;  // next slice to request

  RequestQueue();
  ~RequestQueue();

  void Empty();

  void SetHead(PSLICE ps);
  void SetNextSend(PSLICE ps) { rq_send = ps; }
  PSLICE GetHead() const { return rq_head; }
  PSLICE NextSend() const { return rq_send; }
  size_t GetRequestIdx() const { return rq_head ? rq_head->index :
                                                  BTCONTENT.GetNPieces(); }
  size_t GetRequestLen() const { return rq_head ? rq_head->length : 0; }
  void Release(){ rq_head = rq_send = (PSLICE) 0; }
  int IsValidRequest(size_t idx,size_t off,size_t len) const;

  void operator=(RequestQueue &rq);
  int Copy(RequestQueue *prq);
  int CopyShuffle(RequestQueue *prq, size_t piece);
  size_t Qsize() const;
  size_t Qlen(size_t piece) const;
  int LastSlice() const;

  int IsEmpty() const { return rq_head ? 0 : 1; }

  int Insert(PSLICE ps,size_t idx,size_t off,size_t len);
  int Add(size_t idx,size_t off,size_t len);
  int Append(PSLICE ps);
  int Remove(size_t idx,size_t off,size_t len);
  int Requeue(size_t idx,size_t off,size_t len);
  int HasIdx(size_t idx) const;
  time_t GetReqTime(size_t idx,size_t off,size_t len) const;
  void SetReqTime(PSLICE n,time_t t);


  int Pop(size_t *pidx,size_t *poff,size_t *plen);
  int Peek(size_t *pidx,size_t *poff,size_t *plen) const;

  int CreateWithIdx(size_t idx);
  size_t NSlices(size_t idx) const;
  size_t Slice_Length(size_t idx,size_t sidx) const;
};

#define PENDING_QUEUE_SIZE 100

class PendingQueue
{
 private:
  PSLICE pending_array[PENDING_QUEUE_SIZE];
  size_t pq_count;
  
 public:
  PendingQueue();
  ~PendingQueue();
  void Empty();
  int Pending(RequestQueue *prq);
  int ReAssign(RequestQueue *prq, BitField &bf);
  int Exist(size_t idx) const;
  int Delete(size_t idx);
  int DeleteSlice(size_t idx, size_t off, size_t len);
};

extern PendingQueue PENDINGQUEUE;

#endif
