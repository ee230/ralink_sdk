#include "ctcs.h"  // def.h

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <ctype.h>

#include "btcontent.h"
#include "setnonblock.h"
#include "connect_nonb.h"
#include "tracker.h"
#include "peerlist.h"
#include "peer.h"
#include "btconfig.h"
#include "bttime.h"
#include "console.h"

#ifndef HAVE_SNPRINTF
#include "compat.h"
#endif


#define CTCS_PROTOCOL 3

#define compset(a,member)  ( (a.member==member)? 0 : ((a.member = member)||1) )

Ctcs CTCS;


Ctcs::Ctcs()
{
  memset(m_host,0,MAXHOSTNAMELEN);

  m_sock = INVALID_SOCKET;
  m_port = 2780;
  m_status = T_FREE;
  m_interval = 5;
  m_protocol = CTCS_PROTOCOL;

  m_last_timestamp = m_sent_ctstatus_time = m_statustime = (time_t) 0;
  m_sent_ctstatus = 0;
  m_sent_ctbw = 0;
}


Ctcs::~Ctcs()
{
  if( m_sock != INVALID_SOCKET) CLOSE_SOCKET(m_sock);
}


void Ctcs::Reset(time_t new_interval)
{
  if(new_interval) m_interval = new_interval;

  if( INVALID_SOCKET != m_sock ){
    if(T_READY==m_status) CONSOLE.Warning(2, "Connection to CTCS closed");
    CLOSE_SOCKET(m_sock);
    m_sock = INVALID_SOCKET;
  }

  in_buffer.Reset();
  out_buffer.Reset();
  m_last_timestamp = now;
  m_sent_ctstatus = 0;
  m_sent_ctbw = 0;
  m_status = T_FREE;
}


// borrowed from tracker.cpp (with changes)
int Ctcs:: _s2sin(char *h,int p,struct sockaddr_in *psin)
{
  psin->sin_family = AF_INET;
  psin->sin_port = htons(p);
  psin->sin_addr.s_addr = inet_addr(h);
  if(psin->sin_addr.s_addr == INADDR_NONE){
    struct hostent *ph = gethostbyname(h);
    if( !ph  || ph->h_addrtype != AF_INET){
      memset(psin,0,sizeof(struct sockaddr_in));
      return -1;
    }
    memcpy(&psin->sin_addr,ph->h_addr_list[0],sizeof(struct in_addr));
  }
  return ( psin->sin_addr.s_addr == INADDR_NONE ) ? -1 : 0;
}


int Ctcs::CheckMessage()
{
  ssize_t r;
  size_t q;

  r = in_buffer.FeedIn(m_sock);

  // This differs from tracker.cpp since we maintain a persistent connection.
  if( r == 0 ) return 0;	// no data (should return an error)

  q = in_buffer.Count();

  if( !q ){
    int error = 0;
    socklen_t n = sizeof(error);
    if( getsockopt(m_sock, SOL_SOCKET,SO_ERROR,&error,&n) < 0 )
      error = errno;
    if( error != 0 ) CONSOLE.Warning(2,
      "warn, received nothing from CTCS:  %s", strerror(error));
    Reset(5);
    return -1;
  }

  char *s, *msgbuf;
  while(in_buffer.Count() &&
        (s=strpbrk(msgbuf=in_buffer.BasePointer(), "\r\n"))){
    *s = '\0';
    if(arg_verbose && s!=msgbuf) CONSOLE.Debug("CTCS: %s", msgbuf);
    if( !strncmp("SETDLIMIT",msgbuf,9) ){
      cfg_max_bandwidth_down = (int)(strtod(msgbuf+10, NULL));
      if(arg_verbose) CONSOLE.Debug("DLimit=%d", cfg_max_bandwidth_down);
    }else if( !strncmp("SETULIMIT",msgbuf,9) ){
      cfg_max_bandwidth_up = (int)(strtod(msgbuf+10, NULL));
      if(arg_verbose) CONSOLE.Debug("ULimit=%d", cfg_max_bandwidth_up);
    }else if( !strncmp("SENDPEERS",msgbuf,9) ){
      Send_Peers();
    }else if( !strncmp("SENDSTATUS",msgbuf,10) ){
      Send_Status();
    }else if( !strncmp("SENDCONF",msgbuf,8) ){
      Send_Config();
    }else if( !strncmp("CTCONFIG",msgbuf,8) ){
      Set_Config(msgbuf);
    }else if( !strncmp("SENDDETAIL",msgbuf,10) ){
      Send_Detail();
    }else if( !strncmp("CTQUIT",msgbuf,6) ){
      CONSOLE.Print("CTCS sent Quit command");
      Tracker.ClearRestart();
      Tracker.SetStoped();
    }else if( !strncmp("CTRESTART",msgbuf,9) ){
      RestartTracker();
    }else if( !strncmp("CTUPDATE",msgbuf,8) ){
      Tracker.Reset(15);
    }else if( !strncmp("PROTOCOL",msgbuf,8) ){
      int proto = atoi(msgbuf+9);
      if( proto <= CTCS_PROTOCOL ) m_protocol = proto;
      else m_protocol = CTCS_PROTOCOL;
    }else if( s!=msgbuf ){
      if(arg_verbose) CONSOLE.Debug("unknown CTCS message: %s", msgbuf);
    }
    in_buffer.PickUp(s-msgbuf + 1);
  }
  m_last_timestamp = now;
  return 0;
}


int Ctcs::SendMessage(const char *message)
{
  int len, r=0;
  char buf[CTCS_BUFSIZE];

  if( m_status == T_READY ){
    len = strlen(message);
    strncpy(buf, message, len);
    if( len+1 < CTCS_BUFSIZE ){
      buf[len] = '\n';
      buf[len+1] = '\0';
    }else{
      buf[CTCS_BUFSIZE-2] = '\n';
      buf[CTCS_BUFSIZE-1] = '\0';
    }
    r = out_buffer.PutFlush(m_sock, buf, len+1);
    if( r<0 ) Reset(5);
    else m_last_timestamp = now;
  }
  return r;
}


int Ctcs::Send_Auth()
{
  char message[CTCS_BUFSIZE];

  if(!*m_pass) return 0;
  snprintf(message, CTCS_BUFSIZE, "AUTH %s", m_pass);
  return SendMessage(message);
}


int Ctcs::Send_Protocol()
{
  char message[CTCS_BUFSIZE];

  snprintf(message, CTCS_BUFSIZE, "PROTOCOL %04d", CTCS_PROTOCOL);
  return SendMessage(message);
}


int Ctcs::Send_Torrent(const unsigned char *peerid, char *torrent)
{
  char message[CTCS_BUFSIZE];
  char txtid[PEER_ID_LEN*2+3];

  TextPeerID(peerid, txtid);

  snprintf(message, CTCS_BUFSIZE, "CTORRENT %s %ld %ld %s", txtid,
    (long)(BTCONTENT.GetStartTime()), (long)now, torrent);
  return SendMessage(message);
}


int Ctcs::Report_Status(size_t seeders, size_t leechers, size_t nhave,
  size_t ntotal, size_t dlrate, size_t ulrate,
  uint64_t dltotal, uint64_t ultotal, size_t dlimit, size_t ulimit,
  size_t cacheused)
{
  int changebw=0,change=0;
  int r;
  size_t nhad;

  if( T_READY != m_status ) return 0;

  nhad = m_ctstatus.nhave;

  changebw = (
    compset(m_ctstatus, dlrate) |
    compset(m_ctstatus, ulrate) |
    compset(m_ctstatus, dlimit) |
    compset(m_ctstatus, ulimit) );
  change = ( changebw             |
    compset(m_ctstatus, seeders)  |
    compset(m_ctstatus, leechers) |
    compset(m_ctstatus, nhave)    |
    compset(m_ctstatus, ntotal)   |
    compset(m_ctstatus, dltotal)  |
    compset(m_ctstatus, ultotal)  |
    compset(m_ctstatus, cacheused) );

  if( ( !m_sent_ctstatus || (nhad<nhave && nhave==ntotal) ||
        (Tracker.GetStatus() && now > m_sent_ctstatus_time+30) ) &&
      (r=Send_Status()) != 0 ) return r;
  else return (changebw || !m_sent_ctbw) ? Send_bw() : 0;
}


int Ctcs::Send_Status()
{
  char message[CTCS_BUFSIZE];

  if( m_sent_ctstatus_time + 1 > now ) {
    m_sent_ctstatus = 0;
    return 0;
  }
  if( m_protocol == 1 )
    snprintf( message, CTCS_BUFSIZE,
      "CTSTATUS %d/%d %d/%d/%d %d,%d %llu,%llu %d,%d",
      (int)(m_ctstatus.seeders), (int)(m_ctstatus.leechers),
      (int)(m_ctstatus.nhave), (int)(m_ctstatus.ntotal),
        (int)(WORLD.Pieces_I_Can_Get()),
      (int)(m_ctstatus.dlrate), (int)(m_ctstatus.ulrate),
      (unsigned long long)(m_ctstatus.dltotal),
        (unsigned long long)(m_ctstatus.ultotal),
      (int)(m_ctstatus.dlimit), (int)(m_ctstatus.ulimit) );
  else
    snprintf( message, CTCS_BUFSIZE,
      "CTSTATUS %d:%d/%d:%d/%d %d/%d/%d %d,%d %llu,%llu %d,%d %d",
      (int)(m_ctstatus.seeders),
        (int)(Tracker.GetSeedsCount()) - (BTCONTENT.IsFull() ? 1 : 0),
      (int)(m_ctstatus.leechers),
        (int)(Tracker.GetPeersCount()) - Tracker.GetSeedsCount() -
          (!BTCONTENT.IsFull() ? 1 : 0),
      (int)(WORLD.GetConnCount()),
      (int)(m_ctstatus.nhave), (int)(m_ctstatus.ntotal),
        (int)(WORLD.Pieces_I_Can_Get()),
      (int)(m_ctstatus.dlrate), (int)(m_ctstatus.ulrate),
      (unsigned long long)(m_ctstatus.dltotal),
        (unsigned long long)(m_ctstatus.ultotal),
      (int)(m_ctstatus.dlimit), (int)(m_ctstatus.ulimit),
      (int)(m_ctstatus.cacheused) );
  m_sent_ctstatus = 1;
  m_sent_ctstatus_time = now;
  return SendMessage(message);
}


int Ctcs::Send_bw()
{
  char message[CTCS_BUFSIZE];

  snprintf(message, CTCS_BUFSIZE, "CTBW %d,%d %d,%d",
    (int)(m_ctstatus.dlrate), (int)(m_ctstatus.ulrate),
    (int)(m_ctstatus.dlimit), (int)(m_ctstatus.ulimit) );
  m_sent_ctbw = 1;
  return SendMessage(message);
}


int Ctcs::Send_Config()
{
  char message[CTCS_BUFSIZE];

  if( m_protocol >= 3 ){
    int r = 0;
    char value[MAXPATHLEN], desc[MAXPATHLEN], maxlen[10];

    if( (r=SendMessage("CTCONFIGSTART")) < 0 ) return r;
    snprintf(maxlen, sizeof(maxlen), "%u", (unsigned int)MAXPATHLEN);

    if( (r = SendMessage(ConfigMsg("verbose", "B", "0", arg_verbose?"1":"0",
        "Verbose output [-v]", arg_verbose ? "Enabled" : "Disabled"))) < 0 )
      return r;

    double num = BTCONTENT.GetSeedTime() ?
        (cfg_seed_hours - (now - BTCONTENT.GetSeedTime())/3600.0) :
        cfg_seed_hours;
    unsigned long tmp = (unsigned long)(num * 100);
    int pre = (tmp % 10) ? 2 : (tmp % 100) ? 1 : 0;
    snprintf(value, MAXPATHLEN, "%.*f", pre, num);
    snprintf(desc, MAXPATHLEN, "~hours remaining (-e %lu)", 
      (unsigned long)cfg_seed_hours);
    if( (r = SendMessage(ConfigMsg("seed_time", "F", "0", value,
                                   "Seed time [-e]", desc))) < 0 )
      return r;

    snprintf(value, MAXPATHLEN, "%.2f", (double)cfg_seed_ratio);
    if( (r = SendMessage(ConfigMsg("seed_ratio", "F", "0", value,
        "Seed ratio [-E]", "Upload:Download"))) < 0 )
      return r;

    snprintf(value, MAXPATHLEN, "%d", (int)cfg_max_peers);
    snprintf(desc, MAXPATHLEN, "Current peers: %d",
      (int)(WORLD.GetPeersCount()));
    if( (r = SendMessage(ConfigMsg("max_peers", "I", "20-1000", value,
        "Max peers [-M]", desc))) < 0 )
      return r;

    snprintf(value, MAXPATHLEN, "%d", (int)cfg_min_peers);
    snprintf(desc, MAXPATHLEN, "Current peers: %d",
      (int)(WORLD.GetPeersCount()));
    if( (r = SendMessage(ConfigMsg("min_peers", "I", "1-1000", value,
        "Min peers [-m]", desc))) < 0 )
      return r;

    if( (r = SendMessage(ConfigMsg("file_list", "S", maxlen,
        arg_file_to_download ? arg_file_to_download : "",
        "Download files [-n]", ""))) < 0 )
      return r;

    snprintf(value, MAXPATHLEN, "%d", (int)cfg_cache_size);
    snprintf(desc, MAXPATHLEN, "MB; %dKB now in use",
      (int)(BTCONTENT.CacheUsed()/1024));
    if( (r = SendMessage(ConfigMsg("cache", "I", "0", value,
        "Cache size [-C]", desc))) < 0 )
      return r;

    if( (r = SendMessage(ConfigMsg("pause", "B", "0",
        WORLD.IsPaused()?"1":"0",
        "Pause torrent", "Stop upload/download"))) < 0 )
      return r;

    if( (r = SendMessage(ConfigMsg("user_exit", "S", maxlen,
        arg_completion_exit ? arg_completion_exit : "",
        "Completion command [-X]", ""))) < 0 )
      return r;

    if( (r = SendMessage(ConfigMsg("out_normal", "S", maxlen,
        CONSOLE.GetChannel(O_NORMAL), "Normal/status output", ""))) < 0 )
      return r;
    if( (r = SendMessage(ConfigMsg("out_interact", "S", maxlen,
        CONSOLE.GetChannel(O_INTERACT), "Interactive output", ""))) < 0 )
      return r;
    if( (r = SendMessage(ConfigMsg("out_error", "S", maxlen,
        CONSOLE.GetChannel(O_WARNING), "Error/warning output", ""))) < 0 )
      return r;
    if( (r = SendMessage(ConfigMsg("out_debug", "S", maxlen,
        CONSOLE.GetChannel(O_DEBUG), "Debug/verbose output", ""))) < 0 )
      return r;
    if( (r = SendMessage(ConfigMsg("input", "S", maxlen,
        CONSOLE.GetChannel(O_INPUT), "Console input", ""))) < 0 )
      return r;

    sprintf(message, "CTCONFIGDONE");
  }
  else if( m_protocol == 2 )
    snprintf(message, CTCS_BUFSIZE, "CTCONFIG %d %d %f %d %d %d %d %d",
      (int)arg_verbose, (int)cfg_seed_hours, cfg_seed_ratio,
      (int)cfg_max_peers, (int)cfg_min_peers,
      BTCONTENT.GetFilter() ? atoi(BTCONTENT.GetFilterName()) : 0,
      (int)cfg_cache_size, WORLD.IsPaused());
  else  // m_protocol == 1
    snprintf(message, CTCS_BUFSIZE, "CTCONFIG %d %d %f %d %d %d %d %d %d",
      (int)arg_verbose, (int)cfg_seed_hours, cfg_seed_ratio,
      (int)cfg_max_peers, (int)cfg_min_peers,
      BTCONTENT.GetFilter() ? atoi(BTCONTENT.GetFilterName()) : 0,
      0, WORLD.IsPaused(), 0);

  return SendMessage(message);
}

char *Ctcs::ConfigMsg(const char *name, const char *type, const char *range,
  const char *value, const char *short_desc, const char *long_desc)
{
  static char *message = (char *)0;

  if( !message ){
    message = new char[CTCS_BUFSIZE];
    if( !message ){
      CONSOLE.Warning(1, "error, failed to allocate memory for CTCS message");
      return (char *)0;
    }
  }
  snprintf(message, CTCS_BUFSIZE, "CTCONFIG %s %s %s %d:%s %d:%s %d:%s",
    name, type, range, (int)strlen(value), value,
    (int)strlen(short_desc), short_desc, (int)strlen(long_desc), long_desc);

  return message;
}

int Ctcs::Set_Config(char *msgbuf)
{
  if( m_protocol >= 3 ){
    char *name, *valstr;
    name = strtok(strchr(msgbuf, ' '), " ");
    for( valstr=name+strlen(name)+1; *valstr==' '; valstr++ );

    if( 0==strcmp(name, "verbose") ){
      int arg = atoi(valstr);
      if( arg_verbose && !arg ) CONSOLE.Debug("Verbose output off");
      arg_verbose = arg;
    }else if( 0==strcmp(name, "seed_time") ){
      double value = strtod(valstr, NULL);
      time_t arg = (time_t)value + ((value - (int)value) ? 1 : 0);
      arg += BTCONTENT.GetSeedTime() ?
             ((now - BTCONTENT.GetSeedTime()) / 3600) : 0;
      if( arg > 0 || 0==BTCONTENT.GetSeedTime() ||
          cfg_seed_ratio > (double) Self.TotalUL() /
            (Self.TotalDL() ?
             Self.TotalDL() : BTCONTENT.GetTotalFilesLength()) )
        cfg_seed_hours = arg;
    }else if( 0==strcmp(name, "seed_ratio") ){
      double arg = atof(valstr);
      if( 0==BTCONTENT.GetSeedTime() ||
          cfg_seed_hours > (now - BTCONTENT.GetSeedTime()) / 3600 ||
          arg > (double) Self.TotalUL() /
            (Self.TotalDL() ?
             Self.TotalDL() : BTCONTENT.GetTotalFilesLength()) )
        cfg_seed_ratio = arg;
    }else if( 0==strcmp(name, "max_peers") ){
      cfg_max_peers = atoi(valstr);
    }else if( 0==strcmp(name, "min_peers") ){
      cfg_min_peers = atoi(valstr);
    }else if( 0==strcmp(name, "file_list") ){
      if( arg_file_to_download ) delete []arg_file_to_download;
      if( 0==strlen(valstr) ) arg_file_to_download = (char *)0;
      else{
        arg_file_to_download = new char[strlen(valstr) + 1];
        if( !arg_file_to_download )
          CONSOLE.Warning(1, "error, failed to allocate memory for option");
        else strcpy(arg_file_to_download, valstr);
      }
      BTCONTENT.SetFilter();
    }else if( 0==strcmp(name, "cache") ){
      cfg_cache_size = atoi(valstr);
      BTCONTENT.CacheConfigure();
    }else if( 0==strcmp(name, "pause") ){
      if( atoi(valstr) ){
        if( !WORLD.IsPaused() ) WORLD.Pause();
      }else if( WORLD.IsPaused() ) WORLD.Resume();
    }else if( 0==strcmp(name, "user_exit") ){
      if( arg_completion_exit ) delete []arg_completion_exit;
      arg_completion_exit = new char[strlen(valstr) + 1];
      if( !arg_completion_exit )
        CONSOLE.Warning(1, "error, failed to allocate memory for option");
      else strcpy(arg_completion_exit, valstr);
    }else if( 0==strcmp(name, "out_normal") ){
      CONSOLE.ChangeChannel(O_NORMAL, valstr);
    }else if( 0==strcmp(name, "out_interact") ){
      CONSOLE.ChangeChannel(O_INTERACT, valstr);
    }else if( 0==strcmp(name, "out_error") ){
      CONSOLE.ChangeChannel(O_WARNING, valstr);
    }else if( 0==strcmp(name, "out_debug") ){
      CONSOLE.ChangeChannel(O_DEBUG, valstr);
    }else if( 0==strcmp(name, "input") ){
      CONSOLE.ChangeChannel(O_INPUT, valstr);
    }else CONSOLE.Warning(2, "Unknown config option %s from CTCS", name);
  }else{  // m_protocol <= 2
    if(msgbuf[9] != '.'){
      int arg = atoi(msgbuf+9);
      if( arg_verbose && !arg ) CONSOLE.Debug("Verbose output off");
      arg_verbose = arg;
    }
    if(msgbuf[11] != '.') cfg_seed_hours = atoi(msgbuf+11);
    msgbuf = strchr(msgbuf+11, ' ') + 1;
    if(msgbuf[0] != '.') cfg_seed_ratio = atof(msgbuf);
    msgbuf = strchr(msgbuf, ' ') + 1;
    if(msgbuf[0] != '.') cfg_max_peers = atoi(msgbuf);
    msgbuf = strchr(msgbuf, ' ') + 1;
    if(msgbuf[0] != '.') cfg_min_peers = atoi(msgbuf);
    msgbuf = strchr(msgbuf, ' ') + 1;
    if(msgbuf[0] != '.'){
      char *p = strchr(msgbuf, ' ');
      if( arg_file_to_download ) delete []arg_file_to_download;
      arg_file_to_download = new char[p - msgbuf + 2 + 1];
      if( !arg_file_to_download )
        CONSOLE.Warning(1, "error, failed to allocate memory for option");
      else{
        strncpy(arg_file_to_download, msgbuf, p - msgbuf);
        arg_file_to_download[p - msgbuf] = '\0';
        strcat(arg_file_to_download, ",*");  // mock old behavior
      }
      BTCONTENT.SetFilter();
    }
    if( m_protocol >= 2 ){
      msgbuf = strchr(msgbuf, ' ') + 1;
      if(msgbuf[0] != '.'){
        cfg_cache_size = atoi(msgbuf);
        BTCONTENT.CacheConfigure();
      }
    }
    if( m_protocol == 1 ){
      msgbuf = strchr(msgbuf, ' ') + 1;
      // old cfg_exit_zero_peers option
    }
    msgbuf = strchr(msgbuf, ' ') + 1;
    if(msgbuf[0] != '.'){
      if(atoi(msgbuf)){
        if( !WORLD.IsPaused() ) WORLD.Pause();
      }else if( WORLD.IsPaused() ) WORLD.Resume();
    }
  }

  return 0;
}


int Ctcs::Send_Detail()
{
  char message[CTCS_BUFSIZE];
  int r=0, priority, current=0;
  size_t n=0;
  BTFILE *file=0;
  BitField tmpBitField, fileFilter, availbf, tmpavail, allFilter, tmpFilter,
    *pfilter;

  snprintf( message, CTCS_BUFSIZE, "CTDETAIL %lld %d %ld %ld",
    BTCONTENT.GetTotalFilesLength(),
    (int)(BTCONTENT.GetPieceLength()), (long)now,
    (long)(BTCONTENT.GetSeedTime()) );
  r = SendMessage(message);

  if(r==0) r = SendMessage((m_protocol >= 3) ? "CTFILESTART" : "CTFILES");

  if( m_protocol >= 3 ){  // determine current download priority
    pfilter = (BitField *)0;
    while( pfilter != BTCONTENT.GetFilter() ){
      current++;
      pfilter = BTCONTENT.GetNextFilter(pfilter);
    }
  }

  WORLD.Pieces_I_Can_Get(&availbf);

  while( r==0 && ++n <= BTCONTENT.GetNFiles() ){
    tmpBitField = *BTCONTENT.pBF;
    BTCONTENT.SetTmpFilter(n, &fileFilter);
    tmpBitField.Except(fileFilter);  // the pieces of this file that I have
    tmpavail = availbf;
    tmpavail.Except(fileFilter);     // the available pieces of this file

    if( m_protocol >= 3 ){
      priority = 0;
      if( BTCONTENT.GetFilter() ){
        fileFilter.Invert();
        allFilter.SetAll();
        pfilter = (BitField *)0;
        while( pfilter = BTCONTENT.GetNextFilter(pfilter) ){
          priority++;
          allFilter.And(*pfilter);    // cumulation of filters
          tmpFilter = allFilter;
          tmpFilter.Invert();         // what's included by the filters...
          tmpFilter.And(fileFilter);  // ...that's also in this file
          if( tmpFilter.Count() >= fileFilter.Count() ) break;
        }
        if( !pfilter ) priority = 0;
      }
      snprintf( message, CTCS_BUFSIZE, "CTFILE %d %d %d %d %d %d %llu %s",
        (int)n, priority, current, (int)(BTCONTENT.GetFilePieces(n)),
        (int)(tmpBitField.Count()), (int)(tmpavail.Count()),
        (unsigned long long)(BTCONTENT.GetFileSize(n)),
        BTCONTENT.GetFileName(n) );
    }
    else if( m_protocol == 2 )
      snprintf( message, CTCS_BUFSIZE, "CTFILE %d %d %d %d %llu %s",
        (int)n, (int)(BTCONTENT.GetFilePieces(n)),
        (int)(tmpBitField.Count()), (int)(tmpavail.Count()),
        (unsigned long long)(BTCONTENT.GetFileSize(n)),
        BTCONTENT.GetFileName(n) );
    else  // m_protocol == 1
      snprintf( message, CTCS_BUFSIZE, "CTFILE %d %d %d %llu %s",
        (int)n, (int)(BTCONTENT.GetFilePieces(n)),
        (int)(tmpBitField.Count()),
        (unsigned long long)(BTCONTENT.GetFileSize(n)),
        BTCONTENT.GetFileName(n) );

    r = SendMessage(message);
  }
  if(r==0) r = SendMessage((m_protocol >= 3) ? "CTFILESDONE" : "CTFDONE");
  return r;
}


int Ctcs::Send_Peers()
{
  btPeer *peer=0;
  char message[CTCS_BUFSIZE];
  char txtid[PEER_ID_LEN*2+3];
  struct sockaddr_in psin;
  int r=0;

  r=SendMessage((m_protocol >= 3) ? "CTPEERSTART" : "CTPEERS");
  while( r==0 && (peer = WORLD.GetNextPeer(peer)) ){
    TextPeerID(peer->id, txtid);
     peer->GetAddress(&psin);

     snprintf(message, CTCS_BUFSIZE, "CTPEER %s %s %c%c%c%c %d %d %llu %llu %d",
       txtid, inet_ntoa(psin.sin_addr),
       peer->Is_Remote_UnChoked() ? 'U' : 'C',
       peer->Is_Local_Interested() ? 'i' : 'n',
       peer->Is_Local_UnChoked() ? 'U' : 'C',
       peer->Is_Remote_Interested() ? 'i' : 'n',
       (int)(peer->RateDL()), (int)(peer->RateUL()),
       (unsigned long long)(peer->TotalDL()),
         (unsigned long long)(peer->TotalUL()),
       (int)(peer->bitfield.Count()) );
     r = SendMessage(message);
  }
  if(r==0) r = SendMessage((m_protocol >= 3) ? "CTPEERSDONE" : "CTPDONE");
  return r;
}


int Ctcs::Send_Info(int sev, const char *info)
{
  char message[CTCS_BUFSIZE];

  snprintf(message, CTCS_BUFSIZE, "CTINFO %d %s", sev, info);
  return SendMessage(message);
}


int Ctcs::Initial()
{
  char *s;

  strncpy(m_host, arg_ctcs, MAXHOSTNAMELEN-1);
  m_host[MAXHOSTNAMELEN-1] = '\0';
  if( s = strchr(m_host, ':') ) *s='\0';
  m_port = atoi(s=(strchr(arg_ctcs, ':')+1));
  if(strchr(s, ':')){
    CONSOLE.Input("Enter CTCS password: ", m_pass, CTCS_PASS_SIZE);
  } else *m_pass = '\0';

  return 0;
}


int Ctcs::Connect()
{
  ssize_t r;
  m_last_timestamp = now;

  if(_s2sin(m_host,m_port,&m_sin) < 0) {
    CONSOLE.Warning(2, "warn, get CTCS ip address failed.");
    return -1;
  }

  m_sock = socket(AF_INET,SOCK_STREAM,0);
  if( INVALID_SOCKET == m_sock ) return -1;

  if( setfd_nonblock(m_sock) < 0 ){ CLOSE_SOCKET(m_sock); return -1; }

  r = connect_nonb(m_sock,(struct sockaddr*)&m_sin);

  if( r == -1 ){ CLOSE_SOCKET(m_sock); return -1; }
  else if( r == -2 ) m_status = T_CONNECTING;
  else{
    m_status = T_READY;
    if(arg_verbose) CONSOLE.Debug("Connected to CTCS");
    if( Send_Protocol() != 0 && errno != EINPROGRESS ){
      CONSOLE.Warning(2, "warn, send protocol to CTCS failed:  %s",
        strerror(errno));
      return -1;
    }
    if( Send_Auth() != 0 && errno != EINPROGRESS ) {
      CONSOLE.Warning(2, "warn, send password to CTCS failed:  %s",
        strerror(errno));
      return -1;
    }
    if( Send_Torrent(BTCONTENT.GetPeerId(), arg_metainfo_file) != 0 &&
        errno != EINPROGRESS ){
      CONSOLE.Warning(2, "warn, send torrent to CTCS failed:  %s",
        strerror(errno));
      return -1;
    }
  }
  return 0;
}


int Ctcs::IntervalCheck(fd_set *rfdp, fd_set *wfdp)
{
  if( T_FREE == m_status ){
    if( INVALID_SOCKET != m_sock ){
      FD_CLR(m_sock, rfdp);
      FD_CLR(m_sock, wfdp);
    }
    if( now - m_last_timestamp >= m_interval ){
      if(Connect() < 0){ Reset(15); return -1; }

      FD_SET(m_sock, rfdp);
      if( m_status == T_CONNECTING ) FD_SET(m_sock, wfdp);
    }else if( now < m_last_timestamp ) m_last_timestamp = now;
  }else{
    if( m_status == T_CONNECTING ){
      FD_SET(m_sock, rfdp);
      FD_SET(m_sock, wfdp);
    }else if( INVALID_SOCKET != m_sock ){
      if( now > m_statustime ){
        Report_Status(
          WORLD.GetSeedsCount(),
          WORLD.GetPeersCount() - WORLD.GetSeedsCount() - WORLD.GetConnCount(),
          BTCONTENT.pBF->Count(), BTCONTENT.GetNPieces(),
          Self.RateDL(), Self.RateUL(),
          Self.TotalDL(), Self.TotalUL(),
          cfg_max_bandwidth_down, cfg_max_bandwidth_up,
          BTCONTENT.CacheUsed()/1024 );
        m_statustime = now;
      }
      FD_SET(m_sock, rfdp);
      if( out_buffer.Count() ) FD_SET(m_sock, wfdp);
    }
  }
  return m_sock;
}


int Ctcs::SocketReady(fd_set *rfdp, fd_set *wfdp, int *nfds,
  fd_set *rfdnextp, fd_set *wfdnextp)
{
  if( T_FREE == m_status ) return 0;

  if( T_CONNECTING == m_status && FD_ISSET(m_sock,wfdp) ){
    int error = 0;
    socklen_t n = sizeof(error);
    (*nfds)--;
    FD_CLR(m_sock, wfdnextp);
    if( FD_ISSET(m_sock, rfdp) ){
      (*nfds)--;
      FD_CLR(m_sock, rfdnextp);
    }
    if(getsockopt(m_sock, SOL_SOCKET,SO_ERROR,&error,&n) < 0)
      error = errno;
    if( error != 0 ){
      if( ECONNREFUSED != error )
        CONSOLE.Warning(2, "warn, connect to CTCS failed:  %s",
          strerror(error));
      Reset(15);
      return -1;
    }else{
      m_status = T_READY; 
      if(arg_verbose) CONSOLE.Debug("Connected to CTCS");
      if( Send_Protocol() != 0 && errno != EINPROGRESS ){
        CONSOLE.Warning(2, "warn, send protocol to CTCS failed:  %s",
          strerror(errno));
        return -1;
      }
      if( Send_Auth() != 0 && errno != EINPROGRESS ) {
        CONSOLE.Warning(2, "warn, send password to CTCS failed:  %s",
          strerror(errno));
        return -1;
      }
      if( Send_Torrent(BTCONTENT.GetPeerId(), arg_metainfo_file) == 0
          && errno != EINPROGRESS ){
        CONSOLE.Warning(2, "warn, send torrent to CTCS failed:  %s",
          strerror(errno));
        return -1;
      }
    }
  }else if( T_CONNECTING == m_status && FD_ISSET(m_sock,rfdp) ){
    int error = 0;
    socklen_t n = sizeof(error);
    (*nfds)--;
    FD_CLR(m_sock, rfdnextp);
    if(getsockopt(m_sock, SOL_SOCKET,SO_ERROR,&error,&n) < 0)
      error = errno;
    CONSOLE.Warning(2, "warn, connect to CTCS failed:  %s", strerror(error));
    Reset(15);
    return -1;
  }else if( INVALID_SOCKET != m_sock ){
    if( FD_ISSET(m_sock, rfdp) ){
      (*nfds)--;
      FD_CLR(m_sock,rfdnextp);
      SOCKET tmp_sock = m_sock;
      int r = CheckMessage();
      if( INVALID_SOCKET == m_sock ){
        if( FD_ISSET(tmp_sock, wfdp) ){
          (*nfds)--;
          FD_CLR(tmp_sock,wfdnextp);
        }
        return r;
      }
    }
    if( FD_ISSET(m_sock, wfdp) ){
      (*nfds)--;
      FD_CLR(m_sock,wfdnextp);
      if( out_buffer.Count() && out_buffer.FlushOut(m_sock) < 0 ){
        Reset(5);
        return -1;
      }
    }
  }else{  // failsafe
    Reset(5);
    return -1;
  }
  return 0;
}


void Ctcs::RestartTracker()
{
  Tracker.SetStoped(); // finish the tracker
  // Now we need to wait until the tracker updates (T_FINISHED == m_status),
  // then Tracker.Restart().
  Tracker.SetRestart();
}

