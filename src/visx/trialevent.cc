///////////////////////////////////////////////////////////////////////
//
// trialevent.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Fri Jun 25 12:44:55 1999
// written: Thu May 11 19:38:00 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRIALEVENT_CC_DEFINED
#define TRIALEVENT_CC_DEFINED

#include "trialevent.h"

#include "trialbase.h"

#include "io/reader.h"
#include "io/writer.h"

#include "gwt/canvas.h"
#include "gwt/widget.h"

#include "system/demangle.h"

#include "util/error.h"
#include "util/errorhandler.h"

#include <tcl.h>
#include <cmath>
#include <cstring>
#include <iostream.h>
#include <typeinfo>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

#define EVENT_TRACE

#ifdef EVENT_TRACE
#define EventTraceNL(type) cerr << type << endl;
#else
#define EventTraceNL(type) {}
#endif

class TrialEventError : public ErrorWithMsg {
public:
  TrialEventError(const char* msg = "") : ErrorWithMsg(msg) {}
};

///////////////////////////////////////////////////////////////////////
//
// TrialEvent method definitions
//
///////////////////////////////////////////////////////////////////////

TrialEvent::TrialEvent(int msec) :
  itsRequestedDelay(msec),
  itsToken(0),
  itsWidget(0),
  itsErrorHandler(0),
  itsTrial(0),
  itsIsPending(false),
  itsTimer(),
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

void TrialEvent::serialize(ostream& os, IO::IOFlag flag) const {
DOTRACE("TrialEvent::serialize");

  if (flag&IO::TYPENAME)
	 {
		os << demangle_cstr(typeid(*this).name()) << ' ';
	 }
  os << itsRequestedDelay << endl;
  if (os.fail()) throw IO::InputError(typeid(*this));
}

void TrialEvent::deserialize(istream& is, IO::IOFlag flag) {
DOTRACE("TrialEvent::deserialize");

  cancel(); // cancel since the event is changing state

  if (flag&IO::TYPENAME)
	 {
		IO::IoObject::readTypename(is, demangle_cstr(typeid(*this).name()));
	 }
  is >> itsRequestedDelay;
  if (is.fail()) throw IO::InputError(typeid(*this));
}

int TrialEvent::charCount() const {
DOTRACE("TrialEvent::charCount");
  return ( strlen(demangle_cstr(typeid(*this).name())) + 1
			 + IO::gCharCount<int>(itsRequestedDelay) + 1
			 + 1); // fudge factor
}

void TrialEvent::readFrom(IO::Reader* reader) {
DOTRACE("TrialEvent::readFrom");

  cancel(); // cancel since the event is changing state

  reader->readValue("requestedDelay", itsRequestedDelay);
}

void TrialEvent::writeTo(IO::Writer* writer) const {
DOTRACE("TrialEvent::writeTo");

  writer->writeValue("requestedDelay", itsRequestedDelay);
}

void TrialEvent::schedule(GWT::Widget& widget,
								  Util::ErrorHandler& errhdlr,
								  TrialBase& trial) {
DOTRACE("TrialEvent::schedule");
  itsWidget = &widget;
  itsErrorHandler = &errhdlr;
  itsTrial = &trial;

  // Cancel any possible previously pending invocation.
  cancel();

  // Note the time when the current scheduling request was made.
  itsTimer.restart();

  // If the requested time is zero or negative - i.e., immediate,
  // don't bother creating a timer handler. Instead, generate a direct
  // invocation.
  if (itsRequestedDelay <= 0) {
	 itsIsPending = true;
	 dummyInvoke(static_cast<ClientData>(this));
  }
  // Otherwise, set up a timer that will call the invocation after the
  // specified amount of time.
  else {
	 itsToken = Tcl_CreateTimerHandler(itsRequestedDelay, dummyInvoke, 
												  static_cast<ClientData>(this));
	 itsIsPending = true;
  }
}

void TrialEvent::cancel() {
DOTRACE("TrialEvent::cancel");
  // Cancel the timer handler associated with itsToken. This is a safe
  // no-op if itsToken is NULL.
  Tcl_DeleteTimerHandler(itsToken);

  itsIsPending = false;
  itsToken = 0;
}

GWT::Widget& TrialEvent::getWidget() {
DOTRACE("TrialEvent::getWidget");
  DebugEvalNL((void *) itsWidget);
  if (itsWidget == 0) {
	 throw TrialEventError("TrialEvent::itsWidget is NULL");
  }
  return *itsWidget;
}

Util::ErrorHandler& TrialEvent::getErrorHandler() {
DOTRACE("TrialEvent::getErrorHandler");
  DebugEvalNL((void *) itsErrorHandler);
  if (itsErrorHandler == 0) {
	 throw TrialEventError("TrialEvent::itsErrorHandler is NULL");
  }
  return *itsErrorHandler;
}

TrialBase& TrialEvent::getTrial() {
DOTRACE("TrialEvent::getTrial");
  DebugEvalNL((void *) itsTrial);
  if (itsTrial == 0) {
	 throw TrialEventError("TrialEvent::itsTrial is NULL");
  }
  return *itsTrial;
}

void TrialEvent::dummyInvoke(ClientData clientData) {
DOTRACE("TrialEvent::dummyInvoke");
  TrialEvent* event = static_cast<TrialEvent *>(clientData);

  event->itsIsPending = false;
  event->itsToken = 0;

  EventTraceNL(demangle_cstr(typeid(*event).name()));

#ifdef EVENT_TRACE
  cerr << "    before == " << event->itsTimer.elapsedMsec() << endl;
#endif

#ifdef LOCAL_DEBUG
  int msec = event->itsTimer.elapsedMsec();
  int error = event->itsRequestedDelay - msec;
  event->itsTotalAbsError += abs(error);
  event->itsTotalError += error;
#endif
  ++(event->itsInvokeCount);

  // Do the actual event callback.
  try
	 {
		Util::ErrorHandler& errhdlr = event->getErrorHandler();

		try
		  {
			 event->invoke();
		  }
		catch (ErrorWithMsg& err)
		  {
			 errhdlr.handleErrorWithMsg(err);
		  }
		catch (...)
		  {
			 errhdlr.handleMsg(
				"an error of unknown type occured during a TrialEvent callback");
		  }
	 }
  catch(...)
	 {
	 }

#ifdef EVENT_TRACE
  cerr << "    after == " << event->itsTimer.elapsedMsec() << endl;
#endif
}

///////////////////////////////////////////////////////////////////////
//
// TrialEvent derived class overrides of invoke()
//
///////////////////////////////////////////////////////////////////////

void AbortTrialEvent::invoke() {
DOTRACE("AbortTrialEvent::invoke");
  getTrial().trAbortTrial();
}

void DrawEvent::invoke() {
DOTRACE("DrawEvent::invoke");
  getTrial().trDrawTrial();
}

void EndTrialEvent::invoke() {
DOTRACE("EndTrialEvent::invoke");
  getTrial().trEndTrial();
  getTrial().trNextTrial();
}

void UndrawEvent::invoke() {
DOTRACE("UndrawEvent::invoke");
  getTrial().trUndrawTrial();
}

void SwapBuffersEvent::invoke() {
DOTRACE("SwapBuffersEvent::invoke");
  getWidget().swapBuffers();
}

void RenderBackEvent::invoke() {
DOTRACE("RenderBackEvent::invoke");
  getWidget().getCanvas()->drawOnBackBuffer();
}

void RenderFrontEvent::invoke() {
DOTRACE("RenderFrontEvent::invoke");
  getWidget().getCanvas()->drawOnFrontBuffer();
}

void ClearBufferEvent::invoke() {
DOTRACE("ClearBufferEvent::invoke");
  GWT::Canvas* canvas = getWidget().getCanvas();
  canvas->clearColorBuffer();
  canvas->flushOutput();
}

static const char vcid_trialevent_cc[] = "$Header$";
#endif // !TRIALEVENT_CC_DEFINED
