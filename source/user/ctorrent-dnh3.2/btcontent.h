#ifndef BTCONTENT_H
#define BTCONTENT_H

#include "def.h"
#include <inttypes.h>
#include <sys/types.h>

#include <time.h>
#include "bitfield.h"
#include "btfiles.h"

typedef struct _btcache{
  uint64_t bc_off;
  size_t bc_len;
  
  unsigned char bc_f_flush:1;
  unsigned char bc_f_reserved:7;
  
  char *bc_buf;

  struct _btcache *bc_next;
  struct _btcache *bc_prev;
  struct _btcache *age_next;
  struct _btcache *age_prev;
}BTCACHE;

typedef struct _btflush{
  size_t idx;
  struct _btflush *next;
}BTFLUSH;

typedef struct _bfnode{
  char *name;
  BitField bitfield;
  struct _bfnode *next;

  _bfnode(){
    name = (char *)0;
    next = (struct _bfnode *)0;
  }
  ~_bfnode(){
    if(name) delete []name;
  }
}BFNODE;

class btContent
{
  //METAINFO³ÉÔ±
  char *m_announce;
  unsigned char *m_hash_table;
  unsigned char m_shake_buffer[68];

  size_t m_hashtable_length;
  size_t m_piece_length;
  size_t m_npieces, m_check_piece;
  time_t m_create_date, m_seed_timestamp, m_start_timestamp;

  uint64_t m_left_bytes;
  btFiles m_btfiles;

  time_t m_flush_failed, m_flush_tried;

  BTCACHE **m_cache, *m_cache_oldest, *m_cache_newest;
  size_t m_cache_size, m_cache_used;
  size_t m_cache_hit, m_cache_miss, m_cache_pre;
  time_t m_cache_eval_time;
  BTFLUSH *m_flushq;

  BFNODE *m_filters, *m_current_filter;
  
  void _Set_InfoHash(unsigned char buf[20]);
  char* _file2mem(const char *fname, size_t *psiz);
  
  void ReleaseHashTable(){
    if(m_hash_table){
      delete []m_hash_table;
      m_hash_table = (unsigned char*) 0;
    }
  }

  int CheckExist();
  void CacheClean(size_t need);
  void CacheEval();
  uint64_t max_uint64_t(uint64_t a,uint64_t b) { return (a > b) ? a : b; }
  uint64_t min_uint64_t(uint64_t a,uint64_t b) { return (a > b) ? b : a; }
  ssize_t CacheIO(char *buf, uint64_t off, size_t len, int method);
  void FlushEntry(BTCACHE *p);

 public:
  BitField *pBF;
  BitField *pBMasterFilter;
  BitField *pBRefer;
  BitField *pBChecked;
  char *global_piece_buffer;
  
  btContent();
  ~btContent();
  
  void CacheConfigure();
  void FlushCache();
  void FlushPiece(size_t idx);
  void Uncache(size_t idx);
  void FlushQueue();
  int NeedFlush() const;
  int FlushFailed() const { return m_flush_failed ? 1 : 0 ; }

  int CreateMetainfoFile(const char *mifn);
  int InitialFromFS(const char *pathname, char *ann_url, size_t piece_length);
  int InitialFromMI(const char *metainfo_fname,const char *saveas);

  int CheckNextPiece();
  size_t CheckedPieces() const { return m_check_piece; }

  char* GetAnnounce() { return m_announce;}

  unsigned char* GetShakeBuffer() {return m_shake_buffer;}
  unsigned char* GetInfoHash() {return (m_shake_buffer + 28);}
  unsigned char* GetPeerId() {return (m_shake_buffer + 48); }

  size_t GetPieceLength(size_t idx);
  size_t GetPieceLength() const { return m_piece_length; }
  size_t GetNPieces() const { return m_npieces; }

  uint64_t GetTotalFilesLength() const { return m_btfiles.GetTotalLength(); }
  uint64_t GetLeftBytes() const { return m_left_bytes; }

  int APieceComplete(size_t idx);
  int GetHashValue(size_t idx,unsigned char *md);

  int CachePrep(size_t idx);
  ssize_t ReadSlice(char *buf,size_t idx,size_t off,size_t len);
  ssize_t WriteSlice(char *buf,size_t idx,size_t off,size_t len);
  ssize_t ReadPiece(char *buf,size_t idx);

  int PrintOut();
  int PrintFiles();
  int SeedTimeout();
  void CompletionCommand();
  void SaveBitfield();

  void CheckFilter();
  void SetFilter();
  BitField *GetFilter() const {
    return m_current_filter ? &(m_current_filter->bitfield) : (BitField *)0;
  }
  BitField *GetNextFilter() const { return GetNextFilter((BitField *)0); }
  BitField *GetNextFilter(BitField *pfilter) const;
  char *GetFilterName() const { return m_current_filter->name; }
  void SetTmpFilter(int nfile, BitField *pFilter){
    m_btfiles.SetFilter(nfile, pFilter, m_piece_length);
  }

  size_t GetNFiles() const { return m_btfiles.GetNFiles(); }
  char *GetFileName(size_t nfile) const {
    return m_btfiles.GetFileName(nfile);
  }
  uint64_t GetFileSize(size_t nfile) const {
    return m_btfiles.GetFileSize(nfile);
  }
  size_t GetFilePieces(size_t nfile) const {
    return m_btfiles.GetFilePieces(nfile);
  }

  time_t GetStartTime() const { return m_start_timestamp; }
  time_t GetSeedTime() const { return m_seed_timestamp; }

  int IsFull() const { return pBF->IsFull(); }
  int Seeding() const;

  size_t CacheHits() const { return m_cache_hit; }
  // "miss" does not count prefetch reads from disk
  size_t CacheMiss() const { return m_cache_miss; }
  // instead, "pre" does
  size_t CachePre() const { return m_cache_pre; }
  size_t CacheSize() const { return m_cache_size; }
  size_t CacheUsed() const { return m_cache_used; }

  void DumpCache();
};

extern btContent BTCONTENT;

#endif
