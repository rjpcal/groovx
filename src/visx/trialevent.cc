///////////////////////////////////////////////////////////////////////
//
// trialevent.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jun 25 12:44:55 1999
// written: Sat Jul 21 20:35:05 2001
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
#include "util/log.h"
#include "util/ref.h"

#include <tcl.h>
#include <typeinfo>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

#define EVENT_TRACE

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
  itsWidget(),
  itsErrorHandler(0),
  itsTrial(0),
  itsIsPending(false),
  itsTimer(),
  itsEstimatedOffset(0),
  itsActualRequest(msec),
  itsTotalOffset(0),
  itsTotalError(0),
  itsInvokeCount(0)
{
DOTRACE("TrialEvent::TrialEvent");
}

TrialEvent::~TrialEvent()
{
DOTRACE("TrialEvent::~TrialEvent");

  cancel();

  DebugEval(itsTotalOffset);
  DebugEval(itsTotalError);
  DebugEval(itsInvokeCount);
  double averageError =
    itsInvokeCount ? double(itsTotalError)/itsInvokeCount : 0.0;
  DebugEvalNL(averageError);
}

void TrialEvent::readFrom(IO::Reader* reader)
{
DOTRACE("TrialEvent::readFrom");

  cancel(); // cancel since the event is changing state

  reader->readValue("requestedDelay", itsRequestedDelay);
}

void TrialEvent::writeTo(IO::Writer* writer) const
{
DOTRACE("TrialEvent::writeTo");

  writer->writeValue("requestedDelay", itsRequestedDelay);
}

void TrialEvent::schedule(Util::WeakRef<GWT::Widget> widget,
                          Util::ErrorHandler& errhdlr,
                          TrialBase& trial)
{
DOTRACE("TrialEvent::schedule");
  // Cancel any possible previously pending invocation.
  cancel();

  // Note the time when the current scheduling request was made.
  itsTimer.restart();

  // Remember the participants
  itsWidget = widget;
  itsErrorHandler = &errhdlr;
  itsTrial = &trial;

  // If the requested time is zero or negative - i.e., immediate,
  // don't bother creating a timer handler. Instead, generate a direct
  // invocation.
  if (itsRequestedDelay <= 0)
    {
      itsIsPending = true;
      invokeTemplate();
    }
  // Otherwise, set up a timer that will call the invocation after the
  // specified amount of time.
  else
    {
      itsActualRequest = itsRequestedDelay + itsEstimatedOffset;
      itsToken = Tcl_CreateTimerHandler(itsActualRequest,
                                        dummyInvoke,
                                        static_cast<ClientData>(this));
      itsIsPending = true;
    }
}

void TrialEvent::cancel()
{
DOTRACE("TrialEvent::cancel");
  // Cancel the timer handler associated with itsToken. This is a safe
  // no-op if itsToken is NULL.
  Tcl_DeleteTimerHandler(itsToken);

  itsIsPending = false;
  itsToken = 0;
}

void TrialEvent::dummyInvoke(ClientData clientData)
{
  TrialEvent* event = static_cast<TrialEvent *>(clientData);

  Assert(event != 0);

  event->invokeTemplate();
}

void TrialEvent::invokeTemplate()
{
DOTRACE("TrialEvent::invokeTemplate");

  itsIsPending = false;
  itsToken = 0;

  int msec = itsTimer.elapsedMsec();
  int error = itsActualRequest - msec;

#ifdef EVENT_TRACE
  Util::log() << demangle_cstr(typeid(*this).name()) << ' '
              << IO::IoObject::id() << '\n';

  Util::log() << "    request delay == " << itsRequestedDelay << '\n';
  Util::log() << "    est offset == " << itsEstimatedOffset << '\n';
  Util::log() << "    actual request == " << itsActualRequest << '\n';
  Util::log() << "    invoke count == " << itsInvokeCount << '\n';
  Util::log() << "    total offset == " << itsTotalOffset << '\n';
  Util::log() << "    before == " << msec << '\n';
#endif

  itsTotalOffset += error;
  itsTotalError += (itsRequestedDelay - msec);

  ++itsInvokeCount;

  // Positive error means we expect the event to occur sooner than expected
  // Negative error means we expect the event to occur later than expected
  // (round towards negative infinity)
  itsEstimatedOffset =
    int(double(itsTotalOffset)/itsInvokeCount - 0.5);

  // Do the actual event callback.
  if ( itsWidget.isValid() && itsErrorHandler != 0 && itsTrial != 0 )
    {
      try
        {
          invoke(itsWidget, *itsTrial);
        }
      catch (ErrorWithMsg& err)
        {
          itsErrorHandler->handleErrorWithMsg(err);
        }
      catch (...)
        {
          itsErrorHandler->handleMsg(
            "an error of unknown type occured during a TrialEvent callback");
        }
    }

#ifdef EVENT_TRACE
  Util::log() << "    after == " << itsTimer.elapsedMsec() << '\n';
#endif
}

///////////////////////////////////////////////////////////////////////
//
// TrialEvent derived class overrides of invoke()
//
///////////////////////////////////////////////////////////////////////

AbortTrialEvent::AbortTrialEvent(int msec) : TrialEvent(msec) {}

AbortTrialEvent::~AbortTrialEvent() {}

void AbortTrialEvent::invoke(Util::WeakRef<GWT::Widget>, TrialBase& trial)
{
DOTRACE("AbortTrialEvent::invoke");
  trial.trAbortTrial();
}

DrawEvent::DrawEvent(int msec) : TrialEvent(msec) {}

DrawEvent::~DrawEvent() {}

void DrawEvent::invoke(Util::WeakRef<GWT::Widget> widget, TrialBase& trial)
{
DOTRACE("DrawEvent::invoke");
  trial.installSelf(widget);
  widget->setVisibility(true);
  widget->display();
}

EndTrialEvent::EndTrialEvent(int msec) : TrialEvent(msec) {}

EndTrialEvent::~EndTrialEvent() {}

void EndTrialEvent::invoke(Util::WeakRef<GWT::Widget>, TrialBase& trial)
{
DOTRACE("EndTrialEvent::invoke");
  trial.trEndTrial();
  trial.trNextTrial();
}

NextNodeEvent::NextNodeEvent(int msec) : TrialEvent(msec) {}

NextNodeEvent::~NextNodeEvent() {}

void NextNodeEvent::invoke(Util::WeakRef<GWT::Widget>, TrialBase& trial)
{
DOTRACE("NextNodeEvent::invoke");
  trial.trNextNode();
}

AllowResponsesEvent::AllowResponsesEvent(int msec) : TrialEvent(msec) {}

AllowResponsesEvent::~AllowResponsesEvent() {}

void AllowResponsesEvent::invoke(Util::WeakRef<GWT::Widget>, TrialBase& trial)
{
DOTRACE("AllowResponsesEvent::invoke");
  trial.trAllowResponses();
}

DenyResponsesEvent::DenyResponsesEvent(int msec) : TrialEvent(msec) {}

DenyResponsesEvent::~DenyResponsesEvent() {}

void DenyResponsesEvent::invoke(Util::WeakRef<GWT::Widget>, TrialBase& trial)
{
DOTRACE("DenyResponsesEvent::invoke");
  trial.trDenyResponses();
}

UndrawEvent::UndrawEvent(int msec) : TrialEvent(msec) {}

UndrawEvent::~UndrawEvent() {}

void UndrawEvent::invoke(Util::WeakRef<GWT::Widget> widget, TrialBase&)
{
DOTRACE("UndrawEvent::invoke");
  widget->undraw();
}

SwapBuffersEvent::SwapBuffersEvent(int msec) : TrialEvent(msec) {}

SwapBuffersEvent::~SwapBuffersEvent() {}

void SwapBuffersEvent::invoke(Util::WeakRef<GWT::Widget> widget, TrialBase&)
{
DOTRACE("SwapBuffersEvent::invoke");
  widget->swapBuffers();
}

RenderBackEvent::RenderBackEvent(int msec) : TrialEvent(msec) {}

RenderBackEvent::~RenderBackEvent() {}

void RenderBackEvent::invoke(Util::WeakRef<GWT::Widget> widget, TrialBase&)
{
DOTRACE("RenderBackEvent::invoke");
  widget->getCanvas().drawOnBackBuffer();
}

RenderFrontEvent::RenderFrontEvent(int msec) : TrialEvent(msec) {}

RenderFrontEvent::~RenderFrontEvent() {}

void RenderFrontEvent::invoke(Util::WeakRef<GWT::Widget> widget, TrialBase&)
{
DOTRACE("RenderFrontEvent::invoke");
  widget->getCanvas().drawOnFrontBuffer();
}

ClearBufferEvent::ClearBufferEvent(int msec) : TrialEvent(msec) {}

ClearBufferEvent::~ClearBufferEvent() {}

void ClearBufferEvent::invoke(Util::WeakRef<GWT::Widget> widget, TrialBase&)
{
DOTRACE("ClearBufferEvent::invoke");
  widget->clearscreen();
}

static const char vcid_trialevent_cc[] = "$Header$";
#endif // !TRIALEVENT_CC_DEFINED
