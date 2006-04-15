/** @file nub/timer.h wraps a signal/slot interface around a timer
    callback mechanism */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Thu Oct 14 10:26:11 2004
// commit: $Id$
// $HeadURL$
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

#include "rutz/sharedptr.h"
#include "rutz/stopwatch.h"

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

  void schedule(rutz::shared_ptr<nub::scheduler> s);
  void cancel();

  unsigned int delay_msec() const { return m_msec_delay; }
  void set_delay_msec(unsigned int msec) { m_msec_delay = msec; }

  bool is_repeating() const { return m_is_repeating; }
  void set_repeating(bool repeat) { m_is_repeating = repeat; }

  bool is_pending() const { return m_token.get() != 0; }

  double elapsed_msec() const { return m_stopwatch.elapsed().msec(); }

private:
  static void dummy_callback(void* clientdata);

  timer(const timer&);
  timer& operator=(const timer&);

  rutz::shared_ptr<nub::scheduler>   m_scheduler;
  rutz::shared_ptr<nub::timer_token> m_token;
  unsigned int                       m_msec_delay;
  bool                               m_is_repeating;

  // Diagnostics
  mutable rutz::stopwatch            m_stopwatch;
};

static const char __attribute__((used)) vcid_groovx_nub_timer_h_utc20050626084019[] = "$Id$ $HeadURL$";
#endif // !GROOVX_NUB_TIMER_H_UTC20050626084019_DEFINED
