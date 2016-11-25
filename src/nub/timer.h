/** @file nub/timer.h wraps a signal/slot interface around a timer
    callback mechanism */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Thu Oct 14 10:26:11 2004
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_NUB_TIMER_H_UTC20050626084019_DEFINED
#define GROOVX_NUB_TIMER_H_UTC20050626084019_DEFINED

#include "nub/scheduler.h"
#include "nub/signal.h"

#include "rutz/stopwatch.h"

#include <memory>

namespace nub
{
  class timer;
}

/// Wraps a signal/slot interface around a timer callback mechansim.
class nub::timer
{
public:
  timer(unsigned int msec, bool repeat = false);

  virtual ~timer();

  nub::signal0 sig_timeout;

  void schedule(std::shared_ptr<nub::scheduler> s);
  void cancel();

  unsigned int delay_msec() const { return m_msec_delay; }
  void set_delay_msec(unsigned int msec) { m_msec_delay = msec; }

  bool is_repeating() const { return m_is_repeating; }
  void set_repeating(bool repeat) { m_is_repeating = repeat; }

  bool is_pending() const { return m_token.get() != nullptr; }

  double elapsed_msec() const { return m_stopwatch.elapsed().msec(); }

private:
  void callback();

  timer(const timer&);
  timer& operator=(const timer&);

  std::shared_ptr<nub::scheduler>   m_scheduler;
  std::shared_ptr<nub::timer_token> m_token;
  unsigned int                       m_msec_delay;
  bool                               m_is_repeating;

  // Diagnostics
  mutable rutz::stopwatch            m_stopwatch;
};

#endif // !GROOVX_NUB_TIMER_H_UTC20050626084019_DEFINED
