///////////////////////////////////////////////////////////////////////
//
// tcltimer.h
//
// Copyright (c) 2001-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Aug 23 11:42:34 2001
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

#ifndef TCLTIMER_H_DEFINED
#define TCLTIMER_H_DEFINED

#include "util/pointers.h"
#include "util/signal.h"
#include "util/stopwatch.h"

struct Tcl_TimerToken_;
typedef struct Tcl_TimerToken_* Tcl_TimerToken;
typedef void* ClientData;

template <class T> class shared_ptr;

namespace Tcl
{
  class Timer;
  class TimerScheduler;
  class TimerSchedulerToken;
}

namespace Util
{
  class TimerToken;
}

class Util::TimerToken
{
private:
  TimerToken(const TimerToken&);
  TimerToken& operator=(const TimerToken&);

protected:
  TimerToken();

public:
  virtual ~TimerToken() throw();
};

class Tcl::TimerSchedulerToken : public Util::TimerToken
{
public:
  TimerSchedulerToken(Tcl_TimerToken tok);
  virtual ~TimerSchedulerToken() throw();

private:
  Tcl_TimerToken itsToken;
};

class Tcl::TimerScheduler
{
public:
  TimerScheduler();

  // Tcl_TimerToken
  shared_ptr<Util::TimerToken> schedule(int msec,
                                        void (*callback)(void*),
                                        void* clientdata);
};

/// Wraps a signal/slot interface around the Tcl timer callback mechansim.
class Tcl::Timer
{
public:
  Timer(unsigned int msec, bool repeat = false);
  ~Timer();

  Util::Signal0 sigTimeOut;

  void schedule();
  void cancel();

  unsigned int delayMsec() const { return itsMsecDelay; }
  void setDelayMsec(unsigned int msec) { itsMsecDelay = msec; }

  bool isRepeating() const { return isItRepeating; }
  void setRepeating(bool repeat) { isItRepeating = repeat; }

  bool isPending() const { return itsToken.get() != 0; }

  double elapsedMsec() const { return itsStopWatch.elapsed().msec(); }

private:
  static void dummyCallback(ClientData clientData) throw();

  Timer(const Timer&);
  Timer& operator=(const Timer&);

  Tcl::TimerScheduler itsScheduler;

  shared_ptr<Util::TimerToken> itsToken;
  unsigned int itsMsecDelay;
  bool isItRepeating;

  // Diagnostics
  mutable StopWatch itsStopWatch;
};

static const char vcid_tcltimer_h[] = "$Header$";
#endif // !TCLTIMER_H_DEFINED
