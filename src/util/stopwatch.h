///////////////////////////////////////////////////////////////////////
//
// stopwatch.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Thu Nov 18 10:24:59 1999
// written: Sat Mar 29 12:53:40 2003
// $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef STOPWATCH_H_DEFINED
#define STOPWATCH_H_DEFINED

#include <sys/time.h>

/// Tracks elapsed wall-clock time.
class StopWatch
{
public:
  /// Default constructor.
  StopWatch() : itsStartTime() { restart(); }

  /// Reset the start time to the current time.
  void restart()
    { gettimeofday(&itsStartTime, /* timezone */ 0); }

  /// Get the number of milliseconds between start and stop times.
  double elapsedMsec(const timeval& stop) const
    {
      timeval elapsedTime;

      elapsedTime.tv_sec = stop.tv_sec - itsStartTime.tv_sec;
      elapsedTime.tv_usec = stop.tv_usec - itsStartTime.tv_usec;

      return (double(elapsedTime.tv_sec)*1000.0 +
              double(elapsedTime.tv_usec)/1000.0);
    }

  /// Get the number of milliseconds between start and now.
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
