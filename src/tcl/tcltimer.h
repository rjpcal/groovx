///////////////////////////////////////////////////////////////////////
//
// tcltimer.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Aug 23 11:42:34 2001
// written: Wed Sep 25 18:57:38 2002
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

  Util::Signal sigTimeOut;

  void schedule();
  void cancel();

  unsigned int delayMsec() const { return itsMsecDelay; }
  void setDelayMsec(unsigned int msec) { itsMsecDelay = msec; }

  bool isRepeating() const { return isItRepeating; }
  void setRepeating(bool repeat) { isItRepeating = repeat; }

private:
  static void dummyCallback(ClientData clientData);

  Timer(const Timer&);
  Timer& operator=(const Timer&);

  unsigned int itsMsecDelay;
  bool isItRepeating;

  // Diagnostics
  mutable StopWatch itsStopWatch;

  Tcl_TimerToken itsToken;
};

static const char vcid_tcltimer_h[] = "$Header$";
#endif // !TCLTIMER_H_DEFINED
