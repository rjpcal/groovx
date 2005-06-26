///////////////////////////////////////////////////////////////////////
//
// stopwatch.h
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Nov 18 10:24:59 1999
// commit: $Id$
// $HeadURL$
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

#ifndef GROOVX_RUTZ_STOPWATCH_H_UTC20050626084019_DEFINED
#define GROOVX_RUTZ_STOPWATCH_H_UTC20050626084019_DEFINED

#include "rutz/time.h"

namespace rutz
{
  class stopwatch;
}

/// Tracks elapsed wall-clock time.
class rutz::stopwatch
{
public:
  /// Default constructor.
  stopwatch() : m_start_time() { restart(); }

  /// Reset the start time to the current time.
  void restart()
    { m_start_time = rutz::time::wall_clock_now(); }

  /// Get the time elapsed between start and stop times.
  rutz::time elapsed(const rutz::time& stop) const
  {
    return stop - m_start_time;
  }

  /// Get the time elapsed between start and now.
  rutz::time elapsed() const
    {
      return elapsed(rutz::time::wall_clock_now());
    }

private:
  rutz::time m_start_time;
};

static const char vcid_groovx_rutz_stopwatch_h_utc20050626084019[] = "$Id$ $HeadURL$";
#endif // !GROOVX_RUTZ_STOPWATCH_H_UTC20050626084019_DEFINED
