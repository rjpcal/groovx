///////////////////////////////////////////////////////////////////////
//
// stopwatch.h
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Nov 18 10:24:59 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef STOPWATCH_H_DEFINED
#define STOPWATCH_H_DEFINED

#include "util/time.h"

/// Tracks elapsed wall-clock time.
class StopWatch
{
public:
  /// Default constructor.
  StopWatch() : itsStartTime() { restart(); }

  /// Reset the start time to the current time.
  void restart()
    { itsStartTime = rutz::time::wall_clock_now(); }

  /// Get the time elapsed between start and stop times.
  rutz::time elapsed(const rutz::time& stop) const
  {
    return stop - itsStartTime;
  }

  /// Get the time elapsed between start and now.
  rutz::time elapsed() const
    {
      return elapsed(rutz::time::wall_clock_now());
    }

private:
  rutz::time itsStartTime;
};

static const char vcid_stopwatch_h[] = "$Header$";
#endif // !STOPWATCH_H_DEFINED
