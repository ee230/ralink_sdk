#ifndef BTFILES_H
#define BTFILES_H

#include "./def.h"
#include <inttypes.h>
#include <sys/types.h>
#include <stdio.h>
#include <time.h>

#include "bitfield.h"
#include "btconfig.h"

typedef struct _btfile{
  char *bf_filename;	// full path of file.
  uint64_t bf_length;
  FILE *bf_fp;

  time_t bf_last_timestamp;	// last io timestamp.

  size_t bf_npieces;  //number of pieces

  unsigned char bf_flag_opened:1;
  unsigned char bf_flag_readonly:1;
  unsigned char bf_reserved:6;

  struct _btfile *bf_next;
}BTFILE;


class btFiles
{
 private:
  
  BTFILE *m_btfhead;
  char *m_directory;
  uint64_t m_total_files_length;
  size_t m_total_opened;	// already opened
  size_t m_nfiles;
  BTFILE **m_file;
  char m_buffer[DEFAULT_SLICE_SIZE];

  uint8_t m_flag_automanage:1;
  uint8_t m_flag_reserved:7;	// current version not implement

  BTFILE* _new_bfnode();
  int _btf_close_oldest();
  int _btf_close(BTFILE *pbf);
  int _btf_open(BTFILE *pbf, const int iotype);
  int _btf_ftruncate(int fd, int64_t length);
  int _btf_creat_by_path(const char *pathname, int64_t file_length);
  int _btf_destroy();
  int _btf_recurses_directory(const char *cur_path, BTFILE **plastnode);
  int ConvertFilename(char *dst, const char *src, int size);

 public:
  int CreateFiles();
  void CloseFile(size_t nfile);

  btFiles();
  ~btFiles();
  
  int BuildFromFS(const char *pathname);
  int BuildFromMI(const char *metabuf, const size_t metabuf_len,
                  const char *saveas);

  char *GetDataName() const;
  uint64_t GetTotalLength() const { return m_total_files_length; }
  ssize_t IO(char *buf, uint64_t off, size_t len, const int iotype);
  size_t FillMetaInfo(FILE* fp);

  void SetFilter(int nfile, BitField *pFilter, size_t pieceLength);

  size_t GetNFiles() const { return m_nfiles; }
  char *GetFileName(size_t nfile) const;
  uint64_t GetFileSize(size_t nfile) const;
  size_t GetFilePieces(size_t nfile) const;

  void PrintOut();
};

#endif
