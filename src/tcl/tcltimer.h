///////////////////////////////////////////////////////////////////////
//
// tcltimer.h
//
// Copyright (c) 2001-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Thu Aug 23 11:42:34 2001
// written: Wed Mar 19 12:45:42 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLTIMER_H_DEFINED
#define TCLTIMER_H_DEFINED

#include "util/signal.h"
#include "util/stopwatch.h"

struct Tcl_TimerToken_;
typedef struct Tcl_TimerToken_* Tcl_TimerToken;
typedef void* ClientData;

namespace Tcl
{
  class Timer;
}

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

  bool isPending() const { return itsToken != 0; }

  double elapsedMsec() const { return itsStopWatch.elapsedMsec(); }

private:
  static void dummyCallback(ClientData clientData) throw();

  Timer(const Timer&);
  Timer& operator=(const Timer&);

  Tcl_TimerToken itsToken;
  unsigned int itsMsecDelay;
  bool isItRepeating;

  // Diagnostics
  mutable StopWatch itsStopWatch;
};

static const char vcid_tcltimer_h[] = "$Header$";
#endif // !TCLTIMER_H_DEFINED
