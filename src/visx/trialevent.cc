///////////////////////////////////////////////////////////////////////
//
// trialevent.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jun 25 12:44:55 1999
// written: Fri Jan 25 15:35:56 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRIALEVENT_CC_DEFINED
#define TRIALEVENT_CC_DEFINED

#include "visx/trialevent.h"

#include "visx/trialbase.h"

#include "gfx/canvas.h"

#include "gwt/widget.h"

#include "io/reader.h"
#include "io/writer.h"

#include "system/demangle.h"

#include "util/error.h"
#include "util/errorhandler.h"
#include "util/log.h"
#include "util/ref.h"

#include <tcl.h>
#include <typeinfo>

#include "util/trace.h"
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// TrialEvent method definitions
//
///////////////////////////////////////////////////////////////////////

TrialEvent::TrialEvent(int msec) :
  itsRequestedDelay(msec),
  itsToken(0),
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

void TrialEvent::schedule(TrialBase& trial,
                          Util::ErrorHandler& errhdlr)
{
DOTRACE("TrialEvent::schedule");
  // Cancel any possible previously pending invocation.
  cancel();

  // Note the time when the current scheduling request was made.
  itsTimer.restart();

  // Remember the participants
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

  fstring scopename(demangle_cstr(typeid(*this).name()), " ",
                    IO::IoObject::id());

  Util::Log::addScope(scopename);

  Util::log( fstring("ideal request  == ", itsRequestedDelay) );
  Util::log( fstring("estimated err  == ", itsEstimatedOffset) );
  Util::log( fstring("actual request == ", itsActualRequest) );

  double msec = itsTimer.elapsedMsec();
  double error = itsActualRequest - msec;
  itsTotalOffset += int(error);
  itsTotalError += (itsRequestedDelay - msec);

  ++itsInvokeCount;

  // Positive error means we expect the event to occur sooner than expected
  // Negative error means we expect the event to occur later than expected
  // (round towards negative infinity)
  itsEstimatedOffset =
    int(double(itsTotalOffset)/itsInvokeCount - 0.5);

  // Do the actual event callback.
  if ( itsErrorHandler != 0 && itsTrial != 0 )
    {
      try
        {
          invoke(*itsTrial);
        }
      catch (Util::Error& err)
        {
          itsErrorHandler->handleError(err);
        }
      catch (...)
        {
          itsErrorHandler->handleMsg(
            "an error of unknown type occured during a TrialEvent callback");
        }
    }

  Util::log( "event complete" );

  Util::Log::removeScope(scopename);
}

///////////////////////////////////////////////////////////////////////
//
// TrialEvent derived class overrides of invoke()
//
///////////////////////////////////////////////////////////////////////

AbortTrialEvent::AbortTrialEvent(int msec) : TrialEvent(msec) {}

AbortTrialEvent::~AbortTrialEvent() {}

void AbortTrialEvent::invoke(TrialBase& trial)
{
DOTRACE("AbortTrialEvent::invoke");
  trial.trAbortTrial();
}

DrawEvent::DrawEvent(int msec) : TrialEvent(msec) {}

DrawEvent::~DrawEvent() {}

void DrawEvent::invoke(TrialBase& trial)
{
DOTRACE("DrawEvent::invoke");
  Util::SoftRef<GWT::Widget> widget = trial.getWidget();
  if (widget.isValid())
    {
      trial.installSelf(widget);
      widget->setVisibility(true);
      widget->fullRender();
    }
}

RenderEvent::RenderEvent(int msec) : TrialEvent(msec) {}

RenderEvent::~RenderEvent() {}

void RenderEvent::invoke(TrialBase& trial)
{
DOTRACE("RenderEvent::invoke");
  Util::SoftRef<GWT::Widget> widget = trial.getWidget();
  if (widget.isValid())
    {
      trial.installSelf(widget);
      widget->setVisibility(true);
      widget->render();
    }
}

EndTrialEvent::EndTrialEvent(int msec) : TrialEvent(msec) {}

EndTrialEvent::~EndTrialEvent() {}

void EndTrialEvent::invoke(TrialBase& trial)
{
DOTRACE("EndTrialEvent::invoke");
  trial.trEndTrial();
  trial.trNextTrial();
}

NextNodeEvent::NextNodeEvent(int msec) : TrialEvent(msec) {}

NextNodeEvent::~NextNodeEvent() {}

void NextNodeEvent::invoke(TrialBase& trial)
{
DOTRACE("NextNodeEvent::invoke");
  trial.trNextNode();
}

AllowResponsesEvent::AllowResponsesEvent(int msec) : TrialEvent(msec) {}

AllowResponsesEvent::~AllowResponsesEvent() {}

void AllowResponsesEvent::invoke(TrialBase& trial)
{
DOTRACE("AllowResponsesEvent::invoke");
  trial.trAllowResponses();
}

DenyResponsesEvent::DenyResponsesEvent(int msec) : TrialEvent(msec) {}

DenyResponsesEvent::~DenyResponsesEvent() {}

void DenyResponsesEvent::invoke(TrialBase& trial)
{
DOTRACE("DenyResponsesEvent::invoke");
  trial.trDenyResponses();
}

UndrawEvent::UndrawEvent(int msec) : TrialEvent(msec) {}

UndrawEvent::~UndrawEvent() {}

void UndrawEvent::invoke(TrialBase& trial)
{
DOTRACE("UndrawEvent::invoke");
  Util::SoftRef<GWT::Widget> widget = trial.getWidget();
  if (widget.isValid())
    widget->undraw();
}

SwapBuffersEvent::SwapBuffersEvent(int msec) : TrialEvent(msec) {}

SwapBuffersEvent::~SwapBuffersEvent() {}

void SwapBuffersEvent::invoke(TrialBase& trial)
{
DOTRACE("SwapBuffersEvent::invoke");
  Util::SoftRef<GWT::Widget> widget = trial.getWidget();
  if (widget.isValid())
    widget->swapBuffers();
}

RenderBackEvent::RenderBackEvent(int msec) : TrialEvent(msec) {}

RenderBackEvent::~RenderBackEvent() {}

void RenderBackEvent::invoke(TrialBase& trial)
{
DOTRACE("RenderBackEvent::invoke");
  Util::SoftRef<GWT::Widget> widget = trial.getWidget();
  if (widget.isValid())
    widget->getCanvas().drawOnBackBuffer();
}

RenderFrontEvent::RenderFrontEvent(int msec) : TrialEvent(msec) {}

RenderFrontEvent::~RenderFrontEvent() {}

void RenderFrontEvent::invoke(TrialBase& trial)
{
DOTRACE("RenderFrontEvent::invoke");
  Util::SoftRef<GWT::Widget> widget = trial.getWidget();
  if (widget.isValid())
    widget->getCanvas().drawOnFrontBuffer();
}

ClearBufferEvent::ClearBufferEvent(int msec) : TrialEvent(msec) {}

ClearBufferEvent::~ClearBufferEvent() {}

void ClearBufferEvent::invoke(TrialBase& trial)
{
DOTRACE("ClearBufferEvent::invoke");
  Util::SoftRef<GWT::Widget> widget = trial.getWidget();
  if (widget.isValid())
    widget->clearscreen();
}

static const char vcid_trialevent_cc[] = "$Header$";
#endif // !TRIALEVENT_CC_DEFINED
