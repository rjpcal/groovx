///////////////////////////////////////////////////////////////////////
//
// trialevent.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Fri Jun 25 12:44:55 1999
// written: Wed Oct 13 15:04:40 1999
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

#include "demangle.h"
#include "exptdriver.h"
#include "timeutils.h"

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

  if (flag&TYPENAME) { os << demangle(typeid(*this).name()) << ' '; }
  os << itsRequestedDelay << endl;
  if (os.fail()) throw InputError(typeid(*this));
}

void TrialEvent::deserialize(istream& is, IOFlag flag) {
DOTRACE("TrialEvent::deserialize");

  if (flag&TYPENAME) { IO::readTypename(is, demangle(typeid(*this).name())); }
  DebugEval(demangle(typeid(*this).name()));
  is >> itsRequestedDelay;
  DebugEvalNL(itsRequestedDelay);
  if (is.fail()) throw InputError(typeid(*this));
}

int TrialEvent::charCount() const {
DOTRACE("TrialEvent::charCount");
  return ( demangle(typeid(*this).name()).length() + 1
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

  EventTraceNL(demangle(typeid(*event).name()));

#ifdef EVENT_TRACE
  cerr << "    before == " << elapsedMsecSince(event->itsBeginTime) << endl;
#endif

#ifdef LOCAL_DEBUG
  int msec = elapsedMsecSince(itsBeginTime);
  int error = event->itsRequestedDelay - msec;
  event->itsTotalAbsError += abs(error);
  event->itsTotalError += error;
#endif
  ++(event->itsInvokeCount);

  // Do the actual event callback.
  event->invoke();
  
#ifdef EVENT_TRACE
  cerr << "    after == " << elapsedMsecSince(event->itsBeginTime) << endl;
#endif
}

///////////////////////////////////////////////////////////////////////
//
// TrialEvent derived class overrides of invoke()
//
///////////////////////////////////////////////////////////////////////

void AbortTrialEvent::invoke() {
DOTRACE("AbortTrialEvent::invoke");
  getExptDriver().edAbortTrial();
}

void DrawEvent::invoke() {
DOTRACE("DrawEvent::invoke");
  getExptDriver().draw();
}

void EndTrialEvent::invoke() {
DOTRACE("EndTrialEvent::invoke");
  getExptDriver().edEndTrial();
}

void UndrawEvent::invoke() {
DOTRACE("UndrawEvent::invoke");
  getExptDriver().undraw();
}

void SwapBuffersEvent::invoke() {
DOTRACE("SwapBuffersEvent::invoke");
  getExptDriver().edSwapBuffers();
}

void RenderBackEvent::invoke() {
DOTRACE("RenderBackEvent::invoke");
  glDrawBuffer(GL_BACK);
}

void RenderFrontEvent::invoke() {
DOTRACE("RenderFrontEvent::invoke");
  glDrawBuffer(GL_FRONT);
}

void ClearBufferEvent::invoke() {
DOTRACE("ClearBufferEvent::invoke");
  glClear(GL_COLOR_BUFFER_BIT);
  glFlush();
}

static const char vcid_trialevent_cc[] = "$Header$";
#endif // !TRIALEVENT_CC_DEFINED
