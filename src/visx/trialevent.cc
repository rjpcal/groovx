///////////////////////////////////////////////////////////////////////
//
// trialevent.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jun 25 12:44:55 1999
// written: Thu Dec  5 15:27:14 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRIALEVENT_CC_DEFINED
#define TRIALEVENT_CC_DEFINED

#include "visx/trialevent.h"

#include "visx/trial.h"

#include "gfx/canvas.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include "system/demangle.h"

#include "tcl/tclmain.h"
#include "tcl/toglet.h"

#include "util/algo.h"
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
  itsEstimatedOffset(0.0),
  itsActualRequest(msec),
  itsTotalOffset(0.0),
  itsTotalError(0.0),
  itsInvokeCount(0)
{
DOTRACE("TrialEvent::TrialEvent");
}

TrialEvent::~TrialEvent()
{
DOTRACE("TrialEvent::~TrialEvent");

  cancel();

  dbgEval(3, itsTotalOffset);
  dbgEval(3, itsTotalError);
  dbgEval(3, itsInvokeCount);
  double averageError =
    itsInvokeCount ? itsTotalError/itsInvokeCount : 0.0;
  dbgEvalNL(3, averageError);
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

int TrialEvent::schedule(Trial& trial,
                         Util::ErrorHandler& errhdlr,
                         int minimum_msec)
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
      itsActualRequest = 0;
      invokeTemplate();
    }
  // Otherwise, set up a timer that will call the invocation after the
  // specified amount of time.
  else
    {
      itsActualRequest =
        Util::max(itsRequestedDelay + (int)itsEstimatedOffset,
                  Util::max(minimum_msec, 0));
      itsToken = Tcl_CreateTimerHandler(itsActualRequest,
                                        dummyInvoke,
                                        static_cast<ClientData>(this));
      itsIsPending = true;
    }

  return itsActualRequest;
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

  Util::ErrorHandler* handler = event->itsErrorHandler;

  try
    {
      event->invokeTemplate();
    }
  catch (Util::Error& err)
    {
      if (handler != 0) handler->handleError(err);
    }
  catch (...)
    {
      if (handler != 0)
        handler->handleMsg
          ("an error of unknown type occured during a TrialEvent callback");
    }
}

void TrialEvent::invokeTemplate()
{
DOTRACE("TrialEvent::invokeTemplate");

  itsIsPending = false;
  itsToken = 0;

  const double msec = itsTimer.elapsedMsec();
  const double error = itsActualRequest - msec;

  fstring scopename(demangle_cstr(typeid(*this).name()), " ",
                    IO::IoObject::id());

  Util::Log::addScope(scopename);

  Util::log( fstring("req ", itsRequestedDelay, " - ", -itsEstimatedOffset) );

  itsTotalOffset += error;
  itsTotalError += (itsRequestedDelay - msec);

  ++itsInvokeCount;

  // Positive error means we expect the event to occur sooner than expected
  // Negative error means we expect the event to occur later than expected
  // (round towards negative infinity)
  const double moving_average_ratio = 1.0 / Util::min(10, itsInvokeCount);
  itsEstimatedOffset =
    (1.0 - moving_average_ratio) * itsEstimatedOffset +
           moving_average_ratio  * error;

  // Do the actual event callback.
  if ( itsTrial != 0 )
    {
      invoke(*itsTrial);
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

void AbortTrialEvent::invoke(Trial& trial)
{
DOTRACE("AbortTrialEvent::invoke");
  trial.vxAbort();
}

DrawEvent::DrawEvent(int msec) : TrialEvent(msec) {}

DrawEvent::~DrawEvent() {}

void DrawEvent::invoke(Trial& trial)
{
DOTRACE("DrawEvent::invoke");
  Util::SoftRef<Toglet> widget = trial.getWidget();
  if (widget.isValid())
    {
      trial.installSelf(widget);
      widget->setVisibility(true);
      widget->fullRender();
    }
}

RenderEvent::RenderEvent(int msec) : TrialEvent(msec) {}

RenderEvent::~RenderEvent() {}

void RenderEvent::invoke(Trial& trial)
{
DOTRACE("RenderEvent::invoke");
  Util::SoftRef<Toglet> widget = trial.getWidget();
  if (widget.isValid())
    {
      trial.installSelf(widget);
      widget->setVisibility(true);
      widget->render();
    }
}

EndTrialEvent::EndTrialEvent(int msec) : TrialEvent(msec) {}

EndTrialEvent::~EndTrialEvent() {}

void EndTrialEvent::invoke(Trial& trial)
{
DOTRACE("EndTrialEvent::invoke");
  trial.vxNext();
}

NextNodeEvent::NextNodeEvent(int msec) : TrialEvent(msec) {}

NextNodeEvent::~NextNodeEvent() {}

void NextNodeEvent::invoke(Trial& trial)
{
DOTRACE("NextNodeEvent::invoke");
  trial.trNextNode();
}

AllowResponsesEvent::AllowResponsesEvent(int msec) : TrialEvent(msec) {}

AllowResponsesEvent::~AllowResponsesEvent() {}

void AllowResponsesEvent::invoke(Trial& trial)
{
DOTRACE("AllowResponsesEvent::invoke");
  trial.trAllowResponses();
}

DenyResponsesEvent::DenyResponsesEvent(int msec) : TrialEvent(msec) {}

DenyResponsesEvent::~DenyResponsesEvent() {}

void DenyResponsesEvent::invoke(Trial& trial)
{
DOTRACE("DenyResponsesEvent::invoke");
  trial.trDenyResponses();
}

UndrawEvent::UndrawEvent(int msec) : TrialEvent(msec) {}

UndrawEvent::~UndrawEvent() {}

void UndrawEvent::invoke(Trial& trial)
{
DOTRACE("UndrawEvent::invoke");
  Util::SoftRef<Toglet> widget = trial.getWidget();
  if (widget.isValid())
    widget->undraw();
}

SwapBuffersEvent::SwapBuffersEvent(int msec) : TrialEvent(msec) {}

SwapBuffersEvent::~SwapBuffersEvent() {}

void SwapBuffersEvent::invoke(Trial& trial)
{
DOTRACE("SwapBuffersEvent::invoke");
  Util::SoftRef<Toglet> widget = trial.getWidget();
  if (widget.isValid())
    widget->swapBuffers();
}

RenderBackEvent::RenderBackEvent(int msec) : TrialEvent(msec) {}

RenderBackEvent::~RenderBackEvent() {}

void RenderBackEvent::invoke(Trial& trial)
{
DOTRACE("RenderBackEvent::invoke");
  Util::SoftRef<Toglet> widget = trial.getWidget();
  if (widget.isValid())
    widget->getCanvas().drawOnBackBuffer();
}

RenderFrontEvent::RenderFrontEvent(int msec) : TrialEvent(msec) {}

RenderFrontEvent::~RenderFrontEvent() {}

void RenderFrontEvent::invoke(Trial& trial)
{
DOTRACE("RenderFrontEvent::invoke");
  Util::SoftRef<Toglet> widget = trial.getWidget();
  if (widget.isValid())
    widget->getCanvas().drawOnFrontBuffer();
}

ClearBufferEvent::ClearBufferEvent(int msec) : TrialEvent(msec) {}

ClearBufferEvent::~ClearBufferEvent() {}

void ClearBufferEvent::invoke(Trial& trial)
{
DOTRACE("ClearBufferEvent::invoke");
  Util::SoftRef<Toglet> widget = trial.getWidget();
  if (widget.isValid())
    widget->clearscreen();
}

GenericEvent::GenericEvent(int msec) :
  TrialEvent(msec),
  itsCallback(new Tcl::ProcWrapper(Tcl::Main::safeInterp()))
{}

GenericEvent::~GenericEvent() {}

void GenericEvent::readFrom(IO::Reader* reader)
{
  reader->readOwnedObject("callback", itsCallback);

  reader->readBaseClass("TrialEvent", IO::makeProxy<TrialEvent>(this));
}

void GenericEvent::writeTo(IO::Writer* writer) const
{
  writer->writeOwnedObject("callback", itsCallback);

  writer->writeBaseClass("TrialEvent", IO::makeConstProxy<TrialEvent>(this));
}

fstring GenericEvent::getCallback() const
{
  return itsCallback->fullSpec();
}

void GenericEvent::setCallback(const fstring& script)
{
  itsCallback->define("", script);
}

void GenericEvent::invoke(Trial& /*trial*/)
{
  itsCallback->invoke("");
}

static const char vcid_trialevent_cc[] = "$Header$";
#endif // !TRIALEVENT_CC_DEFINED
