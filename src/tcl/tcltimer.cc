///////////////////////////////////////////////////////////////////////
//
// tcltimer.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Aug 23 14:50:36 2001
// written: Thu Dec 19 18:10:09 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLTIMER_CC_DEFINED
#define TCLTIMER_CC_DEFINED

#include "tcl/tcltimer.h"

#include "tcl/tclsafeinterp.h"

#include <tcl.h>

#include "util/trace.h"
#include "util/debug.h"


Tcl::Timer::Timer(unsigned int msec, bool repeat) :
  sigTimeOut(),
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

  // Cancel any possible previously pending invocation.
  cancel();

  // Note the time when the current scheduling request was made.
  itsStopWatch.restart();

  dbgEvalNL(3, itsMsecDelay);

  itsToken = Tcl_CreateTimerHandler(itsMsecDelay,
                                    dummyCallback,
                                    static_cast<ClientData>(this));
}

void Tcl::Timer::cancel()
{
DOTRACE("Tcl::Timer::cancel");

  Tcl_DeleteTimerHandler(itsToken);

  itsToken = 0;
}

void Tcl::Timer::dummyCallback(ClientData clientData) throw()
{
DOTRACE("Tcl::Timer::dummyCallback");
  Tcl::Timer* timer = static_cast<Tcl::Timer*>(clientData);

  Assert(timer != 0);

  try
    {
      timer->itsToken = 0;

      dbgEvalNL(3, timer->itsStopWatch.elapsedMsec());

      timer->sigTimeOut.emit();

      if (timer->isItRepeating)
        {
          timer->schedule();
        }
    }
  catch(...)
    {
      Tcl::Main::interp().handleLiveException("Tcl::Timer callback", true);
    }
}

static const char vcid_tcltimer_cc[] = "$Header$";
#endif // !TCLTIMER_CC_DEFINED
