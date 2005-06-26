///////////////////////////////////////////////////////////////////////
//
// timer.h
//
// Copyright (c) 2004-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Oct 14 10:26:11 2004
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

#ifndef TIMER_H_DEFINED
#define TIMER_H_DEFINED

#include "nub/scheduler.h"
#include "nub/signal.h"

#include "rutz/sharedptr.h"
#include "rutz/stopwatch.h"

namespace Nub
{
  class Timer;
}

/// Wraps a signal/slot interface around a timer callback mechansim.
class Nub::Timer
{
public:
  Timer(unsigned int msec, bool repeat = false);

  virtual ~Timer();

  Nub::Signal0 sigTimeOut;

  void schedule(rutz::shared_ptr<Nub::Scheduler> s);
  void cancel();

  unsigned int delayMsec() const { return itsMsecDelay; }
  void setDelayMsec(unsigned int msec) { itsMsecDelay = msec; }

  bool isRepeating() const { return isItRepeating; }
  void setRepeating(bool repeat) { isItRepeating = repeat; }

  bool isPending() const { return itsToken.get() != 0; }

  double elapsedMsec() const { return itsStopWatch.elapsed().msec(); }

private:
  static void dummyCallback(void* clientData);

  Timer(const Timer&);
  Timer& operator=(const Timer&);

  rutz::shared_ptr<Nub::Scheduler> itsScheduler;

  rutz::shared_ptr<Nub::TimerToken> itsToken;
  unsigned int itsMsecDelay;
  bool isItRepeating;

  // Diagnostics
  mutable rutz::stopwatch itsStopWatch;
};

static const char vcid_timer_h[] = "$Id$ $URL$";
#endif // !TIMER_H_DEFINED
