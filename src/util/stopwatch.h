///////////////////////////////////////////////////////////////////////
//
// stopwatch.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Nov 18 10:24:59 1999
// written: Mon Jan 13 11:08:25 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef STOPWATCH_H_DEFINED
#define STOPWATCH_H_DEFINED

#include <sys/time.h>

class StopWatch
{
public:
  StopWatch() : itsStartTime() { restart(); }

  void restart()
    { gettimeofday(&itsStartTime, /* timezone */ 0); }

  double elapsedMsec(const timeval& now) const
    {
      timeval elapsedTime;

      elapsedTime.tv_sec = now.tv_sec - itsStartTime.tv_sec;
      elapsedTime.tv_usec = now.tv_usec - itsStartTime.tv_usec;

      return (double(elapsedTime.tv_sec)*1000.0 +
              double(elapsedTime.tv_usec)/1000.0);
    }

  double elapsedMsec() const
    {
      timeval now;
      gettimeofday(&now, /* timezone */ 0);

      return elapsedMsec(now);
    }

private:
  timeval itsStartTime;
};

static const char vcid_stopwatch_h[] = "$Header$";
#endif // !STOPWATCH_H_DEFINED
