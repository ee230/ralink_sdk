#include "rate.h"  // def.h

#include <sys/time.h>

#include "bufio.h" // for BUF_DEF_SIZ
#include "bttime.h"
#include "console.h"

#ifndef HAVE_CLOCK_GETTIME
#include "compat.h"
#endif

#define RATE_INTERVAL 20
#define SHORT_INTERVAL 5

Rate::Rate()
{
  m_last_timestamp = m_total_timeused = (time_t)0;
  m_count_bytes = 0;
  m_history = m_history_last = (BWSAMPLE *)0;
  m_last_realtime = m_recent_realtime = m_prev_realtime = 0;
  m_last_size = m_recent_size = m_prev_size = 0;
  m_selfrate = (Rate *)0;
  m_late = 0;
  m_ontime = 0;
}

void Rate::Reset()
{
  m_last_timestamp = m_total_timeused = (time_t)0;
  ClearHistory();
  m_last_realtime = 0;
  m_last_size = 0;
}

void Rate::StartTimer()
{
  if( !m_last_timestamp ) m_last_timestamp = now;
}

void Rate::StopTimer()
{
  if( m_last_timestamp ){
    m_total_timeused += (now - m_last_timestamp);
    m_last_timestamp = 0;
    ClearHistory();
  }
}

BWSAMPLE *Rate::NewSample()
{
  BWSAMPLE *sample = new BWSAMPLE;
  if( sample ){
    sample->timestamp = 0;
    sample->bytes = 0;
    sample->next = (BWSAMPLE *)0;
  }else
    CONSOLE.Warning(2, "warn, failed to allocate memory for bandwidth sample.");

  return sample;
}

void Rate::ClearHistory()
{
  if( m_history ){
    BWSAMPLE *pnext;
    for( BWSAMPLE *p=m_history; p; p=pnext ){
      pnext = p->next;
      delete p;
    }
    m_history = m_history_last = (BWSAMPLE *)0;
  }
}

void Rate::Cleanup()
{
  BWSAMPLE *p = m_history;

  while( p && RATE_INTERVAL <= now - (time_t)(p->timestamp) ){
    int nzero = 0;
    if( !p->next ){
      if( BWSAMPLE *q = NewSample() ){
        q->timestamp = (double)now - 1;
        p->next = q;
        m_history_last = q;
        nzero++;
      }else{
        p->bytes = p->bytes * RATE_INTERVAL / (now - (time_t)(p->timestamp));
        p->timestamp = (double)(now - RATE_INTERVAL + 1);
      }
    }
    if( p->next ){
      if( RATE_INTERVAL > now - (time_t)(p->next->timestamp) ){
        time_t reftime = nzero ? now : (time_t)(p->next->timestamp);
        while( (time_t)(p->next->timestamp) > (time_t)(p->timestamp) + 1 ){
          // fill holes
          if( BWSAMPLE *q = NewSample() ){
            q->timestamp = (double)((time_t)(p->next->timestamp) - 1);
            q->next = p->next;
            p->next = q;
            nzero++;
          }else break;
        }
        if( nzero ){
          size_t bytes = (size_t)( p->bytes / (reftime - p->timestamp) );
          BWSAMPLE *q = p->next;
          for( ; nzero; nzero-- ){
            q->bytes += bytes;  // distribute over the following empty samples
            q = q->next;
          }
        }
      }
      m_history = p->next;
      delete p;
      p = m_history;
    }
  }
}

void Rate::CountAdd(size_t nbytes)
{
  m_count_bytes += nbytes;
  if( m_selfrate ) m_selfrate->CountAdd(nbytes);
}

void Rate::RateAdd(size_t nbytes, size_t bwlimit)
{
  struct timespec nowspec;

  clock_gettime(CLOCK_REALTIME, &nowspec);

  RateAdd(nbytes, bwlimit,
    nowspec.tv_sec + (double)(nowspec.tv_nsec)/1000000000);
}

void Rate::RateAdd(size_t nbytes, size_t bwlimit, double timestamp)
{
  if( m_history_last && timestamp < m_history_last->timestamp ){
    // time went backward
    ClearHistory();
  }else Cleanup();
  if( timestamp <= m_last_realtime ){  // time went backward
    m_ontime = 0;
    m_last_size = 0;
    m_last_realtime = 0;
  }

  if( m_history_last &&
      (time_t)timestamp == (time_t)(m_history_last->timestamp) )
    m_history_last->bytes += nbytes;
  else{
    if( BWSAMPLE *p = NewSample() ){
      p->timestamp = timestamp;
      p->bytes = nbytes;
      if( m_history_last ) m_history_last->next = p;
      else m_history = p;
      m_history_last = p;
    }
  }

  if( !m_selfrate && m_ontime ){
    double late=timestamp - (m_last_realtime + (double)m_last_size / bwlimit);
//  double tmplate = late;
    // keep the change under control in case the system gets weird on us
    if( late < 0 ) late /= 2;
    else if( m_late && late > m_late ) late = m_late / 2;
    m_late += late;
//  CONSOLE.Debug("%p late %f->%f: %f", this, tmplate, late, m_late);
    m_ontime = 0;
  }

  if( m_selfrate && bwlimit && m_last_realtime && m_selfrate->LastSize() /
      (timestamp - m_selfrate->LastRealtime()) > bwlimit )
    m_last_size += nbytes;
  else if( !m_selfrate && bwlimit && m_last_realtime &&
           m_last_size / (timestamp - m_last_realtime) > bwlimit )
    m_last_size += nbytes;
  else{
    m_last_realtime = timestamp;
    m_last_size = nbytes;
  }

  if( nbytes > BUF_DEF_SIZ ){
    m_prev_realtime = m_recent_realtime;
    m_prev_size = m_recent_size;
    m_recent_realtime = timestamp;
    m_recent_size = nbytes;
  }else m_recent_size += nbytes;

  if( m_selfrate ) m_selfrate->RateAdd(nbytes, bwlimit, timestamp);

//if(!m_selfrate) CONSOLE.Debug("%p RateAdd %u @ %f next=%f", this,
//  nbytes, timestamp, m_last_realtime + (double)m_last_size / bwlimit);
}

void Rate::operator=(const Rate &ra)
{
  m_last_timestamp = now;
  m_count_bytes = ra.m_count_bytes;
}

size_t Rate::CurrentRate()
{
  // We can't make up for past slowness by overloading the line now/future.
  // Look at only the most recent data sent/received.
  if( !m_last_timestamp || !m_history ) return 0; // no current rate

  struct timespec timestamp;
  clock_gettime(CLOCK_REALTIME, &timestamp);

  double timeused = timestamp.tv_sec + (double)(timestamp.tv_nsec)/1000000000 -
    m_last_realtime;
  if( timeused <= 0 ) return 0;

  return (size_t)( m_last_size / timeused );
}

size_t Rate::RateMeasure()
{
  // calculate rate based on bandwidth history data
  time_t timestamp = now;
  unsigned long countbytes = 0;
  double timeused = 0;
  BWSAMPLE *p;

  if( !m_last_timestamp || !m_history ) return 0; // no current rate

  Cleanup();
  for( p=m_history; p; p=p->next ){
    countbytes += p->bytes;
  }
  timeused = (double)(now - (time_t)(m_history->timestamp));
  if( timeused == 0 ) timeused = 1;
  else if( timeused < 0 ) ClearHistory();  // time went backward
  if( now < (time_t)m_recent_realtime ){
    if( m_history ){
      m_recent_realtime = (double)now;
      m_prev_realtime = (double)(now - 1);
      m_recent_size = m_prev_size = 0;
    }else{
      m_recent_realtime = m_prev_realtime = 0;
      m_recent_size = m_prev_size = 0;
    }
  }
  if( !m_history ) return 0;

  // Don't let the most recent addition inflate the rate measurement.
  if( now == (time_t)m_recent_realtime ){
    // don't count the most recent addition
    countbytes -= m_recent_size;
    timeused = m_recent_realtime - m_history->timestamp;
  }else if( m_recent_realtime &&
            RATE_INTERVAL > now - (time_t)m_recent_realtime &&
            m_recent_size / (now - (time_t)m_recent_realtime) >
            m_prev_size / (m_recent_realtime - m_prev_realtime) ){
    // "tone down" the most recent to match the previous addition's rate
    countbytes -= m_recent_size;
    countbytes += (unsigned long)(
      m_prev_size / (m_recent_realtime - m_prev_realtime) *
      (now - (time_t)m_recent_realtime) );
  }

  return (size_t)(countbytes / timeused);
}

size_t Rate::RateMeasure(const Rate &ra_to)
{
  time_t timeused = TimeUsed();
  int tmp = ra_to.m_count_bytes - m_count_bytes;
  return (size_t)( (tmp>0) ? (tmp/(timeused ? timeused : 1)) : 0 );
}

time_t Rate::TimeUsed()
{
  if( now < m_last_timestamp ) m_last_timestamp = now;
  return now - m_last_timestamp;
}

