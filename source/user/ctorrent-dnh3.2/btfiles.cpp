#include "btfiles.h"

#ifdef WINDOWS
#include <io.h>
#include <memory.h>
#include <direct.h>
#else
#include <unistd.h>
#include <dirent.h>
#include <sys/param.h>
#endif

#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>  // isprint

#include "bencode.h"
#include "btcontent.h"
#include "bitfield.h"
#include "console.h"
#include "bttime.h"

#ifndef HAVE_SNPRINTF
#include "compat.h"
#endif

#define MAX_OPEN_FILES 20

btFiles::btFiles()
{
  m_btfhead = (BTFILE*) 0;
  m_nfiles = 0;
  m_file = (BTFILE **)0;
  m_total_files_length = 0;
  m_total_opened = 0;
  m_flag_automanage = 1;
  m_directory = (char*)0;
}

btFiles::~btFiles()
{
  _btf_destroy();
  if( m_directory ) delete []m_directory;
}

BTFILE* btFiles::_new_bfnode()
{
  BTFILE *pnew = new BTFILE;

#ifndef WINDOWS
  if( !pnew ) return (BTFILE*) 0;
#endif

  pnew->bf_flag_opened = 0;
  pnew->bf_flag_readonly = 0;

  pnew->bf_filename = (char*) 0;
  pnew->bf_fp = (FILE*) 0;
  pnew->bf_length = 0;

  pnew->bf_last_timestamp = (time_t) 0;
  pnew->bf_next = (BTFILE*) 0;
  return pnew;
}

void btFiles::CloseFile(size_t nfile)
{
  if( nfile && nfile <= m_nfiles )
    _btf_close(m_file[nfile-1]);
}

int btFiles::_btf_close_oldest()
{
  BTFILE *pbf_n,*pbf_close;
  pbf_close = (BTFILE *) 0;
  for(pbf_n = m_btfhead; pbf_n; pbf_n = pbf_n->bf_next){
    if(!pbf_n->bf_flag_opened) continue; // file not been opened.
    if(!pbf_close || pbf_n->bf_last_timestamp < pbf_close->bf_last_timestamp)
      pbf_close = pbf_n;
  }
  if(!pbf_close) return -1;
  return _btf_close(pbf_close);
}

int btFiles::_btf_close(BTFILE *pbf)
{
  if( !pbf->bf_flag_opened ) return 0;

  if( fclose(pbf->bf_fp) == EOF )
    CONSOLE.Warning(2, "warn, error closing file \"%s\":  %s",
      pbf->bf_filename, strerror(errno));
  pbf->bf_flag_opened = 0;
  pbf->bf_fp = (FILE *)0;
  m_total_opened--;
  return 0;
}

int btFiles::_btf_open(BTFILE *pbf, const int iotype)
{
  char fn[MAXPATHLEN];

  if( pbf->bf_flag_opened ){
    if( pbf->bf_flag_readonly && iotype ) _btf_close(pbf);
    else return 0;  // already open in a usable mode
  }
  
  if(m_flag_automanage && (m_total_opened >= MAX_OPEN_FILES)){  // close a file
    if( _btf_close_oldest() < 0 ) return -1;
  }

  if( m_directory ){
    if( MAXPATHLEN <= snprintf(fn, MAXPATHLEN, "%s%c%s", m_directory, PATH_SP,
                               pbf->bf_filename) )
      return -1;
  }else{
    strcpy(fn, pbf->bf_filename);
  }

  if( !(pbf->bf_fp = fopen(fn, iotype ? "r+b" : "rb")) ){
    if( EMFILE == errno || ENFILE == errno ){
      if( _btf_close_oldest() < 0 ||
          !(pbf->bf_fp = fopen(fn, iotype ? "r+b" : "rb")) )
        return -1;  // caller prints error
    }else return -1;  // caller prints error
  }
  setvbuf(pbf->bf_fp, m_buffer, _IOFBF, DEFAULT_SLICE_SIZE);

  pbf->bf_flag_opened = 1;
  pbf->bf_flag_readonly = iotype ? 0 : 1;
  m_total_opened++;
  return 0;
}

ssize_t btFiles::IO(char *buf, uint64_t off, size_t len, const int iotype)
{
  uint64_t n = 0;
  off_t pos,nio;
  BTFILE *pbf = m_btfhead;

  if( (off + (uint64_t)len) > m_total_files_length ){
    CONSOLE.Warning(1, "error, data offset %llu length %lu out of range",
      (unsigned long long)off, (unsigned long)len);
    return -1;
  }

  for(; pbf; pbf = pbf->bf_next){
    n += (uint64_t) pbf->bf_length;
    if(n > off) break;
  }

  if( !pbf ){
    CONSOLE.Warning(1, "error, failed to find file for offset %llu",
      (unsigned long long)off);
    return -1;
  }

  pos = off - (n - pbf->bf_length);

  for(; len ;){
    if( (!pbf->bf_flag_opened || (iotype && pbf->bf_flag_readonly)) &&
        _btf_open(pbf, iotype) < 0 ){
      CONSOLE.Warning(1, "error, failed to open file \"%s\":  %s",
        pbf->bf_filename, strerror(errno));
      return -1;
    }

    if( m_flag_automanage ) pbf->bf_last_timestamp = now;

#ifdef HAVE_FSEEKO
    if( fseeko(pbf->bf_fp,pos,SEEK_SET) < 0 ){
#else
    if( fseek(pbf->bf_fp,(long) pos,SEEK_SET) < 0 ){
#endif
      CONSOLE.Warning(1, "error, failed to seek to %llu on file \"%s\":  %s",
        (unsigned long long)pos, pbf->bf_filename, strerror(errno));
      return -1;
    }
//  }

    nio = (len < pbf->bf_length - pos) ? len : (pbf->bf_length - pos);

    if(0 == iotype){
      errno = 0;
      if( 1 != fread(buf,nio,1,pbf->bf_fp) && ferror(pbf->bf_fp) ){
        CONSOLE.Warning(1, "error, read failed at %llu on file \"%s\":  %s",
          (unsigned long long)pos, pbf->bf_filename, strerror(errno));
        return -1;
      }
    }else{
      errno = 0;
      if( 1 != fwrite(buf,nio,1,pbf->bf_fp) ){
        CONSOLE.Warning(1, "error, write failed at %llu on file \"%s\":  %s",
          (unsigned long long)pos, pbf->bf_filename, strerror(errno));
        return -1;
      }
      if( fflush(pbf->bf_fp) == EOF ){
        CONSOLE.Warning(1, "error, flush failed at %llu on file \"%s\":  %s",
          (unsigned long long)pos, pbf->bf_filename, strerror(errno));
        return -1;
      }
    }

    len -= nio;
    buf += nio;

    if( len ){
      pbf = pbf->bf_next;
      if( !pbf ){
        CONSOLE.Warning(1, "error, data left over with no more files to write");
        return -1;
      }
      pos = 0;
    }
  } // end for
  return 0;
}

int btFiles::_btf_destroy()
{
  BTFILE *pbf,*pbf_next;
  for(pbf = m_btfhead; pbf;){
    pbf_next = pbf->bf_next;
    if( pbf->bf_fp && pbf->bf_flag_opened ) fclose( pbf->bf_fp );
    if( pbf->bf_filename ) delete []pbf->bf_filename;
    delete pbf;
    pbf = pbf_next;
  }
  m_btfhead = (BTFILE*) 0;
  m_total_files_length = (uint64_t) 0;
  m_total_opened = 0;
  return 0;
}

int btFiles::_btf_ftruncate(int fd,int64_t length)
{
  if( arg_allocate ){
    char *c = new char[256*1024];
    if( !c ) return -1;
    memset(c, 0, 256*1024);
    int r, wlen;
    int64_t len = 0;
    for( int i=0; len < length; i++ ){
      if( len + 256*1024 > length ) wlen = (int)(length - len);
      else wlen = 256*1024;
      if( 0 == i % 100 ) CONSOLE.Interact_n(".");
      if( (r = write(fd, c, wlen)) < 0 ) return r;
      len += wlen;
    }
    return r;
  }
#ifdef WINDOWS
  char c = (char)0;
  if( lseek(fd,length - 1, SEEK_SET) < 0 ) return -1;
  return write(fd, &c, 1);
#else
  // ftruncate() not allowed on [v]fat under linux
  int retval = ftruncate(fd,length);
  if( retval < 0 ) {
    char c = (char)0;
    if( lseek(fd,length - 1, SEEK_SET) < 0 ) return -1;
    return write(fd, &c, 1);
  }
  else return retval;
#endif
}

int btFiles::_btf_recurses_directory(const char *cur_path, BTFILE* *plastnode)
{
  char full_cur[MAXPATHLEN];
  char fn[MAXPATHLEN];
  struct stat sb;
  struct dirent *dirp;
  DIR *dp;
  BTFILE *pbf;

  if( !getcwd(full_cur,MAXPATHLEN) ) return -1;

  if( cur_path ){
    strcpy(fn, full_cur);
    if( MAXPATHLEN <= snprintf(full_cur, MAXPATHLEN, "%s%c%s", fn, PATH_SP,
                               cur_path))
      return -1;
  }
      
  if( (DIR*) 0 == (dp = opendir(full_cur)) ){
    CONSOLE.Warning(1, "error, open directory \"%s\" failed:  %s",
      cur_path, strerror(errno));
    return -1;
  }

  while( (struct dirent*) 0 != (dirp = readdir(dp)) ){
    
    if( 0 == strcmp(dirp->d_name, ".") ||
        0 == strcmp(dirp->d_name, "..") ) continue;

    if( cur_path ){
      if(MAXPATHLEN < snprintf(fn, MAXPATHLEN, "%s%c%s", cur_path, PATH_SP,
                               dirp->d_name)){
        CONSOLE.Warning(1, "error, pathname too long");
        return -1;
      }
    }else{
      strcpy(fn, dirp->d_name);
    }

    if( stat(fn, &sb) < 0 ){
      CONSOLE.Warning(1, "error, stat \"%s\" failed:  %s",fn,strerror(errno));
      return -1;
    }

    if( S_IFREG & sb.st_mode ){
      
      pbf = _new_bfnode();
#ifndef WINDOWS
      if( !pbf ) return -1;
#endif
      pbf->bf_filename = new char[strlen(fn) + 1];
#ifndef WINDOWS
      if( !pbf->bf_filename ){ closedir(dp); return -1; }
#endif
      strcpy(pbf->bf_filename, fn);
      
      pbf->bf_length = sb.st_size;
      m_total_files_length += sb.st_size;

      if( *plastnode ) (*plastnode)->bf_next = pbf; else m_btfhead = pbf;

      *plastnode = pbf;

    }else if( S_IFDIR & sb.st_mode ){
      if(_btf_recurses_directory(fn, plastnode) < 0){closedir(dp); return -1;}
    }else{
      CONSOLE.Warning(1, "error, \"%s\" is not a directory or regular file.",
        fn);
      closedir(dp);
      return -1;
    }
  } // end while
  closedir(dp);
  return 0;
}

int btFiles::_btf_creat_by_path(const char *pathname, int64_t file_length)
{
  struct stat sb;
  int fd;
  char *p,*pnext,last = 0;
  char sp[MAXPATHLEN];

  strcpy(sp,pathname);

  pnext = sp;
  if(PATH_SP == *pnext) pnext++;

  for(; !last; ){
    for(p = pnext; *p && PATH_SP != *p; p++) ;
    if( !*p ) last = 1;
    if(last && PATH_SP == *p){ last = 0; break;}
    *p = '\0';
    if(stat(sp,&sb) < 0){
      if( ENOENT == errno ){
        if( !last ){
#ifdef WINDOWS
          if(mkdir(sp) < 0) break;
#else
          if(mkdir(sp,0755) < 0) break;
#endif
        }else{
          if((fd = creat(sp,0644)) < 0){ last = 0; break; }
          if(file_length && _btf_ftruncate(fd, file_length) < 0){
            close(fd); last = 0; break;
          }
          close(fd);
        }
      }else{last = 0; break;}
    }
    if( !last ){ *p = PATH_SP; pnext = p + 1; }
  }
  return last;
}

int btFiles::BuildFromFS(const char *pathname)
{
  struct stat sb;
  BTFILE *pbf = (BTFILE*) 0;
  BTFILE *lastnode = (BTFILE*) 0;

  if( stat(pathname, &sb) < 0 ){
    CONSOLE.Warning(1, "error, stat file \"%s\" failed:  %s",
      pathname, strerror(errno));
    return -1;
  }

  if( S_IFREG & sb.st_mode ){
    pbf = _new_bfnode();
#ifndef WINDOWS
    if( !pbf ) return -1;
#endif
    pbf->bf_length = m_total_files_length = sb.st_size;
    pbf->bf_filename = new char[strlen(pathname) + 1];
#ifndef WINDOWS
    if( !pbf->bf_filename ) return -1;
#endif
    strcpy(pbf->bf_filename, pathname);
    m_btfhead = pbf;
  }else if( S_IFDIR & sb.st_mode ){
    char wd[MAXPATHLEN];
    if( !getcwd(wd,MAXPATHLEN) ) return -1;
    m_directory = new char[strlen(pathname) + 1];
#ifndef WINDOWS
    if( !m_directory ) return -1;
#endif
    strcpy(m_directory, pathname);
    
    if(chdir(m_directory) < 0){
      CONSOLE.Warning(1, "error, change work directory to \"%s\" failed:  %s",
        m_directory, strerror(errno));
      return -1;
    }

    if(_btf_recurses_directory((const char*)0, &lastnode) < 0) return -1;
    if( chdir(wd) < 0) return -1;
  }else{
    CONSOLE.Warning(1, "error, \"%s\" is not a directory or regular file.",
      pathname);
    return -1;
  }
  return 0;
}

int btFiles::BuildFromMI(const char *metabuf, const size_t metabuf_len, const char *saveas)
{
  char path[MAXPATHLEN];
  const char *s, *p;
  size_t r,q,n;
  int64_t t;
  int f_warned = 0;

  if( !decode_query(metabuf, metabuf_len, "info|name", &s, &q, (int64_t*)0,
      QUERY_STR) || MAXPATHLEN <= q )
    return -1;

  memcpy(path, s, q);
  path[q] = '\0';

  r = decode_query(metabuf, metabuf_len, "info|files", (const char**)0, &q,
                   (int64_t*)0, QUERY_POS);

  if( r ){
    BTFILE *pbf_last = (BTFILE*) 0; 
    BTFILE *pbf = (BTFILE*) 0;
    size_t dl;
    if( decode_query(metabuf,metabuf_len,"info|length",
                    (const char**) 0,(size_t*) 0,(int64_t*) 0,QUERY_LONG) )
      return -1;

    if( saveas ){
      m_directory = new char[strlen(saveas) + 1];
#ifndef WINDOWS
      if(!m_directory) return -1;
#endif
      strcpy(m_directory,saveas);
    }else{
      int f_conv;
      char *tmpfn = new char[strlen(path)*2+5];
#ifndef WINDOWS
      if( !tmpfn ) return -1;
#endif
      if( f_conv = ConvertFilename(tmpfn, path, strlen(path)*2+5) ){
        if( arg_flg_convert_filenames ){
          m_directory = new char[strlen(tmpfn) + 1];
#ifndef WINDOWS
          if( !m_directory ){
            delete []tmpfn;
            return -1;
          }
#endif
          strcpy(m_directory,tmpfn);
        }else{
          CONSOLE.Warning(3,
            "Dir name contains non-printable characters; use -T to convert.");
          f_warned = 1;
        }
      }
      delete []tmpfn;
      if( !f_conv || !arg_flg_convert_filenames ){
        m_directory = new char[strlen(path) + 1];
#ifndef WINDOWS
        if( !m_directory ) return -1;
#endif
        strcpy(m_directory,path);
      }
    }

    /* now r saved the pos of files list. q saved list length */
    p = metabuf + r + 1; 
    q--;
    for(; q && 'e' != *p; p += dl, q -= dl){
      if(!(dl = decode_dict(p, q, (const char*) 0)) ) return -1;
      if( !decode_query(p, dl, "length", (const char**) 0,
                       (size_t*) 0,&t,QUERY_LONG) ) return -1;
      pbf = _new_bfnode();
#ifndef WINDOWS
      if( !pbf ) return -1;
#endif
      pbf->bf_length = t;
      m_total_files_length += t;
      r = decode_query(p, dl, "path", (const char **)0, &n, (int64_t*)0,
                       QUERY_POS);
      if( !r ) return -1;
      if(!decode_list2path(p + r, n, path)) return -1;

      int f_conv;
      char *tmpfn = new char[strlen(path)*2+5];
#ifndef WINDOWS
      if( !tmpfn ) return -1;
#endif
      if( f_conv = ConvertFilename(tmpfn, path, strlen(path)*2+5) ){
        if( arg_flg_convert_filenames ){
          pbf->bf_filename = new char[strlen(tmpfn) + 1];
#ifndef WINDOWS
          if( !pbf->bf_filename ){
            delete []tmpfn;
            return -1;
          }
#endif
          strcpy(pbf->bf_filename, tmpfn);
        }else if(!f_warned){
          CONSOLE.Warning(3,
            "Filename contains non-printable characters; use -T to convert.");
          f_warned = 1;
        }
      }
      delete []tmpfn;
      if( !f_conv || !arg_flg_convert_filenames ){
        pbf->bf_filename = new char[strlen(path) + 1];
#ifndef WINDOWS
        if( !pbf->bf_filename ) return -1;
#endif
        strcpy(pbf->bf_filename, path);
      }
      if(pbf_last) pbf_last->bf_next = pbf; else m_btfhead = pbf;
      pbf_last = pbf;
    }
  }else{
    if( !decode_query(metabuf,metabuf_len,"info|length",
                     (const char**) 0,(size_t*) 0,&t,QUERY_LONG) )
      return -1;
    m_btfhead = _new_bfnode();
#ifndef WINDOWS
    if( !m_btfhead) return -1;
#endif
    m_btfhead->bf_length = m_total_files_length = t;
    if( saveas ){
      m_btfhead->bf_filename = new char[strlen(saveas) + 1];
#ifndef WINDOWS
      if(!m_btfhead->bf_filename ) return -1;
#endif
      strcpy(m_btfhead->bf_filename, saveas);
    }else if( arg_flg_convert_filenames ){
      char *tmpfn = new char[strlen(path)*2+5];
#ifndef WINDOWS
      if( !tmpfn ) return -1;
#endif
      ConvertFilename(tmpfn, path, strlen(path)*2+5);
      m_btfhead->bf_filename = new char[strlen(tmpfn) + 1];
#ifndef WINDOWS
      if( !m_btfhead->bf_filename ){
        delete []tmpfn;
        return -1;
      }
#endif
      strcpy(m_btfhead->bf_filename, tmpfn);
      delete []tmpfn;
    }else{
      m_btfhead->bf_filename = new char[strlen(path) + 1];
#ifndef WINDOWS
      if(!m_btfhead->bf_filename ) return -1;
#endif
      strcpy(m_btfhead->bf_filename, path);
    }
  }
  return 0;
}

int btFiles::CreateFiles()
{
  int check_exist = 0;
  char fn[MAXPATHLEN];
  BTFILE *pbt = m_btfhead;
  struct stat sb;
  int i = 0;

  for(; pbt; pbt = pbt->bf_next){
    m_nfiles++;

    if( m_directory ){
      if( MAXPATHLEN <= snprintf(fn, MAXPATHLEN, "%s%c%s",
          m_directory, PATH_SP, pbt->bf_filename) )
        return -1;
    }else{
      strcpy(fn, pbt->bf_filename);
    }
    
    if(stat(fn, &sb) < 0){
      if(ENOENT == errno){
        if( arg_allocate ){
          CONSOLE.Interact_n("");
          CONSOLE.Interact_n("Creating %s", fn);
        }
        if( !_btf_creat_by_path(fn,pbt->bf_length)){
          CONSOLE.Warning(1, "error, create file \"%s\" failed.",fn);
          return -1;
        }
      }else{
        CONSOLE.Warning(1, "error, couldn't create file \"%s\":  %s", fn,
          strerror(errno));
        return -1;
      }
    }else{
      if( !check_exist) check_exist = 1;
      if( !(S_IFREG & sb.st_mode) ){
        CONSOLE.Warning(1, "error, file \"%s\" is not a regular file.", fn);
        return -1;
      }
      if(sb.st_size != pbt->bf_length){
        CONSOLE.Warning(1,"error, file \"%s\" size doesn't match; must be %llu",
                fn, (unsigned long long)(pbt->bf_length));
        return -1;
      }
    }
  } //end for

  m_file = new BTFILE *[m_nfiles];
  if( !m_file ){
    CONSOLE.Warning(1, "error, failed to allocate memory for files list");
    return -1;
  }
  for( pbt = m_btfhead; pbt; pbt = pbt->bf_next ){
    m_file[i++] = pbt;
  }
  return check_exist;
}

void btFiles::PrintOut()
{
  BTFILE *p = m_btfhead;
  size_t id = 0;
  CONSOLE.Print("");
  CONSOLE.Print("FILES INFO");
  BitField tmpBitField, tmpFilter;
  if(m_directory) CONSOLE.Print("Directory: %s", m_directory);
  for( ; p ; p = p->bf_next ){
    ++id;
    CONSOLE.Print_n("");
    CONSOLE.Print_n("<%d> %s%s [%llu]", (int)id, m_directory ? " " : "",
      p->bf_filename, (unsigned long long)(p->bf_length));
    if( !arg_flg_exam_only ){
      BTCONTENT.SetTmpFilter(id, &tmpFilter);
      tmpBitField = *BTCONTENT.pBF;
      tmpBitField.Except(tmpFilter);
      CONSOLE.Print_n(" %d/%d (%d%%)",
        (int)(tmpBitField.Count()), (int)(GetFilePieces(id)),
        GetFilePieces(id) ?
          (100 * tmpBitField.Count() / GetFilePieces(id)) : 100);
    }
  }
  CONSOLE.Print("Total: %lu MB",
    (unsigned long)(m_total_files_length/1024/1024));
}

size_t btFiles::FillMetaInfo(FILE* fp)
{
  BTFILE *p;
  if( m_directory ){
    // multi files
    if( bencode_str("files", fp) != 1 ) return 0;

    if( bencode_begin_list(fp) != 1) return 0;
    
    for( p = m_btfhead; p; p = p->bf_next){
      if( bencode_begin_dict(fp) != 1) return 0;
      
      if( bencode_str("length", fp) != 1 ) return 0;
      if( bencode_int(p->bf_length, fp) != 1) return 0;

      if( bencode_str("path", fp) != 1) return 0;
      if( bencode_path2list(p->bf_filename, fp) != 1 ) return 0;
      
      if( bencode_end_dict_list(fp) != 1) return 0;
    }
    
    if(bencode_end_dict_list(fp) != 1 ) return 0;
    
    if(bencode_str("name", fp) != 1) return 0;
    return bencode_str(m_directory, fp);
    
  }else{
    if( bencode_str("length", fp) != 1 ) return 0;
    if( bencode_int(m_btfhead->bf_length, fp) != 1) return 0;
    
    if( bencode_str("name", fp) != 1 ) return 0;
    return bencode_str(m_btfhead->bf_filename, fp);
  }
  return 1;
}


void btFiles::SetFilter(int nfile, BitField *pFilter, size_t pieceLength)
{
  BTFILE *p = m_btfhead;
  size_t id = 0;
  uint64_t sizeBuffer=0;
  size_t index;

  if( nfile==0 || nfile>m_nfiles ){
    pFilter->Clear();
    return;
  }

  pFilter->SetAll();
  for( ; p ; p = p->bf_next ){
    if(++id == nfile){
      if( 0 == p->bf_length ){
        p->bf_npieces = 0;
        return;
      }
      size_t start, stop;
      start = sizeBuffer / pieceLength;
      stop  = (sizeBuffer + p->bf_length) / pieceLength;
      // calculation is off if file ends on a piece boundary
      if(stop > start && 0 == (sizeBuffer + p->bf_length) % pieceLength)
        --stop;
      p->bf_npieces = stop - start + 1;
      for(index = start; index <= stop; index++) {
        pFilter->UnSet(index);
      }
      break;
    }
    sizeBuffer += p->bf_length;
  }
}

char *btFiles::GetFileName(size_t nfile) const
{
  if( nfile && nfile <= m_nfiles )
    return m_file[nfile-1]->bf_filename;
  return (char *)0;
}

uint64_t btFiles::GetFileSize(size_t nfile) const
{
  if( nfile && nfile <= m_nfiles )
    return m_file[nfile-1]->bf_length;
  return 0;
}

size_t btFiles::GetFilePieces(size_t nfile) const
{
  //returns the number of pieces in the file
  if( nfile && nfile <= m_nfiles )
    return m_file[nfile-1]->bf_npieces;
  return 0;
}

int btFiles::ConvertFilename(char *dst, const char *src, int size)
{
  int retval=0, i, j, f_print=0, f_punct=0;

  for(i=j=0; src[i] != '\0' && j < size-2; i++){
    if( isprint(src[i]) ){
      if( ispunct(src[i]) ) f_punct = 1;
      else f_punct = 0;
      if(j && !f_print && !f_punct){ sprintf(dst+j, "_"); j++; }
      dst[j++] = src[i];
      f_print = 1;
    }else{
      if(f_print && !f_punct){ sprintf(dst+j, "_"); j++; }
      snprintf(dst+j, 3, "%.2X", (unsigned char)(src[i]));
      j += 2;
      f_print = f_punct = 0;
      if( !retval ) retval = 1;
    }
  }
  dst[j] = '\0';
  return retval;
}

char *btFiles::GetDataName() const
{
  return m_directory ? m_directory : m_btfhead->bf_filename;
}

