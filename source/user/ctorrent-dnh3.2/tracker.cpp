#include "tracker.h"

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "peerlist.h"
#include "peer.h"
#include "httpencode.h"
#include "bencode.h"
#include "setnonblock.h"
#include "connect_nonb.h"
#include "btcontent.h"
#include "iplist.h"

#include "btconfig.h"
#include "ctcs.h"
#include "console.h"
#include "bttime.h"

#ifndef HAVE_SNPRINTF
#include "compat.h"
#endif

btTracker Tracker;

btTracker::btTracker()
{
  memset(m_host,0,MAXHOSTNAMELEN);
  memset(m_path,0,MAXPATHLEN);
  memset(m_trackerid,0,PEER_ID_LEN+1);

  m_sock = INVALID_SOCKET;
  m_port = 80;
  m_status = T_FREE;
  m_f_started = m_f_stoped = m_f_completed = m_f_restart = 0;

  m_interval = 15;
  m_peers_count = m_seeds_count = 0;

  m_connect_refuse_click = 0;
  m_last_timestamp = (time_t) 0;
  m_prevpeers = 0;
}

btTracker::~btTracker()
{
  if( m_sock != INVALID_SOCKET) CLOSE_SOCKET(m_sock);
}

void btTracker::Reset(time_t new_interval)
{
  if(new_interval) m_interval = new_interval;

  if( INVALID_SOCKET != m_sock ){
    if(arg_verbose && T_READY==m_status)
      CONSOLE.Debug("Disconnected from tracker");
    CLOSE_SOCKET(m_sock);
    m_sock = INVALID_SOCKET;
  }

  m_request_buffer.Reset();
  m_reponse_buffer.Reset();
  if( now < m_last_timestamp ) m_last_timestamp = now;  // time reversed

  if( m_f_stoped ){
    m_status = T_FINISHED;
    if( m_f_restart ) Restart();
  }
  else m_status = T_FREE;
}

int btTracker:: _IPsin(char *h, int p, struct sockaddr_in *psin)
{
  psin->sin_family = AF_INET;
  psin->sin_port = htons(p);
  psin->sin_addr.s_addr = inet_addr(h);
  return ( psin->sin_addr.s_addr == INADDR_NONE ) ? -1 : 0;
}

int btTracker:: _s2sin(char *h,int p,struct sockaddr_in *psin)
{
  psin->sin_family = AF_INET;
  psin->sin_port = htons(p);
  if( h ){
    psin->sin_addr.s_addr = inet_addr(h);
    if(psin->sin_addr.s_addr == INADDR_NONE){
      struct hostent *ph = gethostbyname(h);
      if( !ph  || ph->h_addrtype != AF_INET){
        memset(psin,0,sizeof(struct sockaddr_in));
        return -1;
      }
      memcpy(&psin->sin_addr,ph->h_addr_list[0],sizeof(struct in_addr));
    }
  }else 
    psin->sin_addr.s_addr = htonl(INADDR_ANY);
  return 0;
}

int btTracker::_UpdatePeerList(char *buf,size_t bufsiz)
{
  char tmphost[MAXHOSTNAMELEN];
  const char *ps;
  size_t i,pos,tmpport;
  size_t cnt = 0;

  struct sockaddr_in addr;

  if( decode_query(buf,bufsiz,"failure reason",&ps,&i,(int64_t*)0,QUERY_STR) ){
    char failreason[1024];
    if( i < 1024 ){
      memcpy(failreason, ps, i);
      failreason[i] = '\0';
    }else{
      memcpy(failreason, ps, 1000);
      failreason[1000] = '\0';
      strcat(failreason,"...");
    }
    CONSOLE.Warning(1, "TRACKER FAILURE REASON: %s", failreason);
    return -1;
  }
  if( decode_query(buf,bufsiz,"warning message",&ps,&i,(int64_t*)0,QUERY_STR) ){
    char warnmsg[1024];
    if( i < 1024 ){
      memcpy(warnmsg, ps, i);
      warnmsg[i] = '\0';
    }else{
      memcpy(warnmsg, ps, 1000);
      warnmsg[1000] = '\0';
      strcat(warnmsg,"...");
    }
    CONSOLE.Warning(2, "TRACKER WARNING: %s", warnmsg);
  }

  m_peers_count = m_seeds_count = 0;

  if( decode_query(buf,bufsiz,"tracker id",&ps,&i,(int64_t*)0,QUERY_STR) ){
    if( i <= PEER_ID_LEN ){
      memcpy(m_trackerid, ps, i);
      m_trackerid[i] = '\0';
    }else{
      memcpy(m_trackerid, ps, PEER_ID_LEN);
      m_trackerid[PEER_ID_LEN] = '\0';
    }
  }

  if( !decode_query(buf, bufsiz, "interval", (const char**)0, &i,
      (int64_t*)0, QUERY_INT) )
    return -1;

  if(m_interval != (time_t)i) m_interval = (time_t)i;
  if(m_default_interval != (time_t)i) m_default_interval = (time_t)i;

  if( decode_query(buf, bufsiz, "complete", (const char**)0, &i,
      (int64_t*)0, QUERY_INT) )
    m_seeds_count = i;
  if( decode_query(buf, bufsiz, "incomplete", (const char**)0, &i,
      (int64_t*)0, QUERY_INT) )
    m_peers_count = m_seeds_count + i;
  else{
    if(arg_verbose && 0==m_seeds_count)
      CONSOLE.Debug("Tracker did not supply peers count.");
    m_peers_count = m_seeds_count;
  }

  pos = decode_query(buf, bufsiz, "peers", (const char**)0, (size_t *)0,
    (int64_t*)0, QUERY_POS);

  if( !pos ){
    return -1;
  }

  if(4 > bufsiz - pos){ return -1; } // peers list ̫С

  buf += (pos + 1); bufsiz -= (pos + 1);

  ps = buf-1;
  if( *ps != 'l' ){		// binary peers section if not 'l'
    addr.sin_family = AF_INET;
    i = 0;
    while( *ps != ':' ) i = i * 10 + (*ps++ - '0');
    i /= 6;
    ps++;
    while( i-- > 0 ){
      memcpy(&addr.sin_addr,ps,sizeof(struct in_addr));
      memcpy(&addr.sin_port,ps+sizeof(struct in_addr),sizeof(unsigned short));
      if( !Self.IpEquiv(addr) ){
        cnt++;
        IPQUEUE.Add(&addr);
      }
      ps += 6;
    }
  }
  else
  for( ; bufsiz && *buf!='e'; buf += pos, bufsiz -= pos ){
    pos = decode_dict(buf,bufsiz,(char*)0);
    if(!pos) break;
    if( !decode_query(buf,pos,"ip",&ps,&i,(int64_t*)0,QUERY_STR) ||
        MAXHOSTNAMELEN < i ) continue;
    memcpy(tmphost,ps,i); tmphost[i] = '\0';

    if( !decode_query(buf,pos,"port",(const char**)0,&tmpport,(int64_t*)0,
                      QUERY_INT) ) continue;

    if( !decode_query(buf,pos,"peer id",&ps,&i,(int64_t*)0,QUERY_STR) &&
        i != 20 ) continue;

    if(_IPsin(tmphost,tmpport,&addr) < 0){
      CONSOLE.Warning(3, "warn, detected invalid ip address %s.",tmphost);
      continue;
    }

    if( !Self.IpEquiv(addr) ){
      cnt++;
      IPQUEUE.Add(&addr);
    }
  }

  if( 0==m_peers_count ){
    m_peers_count = cnt + 1;  // include myself
    m_f_boguspeercnt = 1;
  }else m_f_boguspeercnt = 0;
  if(arg_verbose) CONSOLE.Debug("new peers=%d; next check in %d sec",
    (int)cnt, (int)m_interval);
  return 0;
}

int btTracker::CheckReponse()
{
  char *pdata, *format;
  ssize_t r;
  size_t q, hlen, dlen;

  r = m_reponse_buffer.FeedIn(m_sock);
  time(&m_last_timestamp);

  if( r > 0 ) return 0;  // connection is still open; may have more data coming

  q = m_reponse_buffer.Count();

  if( !q ){
    int error = 0;
    socklen_t n = sizeof(error);
    if( getsockopt(m_sock, SOL_SOCKET,SO_ERROR,&error,&n) < 0 )
      error = errno;
    if( error != 0 ) CONSOLE.Warning(2,
      "warn, received nothing from tracker:  %s", strerror(error));
    else CONSOLE.Warning(2, "warn, received nothing from tracker!");
    Reset(15);  // try again
    return -1;
  }

  Reset( (-1 == r) ? 15 : 0 );  // can't reset socket before error check

  hlen = Http_split(m_reponse_buffer.BasePointer(), q, &pdata,&dlen);

  if( !hlen ){
    CONSOLE.Warning(2, "warn, tracker reponse invalid. No html header found.");
    return -1;
  }

  r = Http_reponse_code(m_reponse_buffer.BasePointer(),hlen);
  if ( r != 200 ){
    if( r == 301 || r == 302 ){
      char redirect[MAXPATHLEN], ih_buf[20 * 3 + 1], pi_buf[20 * 3 + 1],
           tmppath[MAXPATHLEN];

      if( Http_get_header(m_reponse_buffer.BasePointer(), hlen, "Location",
          redirect) < 0 )
        return -1;

      if( Http_url_analyse(redirect,m_host,&m_port,m_path) < 0 ){
        CONSOLE.Warning(1,
          "warn, tracker redirected to an invalid url %s", redirect);
        return -1;
      }else{
        char *c = strstr(m_path, "?info_hash=");
        if( !c ) c = strstr(m_path, "&info_hash=");
        if( c ) *c = '\0';
        if(arg_verbose) CONSOLE.Debug("tracker redirect to %s", redirect);
        if( BuildBaseRequest() < 0 ) return -1;
      }

      if( Connect() < 0 ){
        Reset(15);
        return -1;
      }else return 0;
    }else if( r >= 400 ){
      CONSOLE.Warning(2, "Tracker reponse code >= 400 !!!");
      CONSOLE.Warning(2,
        "The file is not registered on this tracker or may have been removed.");
      CONSOLE.Warning(2,
        "IF YOU CONTINUE TO GET THIS MESSAGE AND DOWNLOAD DOES NOT BEGIN, PLEASE STOP CTORRENT!");
      if( pdata && dlen ){  // write(STDERR_FILENO, pdata, dlen);
        CONSOLE.Warning(0, "Tracker reponse data DUMP:");
        CONSOLE.Warning(0, "%s", pdata);
        CONSOLE.Warning(0, "== DUMP OVER==");
      }
      return -1;
    }else
      return 0;
  }

  if( !m_f_started ) m_f_started = 1;
  m_connect_refuse_click = 0;
  m_ok_click++;

  if ( !pdata ){
    CONSOLE.Warning(2, "warn, peers list received from tracker is empty.");
    return 0;
  }
  return _UpdatePeerList(pdata,dlen);
}

int btTracker::Initial()
{
  if(Http_url_analyse(BTCONTENT.GetAnnounce(),m_host,&m_port,m_path) < 0){
    CONSOLE.Warning(1, "error, invalid tracker url format!");
    return -1;
  }

  char chars[37] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  for(int i=0; i<8; i++)
    m_key[i] = chars[random()%36];
  m_key[8] = 0;

  if( BuildBaseRequest() < 0 ) return -1;

  /* get local ip address */
  { // Try to get address corresponding to the hostname.
    struct sockaddr_in addr;
    struct hostent *h;
    char hostname[MAXHOSTNAMELEN];

//    if(gethostname(hostname, MAXHOSTNAMELEN) == -1) return -1;
    if( gethostname(hostname, MAXHOSTNAMELEN) >= 0 ){
//    CONSOLE.Debug("hostname: %s", hostname);
      if( h = gethostbyname(hostname) ){
//      CONSOLE.Debug("Host name: %s", h->h_name);
//      CONSOLE.Debug("Address: %s", inet_ntoa(*((struct in_addr *)h->h_addr)));
        memcpy(&addr.sin_addr,h->h_addr,sizeof(struct in_addr));
        Self.SetIp(addr);
        return 0;
      }
    }
  }
  { // If behind NAT, this only gets the local side address.
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    if( getsockname(m_sock,(struct sockaddr*)&addr,&addrlen) == 0 )
      Self.SetIp(addr);
  }

  return 0;
}

int btTracker::BuildBaseRequest()
{
  char ih_buf[20 * 3 + 1], pi_buf[20 * 3 + 1], tmppath[MAXPATHLEN];
  const char *format;

  strcpy(tmppath,m_path);
  if(strchr(m_path, '?'))
    format=REQ_URL_P1A_FMT;
  else format=REQ_URL_P1_FMT;

  if(MAXPATHLEN < snprintf((char*)m_path,MAXPATHLEN,format,
                     tmppath,
                     Http_url_encode(ih_buf,(char*)BTCONTENT.GetInfoHash(),20),
                     Http_url_encode(pi_buf,(char*)BTCONTENT.GetPeerId(),20),
                     cfg_listen_port,
                     m_key)){
    return -1;
  }

  return 0;
}

int btTracker::Connect()
{
  ssize_t r;
  time(&m_last_timestamp);

  if(_s2sin(m_host,m_port,&m_sin) < 0) {
    CONSOLE.Warning(2, "warn, get tracker's ip address failed.");
    return -1;
  }

  m_sock = socket(AF_INET,SOCK_STREAM,0);
  if(INVALID_SOCKET == m_sock) return -1;

  // we only need to bind if we have specified an ip
  // we need it to bind here before the connect!!!!
  if ( cfg_listen_ip != 0 ) {
    struct sockaddr_in addr;
    // clear the struct as requested in the manpages
    memset(&addr,0, sizeof(sockaddr_in));
    // set the type
    addr.sin_family = AF_INET;
    // we want the system to choose port
    addr.sin_port = 0;
    // set the defined ip from the commandline
    addr.sin_addr.s_addr = cfg_listen_ip;
    // bind it or return...
    if(bind(m_sock,(struct sockaddr*)&addr,sizeof(struct sockaddr_in)) != 0){
      CONSOLE.Warning(1, "warn, can't set up tracker connection:  %s",
        strerror(errno));
      return -1;
    }
  }

  if(setfd_nonblock(m_sock) < 0){ CLOSE_SOCKET(m_sock); return -1; }

  r = connect_nonb(m_sock,(struct sockaddr*)&m_sin);

  if( r == -1 ){ CLOSE_SOCKET(m_sock); return -1; }
  else if( r == -2 ) m_status = T_CONNECTING;
  else{
    if(arg_verbose) CONSOLE.Debug("Connected to tracker");
    if( 0 == SendRequest() ) m_status = T_READY;
    else{ CLOSE_SOCKET(m_sock); return -1; }
  }
  return 0;
}

int btTracker::SendRequest()
{
  char *event,*str_event[] = {"started","stopped","completed" };
  char REQ_BUFFER[2*MAXPATHLEN];
  struct sockaddr_in addr;

  if( m_f_stoped )
    event = str_event[1];	/* stopped */
  else if( !m_f_started ){
    if( BTCONTENT.IsFull() ) m_f_completed = 1;
    event = str_event[0];	/* started */
  }else if( BTCONTENT.IsFull() && !m_f_completed ){
    if( Self.TotalDL() > 0 ) event = str_event[2];  /* download complete */
    else event = (char*) 0;  /* interval */
    m_f_completed = 1;		/* only send download complete once */
  }else
    event = (char*) 0;  /* interval */

  char opt1[20] = "&event=";
  char opt2[12+PEER_ID_LEN] = "&trackerid=";

  if(MAXPATHLEN < snprintf(REQ_BUFFER,MAXPATHLEN,REQ_URL_P2_FMT,
                     m_path,
                     event ? strncat(opt1,event,12) : "",
                     *m_trackerid ? strncat(opt2,m_trackerid,PEER_ID_LEN) : "",
                     (unsigned long long)(Self.TotalUL()),
                     (unsigned long long)(Self.TotalDL()),
                     (unsigned long long)(BTCONTENT.GetLeftBytes()),
                     (int)cfg_max_peers)){
    return -1;
  }

  // if we have a tracker hostname (not just an IP), send a Host: header
  if(_IPsin(m_host, m_port, &addr) < 0){
    char REQ_HOST[MAXHOSTNAMELEN];
    if(MAXHOSTNAMELEN < snprintf(REQ_HOST,MAXHOSTNAMELEN,"\r\nHost: %s",m_host))
      return -1;
    strcat(REQ_BUFFER, REQ_HOST);
  }

  strcat(REQ_BUFFER, "\r\nUser-Agent: ");
  strcat(REQ_BUFFER, cfg_user_agent);

  strcat(REQ_BUFFER,"\r\n\r\n");
  // hc
  //CONSOLE.Warning(0, "SendRequest: %s", REQ_BUFFER);

  if( 0 !=
      m_request_buffer.PutFlush(m_sock,REQ_BUFFER,strlen((char*)REQ_BUFFER)) ){
    CONSOLE.Warning(2,
      "warn, send request to tracker failed:  %s", strerror(errno));
    if( event == str_event[2] )
      m_f_completed = 0;  // failed sending completion event
    return -1;
  }

  return 0;
}

int btTracker::IntervalCheck(fd_set *rfdp, fd_set *wfdp)
{
  /* tracker communication */
  if( T_FREE == m_status ){
    if( INVALID_SOCKET != m_sock ){
      FD_CLR(m_sock, rfdp);
      FD_CLR(m_sock, wfdp);
    }
    if( now - m_last_timestamp >= m_interval ||
        // Connect to tracker early if we run low on peers.
        (WORLD.GetPeersCount() < cfg_min_peers &&
          m_prevpeers >= cfg_min_peers && now - m_last_timestamp >= 15) ){
      m_prevpeers = WORLD.GetPeersCount();

      if(Connect() < 0){ Reset(15); return -1; }

      FD_SET(m_sock, rfdp);
      if( m_status == T_CONNECTING ) FD_SET(m_sock, wfdp);
    }else if( now < m_last_timestamp ) m_last_timestamp = now; // time reversed
  }else{
    if( m_status == T_CONNECTING ){
      FD_SET(m_sock, rfdp);
      FD_SET(m_sock, wfdp);
    }else if( INVALID_SOCKET != m_sock ){
      FD_SET(m_sock, rfdp);
      if( m_request_buffer.Count() ) FD_SET(m_sock, wfdp);
    }
  }
  return m_sock;
}

int btTracker::SocketReady(fd_set *rfdp, fd_set *wfdp, int *nfds,
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
    if( error ){
      if( ECONNREFUSED == error ){
        if(arg_verbose) CONSOLE.Debug("tracker connection refused");
        m_connect_refuse_click++;
      }else CONSOLE.Warning(2,
          "warn, connect to tracker failed:  %s", strerror(error));
      Reset(15);
      return -1;
    }else{
      if(arg_verbose) CONSOLE.Debug("Connected to tracker");
      if( SendRequest() == 0 ) m_status = T_READY; 
      else { Reset(15); return -1; }
    }
  }else if( T_CONNECTING == m_status && FD_ISSET(m_sock,rfdp) ){
    int error = 0;
    socklen_t n = sizeof(error);
    (*nfds)--;
    FD_CLR(m_sock, rfdnextp);
    if(getsockopt(m_sock, SOL_SOCKET,SO_ERROR,&error,&n) < 0)
      error = errno;
    CONSOLE.Warning(2, "warn, connect to tracker failed:  %s", strerror(error));
    Reset(15);
    return -1;
  }else if( INVALID_SOCKET != m_sock ){
    if( FD_ISSET(m_sock, rfdp) ){
      (*nfds)--;
      FD_CLR(m_sock,rfdnextp);
      SOCKET tmp_sock = m_sock;
      int r = CheckReponse();
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
      if( m_request_buffer.Count() && m_request_buffer.FlushOut(m_sock) < 0 ){
        Reset(15);
        return -1;
      }
    }
  }else{  // failsafe
    Reset(15);
    return -1;
  }
  return 0;
}

void btTracker::Restart()
{
  m_f_stoped = m_f_restart = 0;

  if( T_FINISHED == m_status ){
    m_status = T_FREE;
    m_f_started = 0;
    m_interval = 15;
  }
}

void btTracker::SetStoped()
{
  if( !m_f_started ){
    m_f_stoped = 1;
    m_status = T_FINISHED;
  }else{
    Reset(15);
    m_f_stoped = 1;
  }
}

size_t btTracker::GetPeersCount() const 
{
  // includes seeds, so must always be >= 1 (myself!)
  return (m_peers_count > m_seeds_count) ? m_peers_count :
           (GetSeedsCount() + (BTCONTENT.IsFull() ? 0 : 1));
}

size_t btTracker::GetSeedsCount() const
{
  return m_seeds_count ? m_seeds_count : (BTCONTENT.IsFull() ? 1 : 0);
}

