///////////////////////////////////////////////////////////////////////
//
// timer.cc
//
// Copyright (c) 2004-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Oct 14 10:56:13 2004
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

#ifndef TIMER_CC_DEFINED
#define TIMER_CC_DEFINED

#include "timer.h"

#include "util/error.h"

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

using rutz::shared_ptr;

Util::Timer::Timer(unsigned int msec, bool repeat)
  :
  sigTimeOut(),
  itsScheduler(0),
  itsToken(0),
  itsMsecDelay(msec),
  isItRepeating(repeat),
  itsStopWatch()
{}

Util::Timer::~Timer()
{
  cancel();
}

void Util::Timer::schedule(rutz::shared_ptr<Util::Scheduler> scheduler)
{
DOTRACE("Util::Timer::schedule");

  PRECONDITION(scheduler.get() != 0);

  if (itsMsecDelay == 0 && isItRepeating == true)
    {
      throw rutz::error("can't schedule a timer callback with "
                        "delay=0 and repeating=true", SRC_POS);
    }

  // Cancel any possible previously pending invocation.
  cancel();

  // Note the time when the current scheduling request was made.
  itsStopWatch.restart();

  dbg_eval_nl(3, itsMsecDelay);

  itsScheduler = scheduler;

  // Note that the returned token might be null for one reason or
  // another (e.g. if the scheduler decides to run the callback
  // immediately rather than scheduling a deferred callback).
  itsToken = itsScheduler->schedule(itsMsecDelay,
                                    dummyCallback,
                                    static_cast<void*>(this));
}

void Util::Timer::cancel()
{
DOTRACE("Util::Timer::cancel");

  itsToken.reset(0);
}

void Util::Timer::dummyCallback(void* clientData)
{
DOTRACE("Util::Timer::dummyCallback");
  Util::Timer* timer = static_cast<Util::Timer*>(clientData);

  ASSERT(timer != 0);

  timer->itsToken.reset(0);

  dbg_eval_nl(3, timer->itsStopWatch.elapsed().msec());

  // NOTE: make sure we re-schedule a repeating event BEFORE we
  // emit the signal and trigger the callbacks; this way, it's
  // possible for code inside the callback to cancel() this timer
  // callback and end the repeating.
  if (timer->isItRepeating)
    {
      // can't allow a timer callback that is both repeating and
      // immediate (delay == 0), otherwise we fall into an
      // infinite loop
      ASSERT(timer->itsMsecDelay != 0);

      if (timer->itsScheduler.get() != 0)
        timer->schedule(timer->itsScheduler);
    }

  timer->sigTimeOut.emit();
}

static const char vcid_timer_cc[] = "$Header$";
#endif // !TIMER_CC_DEFINED
