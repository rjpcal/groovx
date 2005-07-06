/** @file nub/timer.cc wraps a signal/slot interface around a timer
    callback mechanism */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Thu Oct 14 10:56:13 2004
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

#ifndef GROOVX_NUB_TIMER_CC_UTC20050626084019_DEFINED
#define GROOVX_NUB_TIMER_CC_UTC20050626084019_DEFINED

#include "timer.h"

#include "rutz/error.h"

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

using rutz::shared_ptr;

nub::timer::timer(unsigned int msec, bool repeat)
  :
  sig_timeout(),
  m_scheduler(0),
  m_token(0),
  m_msec_delay(msec),
  m_is_repeating(repeat),
  m_stopwatch()
{}

nub::timer::~timer()
{
  cancel();
}

void nub::timer::schedule(rutz::shared_ptr<nub::scheduler> scheduler)
{
GVX_TRACE("nub::timer::schedule");

  GVX_PRECONDITION(scheduler.get() != 0);

  if (m_msec_delay == 0 && m_is_repeating == true)
    {
      throw rutz::error("can't schedule a timer callback with "
                        "delay=0 and repeating=true", SRC_POS);
    }

  // Cancel any possible previously pending invocation.
  cancel();

  // Note the time when the current scheduling request was made.
  m_stopwatch.restart();

  dbg_eval_nl(3, m_msec_delay);

  m_scheduler = scheduler;

  // Note that the returned token might be null for one reason or
  // another (e.g. if the scheduler decides to run the callback
  // immediately rather than scheduling a deferred callback).
  m_token = m_scheduler->schedule(m_msec_delay,
                                    dummy_callback,
                                    static_cast<void*>(this));
}

void nub::timer::cancel()
{
GVX_TRACE("nub::timer::cancel");

  m_token.reset(0);
}

void nub::timer::dummy_callback(void* clientdata)
{
GVX_TRACE("nub::timer::dummy_callback");
  nub::timer* timer = static_cast<nub::timer*>(clientdata);

  GVX_ASSERT(timer != 0);

  timer->m_token.reset(0);

  dbg_eval_nl(3, timer->m_stopwatch.elapsed().msec());

  // NOTE: make sure we re-schedule a repeating event BEFORE we
  // emit the signal and trigger the callbacks; this way, it's
  // possible for code inside the callback to cancel() this timer
  // callback and end the repeating.
  if (timer->m_is_repeating)
    {
      // can't allow a timer callback that is both repeating and
      // immediate (delay == 0), otherwise we fall into an
      // infinite loop
      GVX_ASSERT(timer->m_msec_delay != 0);

      if (timer->m_scheduler.get() != 0)
        timer->schedule(timer->m_scheduler);
    }

  timer->sig_timeout.emit();
}

static const char vcid_groovx_nub_timer_cc_utc20050626084019[] = "$Id$ $HeadURL$";
#endif // !GROOVX_NUB_TIMER_CC_UTC20050626084019_DEFINED
