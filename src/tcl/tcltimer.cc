///////////////////////////////////////////////////////////////////////
//
// tcltimer.cc
//
// Copyright (c) 2001-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Aug 23 14:50:36 2001
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

#ifndef TCLTIMER_CC_DEFINED
#define TCLTIMER_CC_DEFINED

#include "tcl/tcltimer.h"

#include "tcl/tcltimerscheduler.h"

#include "util/error.h"

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

Tcl::Timer::Timer(unsigned int msec, bool repeat) :
  sigTimeOut(),
  itsScheduler(new Tcl::TimerScheduler),
  itsToken(0),
  itsMsecDelay(msec),
  isItRepeating(repeat),
  itsStopWatch()
{}

Tcl::Timer::~Timer()
{
  cancel();
}

void Tcl::Timer::schedule()
{
DOTRACE("Tcl::Timer::schedule");

  if (itsMsecDelay == 0 && isItRepeating == true)
    {
      throw Util::Error("can't schedule a timer callback with "
                        "delay=0 and repeating=true", SRC_POS);
    }

  // Cancel any possible previously pending invocation.
  cancel();

  // Note the time when the current scheduling request was made.
  itsStopWatch.restart();

  dbgEvalNL(3, itsMsecDelay);

  // Note that the returned token might be null for one reason or
  // another (e.g. if the scheduler decides to run the callback
  // immediately rather than scheduling a deferred callback).
  itsToken = itsScheduler->schedule(itsMsecDelay,
                                    dummyCallback,
                                    static_cast<void*>(this));
}

void Tcl::Timer::cancel()
{
DOTRACE("Tcl::Timer::cancel");

  itsToken.reset(0);
}

void Tcl::Timer::dummyCallback(void* clientData)
{
DOTRACE("Tcl::Timer::dummyCallback");
  Tcl::Timer* timer = static_cast<Tcl::Timer*>(clientData);

  Assert(timer != 0);

  timer->itsToken.reset(0);

  dbgEvalNL(3, timer->itsStopWatch.elapsed().msec());

  // NOTE: make sure we re-schedule a repeating event BEFORE we
  // emit the signal and trigger the callbacks; this way, it's
  // possible for code inside the callback to cancel() this timer
  // callback and end the repeating.
  if (timer->isItRepeating)
    {
      // can't allow a timer callback that is both repeating and
      // immediate (delay == 0), otherwise we fall into an
      // infinite loop
      Assert(timer->itsMsecDelay != 0);
      timer->schedule();
    }

  timer->sigTimeOut.emit();
}

static const char vcid_tcltimer_cc[] = "$Header$";
#endif // !TCLTIMER_CC_DEFINED
