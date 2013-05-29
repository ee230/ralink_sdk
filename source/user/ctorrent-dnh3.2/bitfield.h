#ifndef BITFIELD_H
#define BITFIELD_H

#include <sys/types.h>

class BitField
{
 private:
  static size_t nbits;
  static size_t nbytes;

  unsigned char *b;
  size_t nset;

  void _recalc();
  void _setall(unsigned char* buf);
  void _set(size_t idx);

 public:
  BitField();
  BitField(size_t n_bits);
  BitField(const BitField &bf);
  ~BitField(){ if(b) delete []b; }

  void operator=(const BitField &bf);

  void SetAll();
  void Clear();
  void SetReferBuffer(char *buf);
  void Set(size_t idx);
  void UnSet(size_t idx);

  int IsSet(size_t idx) const;
  int IsFull() const { return (nset >= nbits) ? 1 : 0; }
  int IsEmpty() const { return (nset == 0) ? 1 : 0; }

  size_t Count() const { return nset;}
  size_t NBytes() const { return nbytes; }
  size_t NBits() const { return nbits; }
  size_t Random() const;

  void Comb(const BitField &bf); 
  void Comb(const BitField *pbf) { if(pbf) Comb(*pbf); }
  void Except(const BitField &bf);
  void Except(const BitField *pbf) { if(pbf) Except(*pbf); }
  void And(const BitField &bf);
  void And(const BitField *pbf) { if(pbf) And(*pbf); }
  void Invert();

  int WriteToFile(const char *fname);
  int SetReferFile(const char *fname);
  
  void WriteToBuffer(char *buf);
};

#endif
