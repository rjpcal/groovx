///////////////////////////////////////////////////////////////////////
//
// timinghandler.cc
// Rob Peters
// created: Wed May 19 21:39:51 1999
// written: Fri Jun 11 11:59:47 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TIMINGHANDLER_CC_DEFINED
#define TIMINGHANDLER_CC_DEFINED

#include "timinghandler.h"

#include <cmath>

#include "exptdriver.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

///////////////////////////////////////////////////////////////////////
//
// File scope declarations
//
///////////////////////////////////////////////////////////////////////

namespace {
  ExptDriver& exptDriver = ExptDriver::theExptDriver();

  const char* const ioTag = "TimingHandler";
}

///////////////////////////////////////////////////////////////////////
//
// ExptEvent method definitions
//
///////////////////////////////////////////////////////////////////////

ExptEvent::ExptEvent(int msec) :
  itsToken(NULL),
  itsRequestedDelay(msec),
  itsDelayErrors(0),
  itsInvokeCount(0)
{
DOTRACE("ExptEvent::ExptEvent");
}

ExptEvent::~ExptEvent() {
DOTRACE("ExptEvent::~ExptEvent");

  cancel();

  DebugEval(itsDelayErrors);
  DebugEval(itsInvokeCount);
  DebugEvalNL(itsInvokeCount ? itsDelayErrors/itsInvokeCount : 0);
}

void ExptEvent::schedule() {
DOTRACE("ExptEvent::schedule");
  // Cancel any possible previously pending invocation.
  cancel();

  // Note the time when the current scheduling request was made.
  gettimeofday(&itsBeginTime, NULL);

  // If the requested time is zero or negative - i.e., immediate,
  // don't bother creating a timer handler. Instead, generate a direct
  // invocation.
  if (itsRequestedDelay <= 0) {
	 invoke();
  }
  // Otherwise, set up a timer that will call the invocation after the
  // specified amount of time.
  else {
	 itsToken = Tcl_CreateTimerHandler(itsRequestedDelay, dummyInvoke, 
												  static_cast<ClientData>(this));
  }
}

void ExptEvent::cancel() {
DOTRACE("ExptEvent::cancel");
  // Cancel the timer handler associated with itsToken. This is a safe
  // no-op if itsToken is NULL.
  Tcl_DeleteTimerHandler(itsToken);
}

ExptDriver& ExptEvent::getExptDriver() {
DOTRACE("ExptEvent::getExptDriver");
  DebugEvalNL((void *) &exptDriver);
  return exptDriver;
}

void ExptEvent::dummyInvoke(ClientData clientData) {
DOTRACE("ExptEvent::dummyInvoke");
  ExptEvent* event = static_cast<ExptEvent *>(clientData);

  // Check the elapsed time and compare it to the requested elapsed
  // time.
  static timeval endTime, elapsedTime;
  gettimeofday(&endTime, NULL);

  elapsedTime.tv_sec = endTime.tv_sec - event->itsBeginTime.tv_sec;
  elapsedTime.tv_usec = endTime.tv_usec - event->itsBeginTime.tv_usec;

  int msec = int(double(elapsedTime.tv_sec)*1000.0 +
					  double(elapsedTime.tv_usec)/1000.0);
  event->itsDelayErrors += abs(event->itsRequestedDelay - msec);
  ++(event->itsInvokeCount);

  // Do the actual event callback.
  event->invoke();
}

///////////////////////////////////////////////////////////////////////
//
// ExptEvent derived class overrides of invoke()
//
///////////////////////////////////////////////////////////////////////

void AbortTrialEvent::invoke() {
DOTRACE("AbortTrialEvent::invoke");
  getExptDriver().edAbortTrial();
}

void BeginTrialEvent::invoke() {
DOTRACE("BeginTrialEvent::invoke");
  getExptDriver().edBeginTrial();
}

void EndTrialEvent::invoke() {
DOTRACE("EndTrialEvent::invoke");
  getExptDriver().edEndTrial();
}

void DrawEvent::invoke() {
DOTRACE("DrawEvent::invoke");
  getExptDriver().draw();
}

void UndrawEvent::invoke() {
DOTRACE("UndrawEvent::invoke");
  getExptDriver().undraw();
}

///////////////////////////////////////////////////////////////////////
//
// TimingHandler creator method definitions
//
///////////////////////////////////////////////////////////////////////

TimingHandler::TimingHandler() :
  itsTimeoutEvent(4000),
  itsBeginEvent(1000),
  itsAbortWaitEvent(0),
  itsDrawEvent(0),
  itsUndrawEvent(2000),
  itsAutosavePeriod(10)
{
DOTRACE("TimingHandler::TimingHandler");
}

TimingHandler::~TimingHandler() {
DOTRACE("TimingHandler::~TimingHandler");
}

void TimingHandler::serialize(ostream &os, IOFlag flag) const {
DOTRACE("TimingHandler::serialize");
  if (flag & BASES) { /* no bases to serialize */ }

  char sep = ' ';
  if (flag & TYPENAME) { os << ioTag << sep; }
  
  os << getAbortWait() << sep;

  os << getAutosavePeriod() << sep;

  os << getInterTrialInterval() << sep;

  os << getStimDur() << sep;

  os << getTimeout() << endl;

  if (os.fail()) throw OutputError(ioTag);
}

void TimingHandler::deserialize(istream &is, IOFlag flag) {
DOTRACE("TimingHandler::deserialize");
  if (flag & BASES) { /* no bases to deserialize */ }
  if (flag & TYPENAME) { IO::readTypename(is, ioTag); }

  if (is.peek() != EOF) {
	 int aAbortWait;
	 is >> aAbortWait;
	 setAbortWait(aAbortWait);
  }

  if (is.peek() != EOF) {
	 int aAutosavePeriod;
	 is >> aAutosavePeriod;
	 setAutosavePeriod(aAutosavePeriod);
  }

  if (is.peek() != EOF) {
	 int aInterTrialInterval;
	 is >> aInterTrialInterval;
	 setInterTrialInterval(aInterTrialInterval);
  }

  if (is.peek() != EOF) {
	 int aStimDur;
	 is >> aStimDur;
	 setStimDur(aStimDur);
  }

  if (is.peek() != EOF) {
	 int aTimeout;
	 is >> aTimeout;
	 setTimeout(aTimeout);
  }

  if (is.fail()) throw InputError(ioTag);
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

void TimingHandler::thBeginTrial() {
DOTRACE("TimingHandler::thBeginTrial");
  cancelAll();

  itsDrawEvent.schedule();
  itsUndrawEvent.schedule();
  itsTimeoutEvent.schedule();
}

void TimingHandler::thAbortTrial() {
DOTRACE("TimingHandler::thAbortTrial");
  cancelAll();

  itsAbortWaitEvent.schedule();
}

//--------------------------------------------------------------------
//
// TimingHandler::thEndTrial --
//
// Invoke a timer to schedule the next trial to begin after the
// appropriate inter-trial interval.
//
//--------------------------------------------------------------------

void TimingHandler::thEndTrial() {
DOTRACE("TimingHandler::thEndTrial");
  itsBeginEvent.schedule();
}

void TimingHandler::thHaltExpt() {
DOTRACE("TimingHandler::thHaltExpt");
  cancelAll();
}

void TimingHandler::thResponseSeen() {
DOTRACE("TimingHandler::thResponseSeen");
  cancelAll();
}

void TimingHandler::cancelAll() {
DOTRACE("TimingHandler::cancelAll");
  itsTimeoutEvent.cancel();
  itsBeginEvent.cancel();
  itsAbortWaitEvent.cancel();
  itsDrawEvent.cancel();
  itsUndrawEvent.cancel();
}

static const char vcid_timinghandler_cc[] = "$Header$";
#endif // !TIMINGHANDLER_CC_DEFINED
