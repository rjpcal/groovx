///////////////////////////////////////////////////////////////////////
// timinghandler.cc
// Rob Peters
// created: Wed May 19 21:39:51 1999
// written: Thu May 20 12:54:06 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef TIMINGHANDLER_CC_DEFINED
#define TIMINGHANDLER_CC_DEFINED

#include "timinghandler.h"

#include "expt.h"
#include "exptdriver.h"

#define NO_TRACE
#include "trace.h"
#include "debug.h"

///////////////////////////////////////////////////////////////////////
//
// ExptTimer method definitions
//
///////////////////////////////////////////////////////////////////////

void ExptTimer::schedule(int msec) {
DOTRACE("ExptTimer::schedule");
  cancel();
  itsToken = Tcl_CreateTimerHandler(msec, dummyTimerProc, 
												static_cast<ClientData>(this));
}

void AbortTrialTimer::invoke() {
DOTRACE("AbortTrialTimer::invoke");
  itsExptDriver.abortTrial();
}

void StartTrialTimer::invoke() {
DOTRACE("StartTrialTimer::invoke");
  itsExptDriver.startTrial();
}

void UndrawTrialTimer::invoke() {
DOTRACE("UndrawTrialTimer::invoke");
  itsExptDriver.expt().undrawTrial();
}

///////////////////////////////////////////////////////////////////////
//
// TimingHandler creator method definitions
//
///////////////////////////////////////////////////////////////////////

TimingHandler::TimingHandler(ExptDriver& ed) :
  itsExptDriver(ed),
  itsAbortTimer(ed),
  itsStartTimer(ed),
  itsUndrawTrialTimer(ed)
{
DOTRACE("TimingHandler::TimingHandler");
  itsAbortWait = 1000;
  itsAutosavePeriod = 10;
  itsInterTrialInterval = 1000;
  itsStimDur = 2000;
  itsTimeout = 4000;
}

TimingHandler::~TimingHandler() {
DOTRACE("TimingHandler::~TimingHandler");
}

void TimingHandler::serialize(ostream &os, IOFlag flag) const {
DOTRACE("TimingHandler::serialize");
}

void TimingHandler::deserialize(istream &is, IOFlag flag) {
DOTRACE("TimingHandler::deserialize");
}

int TimingHandler::charCount() const {
DOTRACE("TimingHandler::charCount"); 
  return 0; 
}

///////////////////////////////////////////////////////////////////////
//
// TimingHandler action method definitions
//
///////////////////////////////////////////////////////////////////////

void TimingHandler::scheduleImmediate() {
DOTRACE("TimingHandler::scheduleImmediate");
}

void TimingHandler::scheduleFromStart() {
DOTRACE("TimingHandler::scheduleFromStart");
  itsUndrawTrialTimer.schedule(itsStimDur);
  itsAbortTimer.schedule(itsTimeout);
}

void TimingHandler::scheduleFromResponse() {
DOTRACE("TimingHandler::scheduleFromResponse");
}

void TimingHandler::scheduleNextTrial() {
DOTRACE("TimingHandler::scheduleNextTrial");
  itsStartTimer.schedule(itsInterTrialInterval);
}

void TimingHandler::scheduleAfterAbort() {
DOTRACE("TimingHandler::scheduleAfterAbort");
  itsStartTimer.schedule(itsAbortWait);
}

void TimingHandler::cancelAll() {
DOTRACE("TimingHandler::cancelAll");
  itsAbortTimer.cancel();
  itsStartTimer.cancel();
  itsUndrawTrialTimer.cancel();
}

static const char vcid_timinghandler_cc[] = "$Header$";
#endif // !TIMINGHANDLER_CC_DEFINED
