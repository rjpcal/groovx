///////////////////////////////////////////////////////////////////////
//
// trialevent.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Fri Jun 25 12:44:55 1999
// written: Fri Jun 25 13:01:39 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRIALEVENT_CC_DEFINED
#define TRIALEVENT_CC_DEFINED

#include "trialevent.h"

#include <GL/gl.h>
#include <cmath>
#include <typeinfo>
#include <cstring>

#include "exptdriver.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

#define EVENT_TRACE

#ifdef EVENT_TRACE
#define EventTraceNL(type) cerr << type << endl;
#else
#define EventTraceNL(type) {}
#endif

namespace {
  ExptDriver& exptDriver = ExptDriver::theExptDriver();
}

///////////////////////////////////////////////////////////////////////
//
// TrialEvent method definitions
//
///////////////////////////////////////////////////////////////////////

TrialEvent::TrialEvent(int msec) :
  itsToken(NULL),
  itsRequestedDelay(msec),
  itsTotalError(0),
  itsTotalAbsError(0),
  itsInvokeCount(0)
{
DOTRACE("TrialEvent::TrialEvent");
}

TrialEvent::~TrialEvent() {
DOTRACE("TrialEvent::~TrialEvent");

  cancel();

  DebugEval(itsTotalError);
  DebugEval(itsTotalAbsError);
  DebugEval(itsInvokeCount);
  DebugEvalNL(itsInvokeCount ? itsTotalError/itsInvokeCount : 0);
}

void TrialEvent::serialize(ostream& os, IOFlag flag) const {
DOTRACE("TrialEvent::serialize");

  if (flag&TYPENAME) { os << typeid(*this).name() << ' '; }
  os << itsRequestedDelay << endl;
  if (os.fail()) throw InputError(typeid(*this));
}

void TrialEvent::deserialize(istream& is, IOFlag flag) {
DOTRACE("TrialEvent::deserialize");

  if (flag&TYPENAME) { IO::readTypename(is, typeid(*this).name()); }
  DebugEval(typeid(*this).name());
  is >> itsRequestedDelay;
  DebugEvalNL(itsRequestedDelay);
  if (is.fail()) throw InputError(typeid(*this));
}

int TrialEvent::charCount() const {
DOTRACE("TrialEvent::charCount");
  return (strlen(typeid(*this).name()) + 1
			 + gCharCount<int>(itsRequestedDelay) + 1
			 + 1); // fudge factor
}

void TrialEvent::schedule() {
DOTRACE("TrialEvent::schedule");
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

void TrialEvent::cancel() {
DOTRACE("TrialEvent::cancel");
  // Cancel the timer handler associated with itsToken. This is a safe
  // no-op if itsToken is NULL.
  Tcl_DeleteTimerHandler(itsToken);
}

ExptDriver& TrialEvent::getExptDriver() {
DOTRACE("TrialEvent::getExptDriver");
  DebugEvalNL((void *) &exptDriver);
  return exptDriver;
}

void TrialEvent::dummyInvoke(ClientData clientData) {
DOTRACE("TrialEvent::dummyInvoke");
  TrialEvent* event = static_cast<TrialEvent *>(clientData);

#ifdef LOCAL_DEBUG
  // Check the elapsed time and compare it to the requested elapsed
  // time.
  static timeval endTime, elapsedTime;
  gettimeofday(&endTime, NULL);

  elapsedTime.tv_sec = endTime.tv_sec - event->itsBeginTime.tv_sec;
  elapsedTime.tv_usec = endTime.tv_usec - event->itsBeginTime.tv_usec;

  int msec = int(double(elapsedTime.tv_sec)*1000.0 +
					  double(elapsedTime.tv_usec)/1000.0);
  int error = event->itsRequestedDelay - msec;
  event->itsTotalAbsError += abs(error);
  event->itsTotalError += error;
#endif
  ++(event->itsInvokeCount);

  // Do the actual event callback.
  event->invoke();
}

///////////////////////////////////////////////////////////////////////
//
// TrialEvent derived class overrides of invoke()
//
///////////////////////////////////////////////////////////////////////

void AbortTrialEvent::invoke() {
DOTRACE("AbortTrialEvent::invoke");
  EventTraceNL("AbortTrialEvent");
  getExptDriver().edAbortTrial();
}

void DrawEvent::invoke() {
DOTRACE("DrawEvent::invoke");
  EventTraceNL("DrawEvent");
  getExptDriver().draw();
}

void EndTrialEvent::invoke() {
DOTRACE("EndTrialEvent::invoke");
  EventTraceNL("EndTrialEvent");
  getExptDriver().edEndTrial();
}

void UndrawEvent::invoke() {
DOTRACE("UndrawEvent::invoke");
  EventTraceNL("UndrawEvent");
  getExptDriver().undraw();
}

void SwapBuffersEvent::invoke() {
DOTRACE("SwapBuffersEvent::invoke");
  EventTraceNL("SwapBuffersEvent");
  getExptDriver().edSwapBuffers();
}

void RenderBackEvent::invoke() {
DOTRACE("RenderBackEvent::invoke");
  EventTraceNL("RenderBackEvent");
  glDrawBuffer(GL_BACK);
}

void RenderFrontEvent::invoke() {
DOTRACE("RenderFrontEvent::invoke");
  EventTraceNL("RenderFrontEvent");
  glDrawBuffer(GL_FRONT);
}

void ClearBufferEvent::invoke() {
DOTRACE("ClearBufferEvent::invoke");
  EventTraceNL("ClearBufferEvent");
  glClear(GL_COLOR_BUFFER_BIT);
}

static const char vcid_trialevent_cc[] = "$Header$";
#endif // !TRIALEVENT_CC_DEFINED
