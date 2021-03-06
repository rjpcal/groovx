/** @file nub/timer.cc wraps a signal/slot interface around a timer
    callback mechanism */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Thu Oct 14 10:56:13 2004
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#include "timer.h"

#include "rutz/error.h"

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

using std::shared_ptr;

nub::timer::timer(unsigned int msec, bool repeat)
  :
  sig_timeout(),
  m_scheduler(nullptr),
  m_token(nullptr),
  m_msec_delay(msec),
  m_is_repeating(repeat),
  m_stopwatch()
{}

nub::timer::~timer()
{
  cancel();
}

void nub::timer::schedule(std::shared_ptr<nub::scheduler> scheduler)
{
GVX_TRACE("nub::timer::schedule");

  GVX_PRECONDITION(scheduler.get() != nullptr);

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
  m_token = m_scheduler->schedule(int(m_msec_delay),
                                  std::bind(&nub::timer::callback, this));
}

void nub::timer::cancel()
{
GVX_TRACE("nub::timer::cancel");

  m_token.reset();
}

void nub::timer::callback()
{
GVX_TRACE("nub::timer::dummy_callback");

  m_token.reset();

  dbg_eval_nl(3, m_stopwatch.elapsed().msec());

  // NOTE: make sure we re-schedule a repeating event BEFORE we
  // emit the signal and trigger the callbacks; this way, it's
  // possible for code inside the callback to cancel() this this
  // callback and end the repeating.
  if (m_is_repeating)
    {
      // can't allow a timer callback that is both repeating and
      // immediate (delay == 0), otherwise we fall into an
      // infinite loop
      GVX_ASSERT(m_msec_delay != 0);

      if (m_scheduler.get() != nullptr)
        schedule(m_scheduler);
    }

  sig_timeout.emit();
}
