///////////////////////////////////////////////////////////////////////
//
// tcltimer.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Aug 23 14:50:36 2001
// written: Thu Sep 13 11:31:14 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLTIMER_CC_DEFINED
#define TCLTIMER_CC_DEFINED

#include "tcl/tcltimer.h"

#include <tcl.h>

#include "util/trace.h"
#include "util/debug.h"


Tcl::Timer::Timer(unsigned int msec, bool repeat) :
  sigTimeOut(),
  itsMsecDelay(msec),
  isItRepeating(repeat),
  itsStopWatch(),
  itsToken(0)
{}

Tcl::Timer::~Timer()
{
  cancel();
}


void Tcl::Timer::schedule()
{
DOTRACE("Tcl::Timer::schedule");

  cancel();

  itsStopWatch.restart();

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

void Tcl::Timer::dummyCallback(ClientData clientData)
{
  Tcl::Timer* timer = static_cast<Tcl::Timer*>(clientData);

  Assert(timer != 0);

  timer->itsToken = 0;

  DebugEvalNL(timer->itsStopWatch.elapsedMsec());

  timer->sigTimeOut.emit();

  if (timer->isItRepeating)
    {
      timer->schedule();
    }
}

static const char vcid_tcltimer_cc[] = "$Header$";
#endif // !TCLTIMER_CC_DEFINED
