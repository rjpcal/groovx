///////////////////////////////////////////////////////////////////////
//
// trialevent.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Fri Jun 25 12:44:55 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef TRIALEVENT_CC_DEFINED
#define TRIALEVENT_CC_DEFINED

#include "visx/trialevent.h"

#include "gfx/canvas.h"
#include "gfx/toglet.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/readutils.h"
#include "io/writer.h"
#include "io/writeutils.h"

#include "tcl/tclmain.h"

#include "util/algo.h"
#include "util/error.h"
#include "util/iter.h"
#include "util/log.h"
#include "util/ref.h"

#include "visx/trial.h"

#include "util/trace.h"
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// TrialEvent method definitions
//
///////////////////////////////////////////////////////////////////////

TrialEvent::TrialEvent(unsigned int msec) :
  itsTimer(msec, false),
  itsRequestedDelay(msec),
  itsTrial(0),
  itsEstimatedOffset(0.0),
  itsTotalOffset(0.0),
  itsTotalError(0.0),
  itsInvokeCount(0)
{
DOTRACE("TrialEvent::TrialEvent");

  itsTimer.sigTimeOut.connect(this, &TrialEvent::invokeTemplate);
}

TrialEvent::~TrialEvent() throw()
{
DOTRACE("TrialEvent::~TrialEvent");

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

unsigned int TrialEvent::schedule(Trial& trial, unsigned int minimum_msec)
{
DOTRACE("TrialEvent::schedule");

  // Remember the participants
  itsTrial = &trial;

  // Figure out the delay we want to request. If our requested delay is
  // zero, then just leave it as is, so that we get an immediate callback
  // from Tcl::Timer. Otherwise, adjust the request according to how much
  // error we expect based on past observations.

  unsigned int actual_request = 0;

  if (itsRequestedDelay > 0)
    actual_request =
      Util::max(int(itsRequestedDelay) + int(itsEstimatedOffset),
                int(minimum_msec));

  itsTimer.setDelayMsec(actual_request);
  itsTimer.schedule();

  return actual_request;
}

void TrialEvent::cancel()
{
DOTRACE("TrialEvent::cancel");
  itsTimer.cancel();
}

void TrialEvent::invokeTemplate()
{
DOTRACE("TrialEvent::invokeTemplate");

  const double msec = itsTimer.elapsedMsec();
  const double error = itsTimer.delayMsec() - msec;

  Util::Log::addObjScope(*this);

  Util::log( fstring("req ", itsRequestedDelay,
                     " - ", -itsEstimatedOffset) );

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

  Util::Log::removeObjScope(*this);
}

///////////////////////////////////////////////////////////////////////
//
// TrialEvent derived class overrides of invoke()
//
///////////////////////////////////////////////////////////////////////

AbortTrialEvent::AbortTrialEvent(unsigned int msec) : TrialEvent(msec) {}

AbortTrialEvent::~AbortTrialEvent() throw() {}

void AbortTrialEvent::invoke(Trial& trial)
{
DOTRACE("AbortTrialEvent::invoke");
  trial.trAbort();
}

DrawEvent::DrawEvent(unsigned int msec) : TrialEvent(msec) {}

DrawEvent::~DrawEvent() throw() {}

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

RenderEvent::RenderEvent(unsigned int msec) : TrialEvent(msec) {}

RenderEvent::~RenderEvent() throw() {}

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

EndTrialEvent::EndTrialEvent(unsigned int msec) : TrialEvent(msec) {}

EndTrialEvent::~EndTrialEvent() throw() {}

void EndTrialEvent::invoke(Trial& trial)
{
DOTRACE("EndTrialEvent::invoke");
  trial.trEndTrial();
}

NextNodeEvent::NextNodeEvent(unsigned int msec) : TrialEvent(msec) {}

NextNodeEvent::~NextNodeEvent() throw() {}

void NextNodeEvent::invoke(Trial& trial)
{
DOTRACE("NextNodeEvent::invoke");
  trial.trNextNode();
}

AllowResponsesEvent::AllowResponsesEvent(unsigned int msec) : TrialEvent(msec) {}

AllowResponsesEvent::~AllowResponsesEvent() throw() {}

void AllowResponsesEvent::invoke(Trial& trial)
{
DOTRACE("AllowResponsesEvent::invoke");
  trial.trAllowResponses();
}

DenyResponsesEvent::DenyResponsesEvent(unsigned int msec) : TrialEvent(msec) {}

DenyResponsesEvent::~DenyResponsesEvent() throw() {}

void DenyResponsesEvent::invoke(Trial& trial)
{
DOTRACE("DenyResponsesEvent::invoke");
  trial.trDenyResponses();
}

UndrawEvent::UndrawEvent(unsigned int msec) : TrialEvent(msec) {}

UndrawEvent::~UndrawEvent() throw() {}

void UndrawEvent::invoke(Trial& trial)
{
DOTRACE("UndrawEvent::invoke");
  Util::SoftRef<Toglet> widget = trial.getWidget();
  if (widget.isValid())
    widget->undraw();
}

SwapBuffersEvent::SwapBuffersEvent(unsigned int msec) : TrialEvent(msec) {}

SwapBuffersEvent::~SwapBuffersEvent() throw() {}

void SwapBuffersEvent::invoke(Trial& trial)
{
DOTRACE("SwapBuffersEvent::invoke");
  Util::SoftRef<Toglet> widget = trial.getWidget();
  if (widget.isValid())
    widget->swapBuffers();
}

RenderBackEvent::RenderBackEvent(unsigned int msec) : TrialEvent(msec) {}

RenderBackEvent::~RenderBackEvent() throw() {}

void RenderBackEvent::invoke(Trial& trial)
{
DOTRACE("RenderBackEvent::invoke");
  Util::SoftRef<Toglet> widget = trial.getWidget();
  if (widget.isValid())
    widget->getCanvas().drawOnBackBuffer();
}

RenderFrontEvent::RenderFrontEvent(unsigned int msec) : TrialEvent(msec) {}

RenderFrontEvent::~RenderFrontEvent() throw() {}

void RenderFrontEvent::invoke(Trial& trial)
{
DOTRACE("RenderFrontEvent::invoke");
  Util::SoftRef<Toglet> widget = trial.getWidget();
  if (widget.isValid())
    widget->getCanvas().drawOnFrontBuffer();
}

ClearBufferEvent::ClearBufferEvent(unsigned int msec) : TrialEvent(msec) {}

ClearBufferEvent::~ClearBufferEvent() throw() {}

void ClearBufferEvent::invoke(Trial& trial)
{
DOTRACE("ClearBufferEvent::invoke");
  Util::SoftRef<Toglet> widget = trial.getWidget();
  if (widget.isValid())
    widget->clearscreen();
}

FinishDrawingEvent::FinishDrawingEvent(unsigned int msec) : TrialEvent(msec) {}

FinishDrawingEvent::~FinishDrawingEvent() throw() {}

void FinishDrawingEvent::invoke(Trial& trial)
{
DOTRACE("FinishDrawingEvent::invoke");
  Util::SoftRef<Toglet> widget = trial.getWidget();
  if (widget.isValid())
    widget->getCanvas().finishDrawing();
}

GenericEvent::GenericEvent(unsigned int msec) :
  TrialEvent(msec),
  itsCallback(new Tcl::ProcWrapper(Tcl::Main::interp()))
{}

GenericEvent::~GenericEvent() throw() {}

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

MultiEvent::MultiEvent(unsigned int msec) :
  TrialEvent(msec),
  itsEvents()
{}

MultiEvent::~MultiEvent() throw() {}

void MultiEvent::invoke(Trial& trial)
{
  for (unsigned int i = 0; i < itsEvents.size(); ++i)
    {
      itsEvents[i]->setDelay(0);
      itsEvents[i]->schedule(trial);
    }
}

void MultiEvent::readFrom(IO::Reader* reader)
{
  minivec<Util::Ref<TrialEvent> > newEvents;

  IO::ReadUtils::readObjectSeq<TrialEvent>
    (reader, "events", std::back_inserter(newEvents));

  itsEvents.swap(newEvents);

  reader->readBaseClass("TrialEvent", IO::makeProxy<TrialEvent>(this));
}

void MultiEvent::writeTo(IO::Writer* writer) const
{
  IO::WriteUtils::writeObjectSeq(writer, "events",
                                 itsEvents.begin(),
                                 itsEvents.end());

  writer->writeBaseClass("TrialEvent", IO::makeConstProxy<TrialEvent>(this));
}

Util::FwdIter<const Util::Ref<TrialEvent> > MultiEvent::getEvents() const
{
DOTRACE("MultiEvent::getEvents");

  return Util::FwdIter<const Util::Ref<TrialEvent> >
    (itsEvents.begin(), itsEvents.end());
}

unsigned int MultiEvent::addEvent(Util::Ref<TrialEvent> event)
{
DOTRACE("MultiEvent::addEvent");
  itsEvents.push_back(event);
  Assert(itsEvents.size() >= 1);
  return itsEvents.size() - 1;
}

void MultiEvent::eraseEventAt(unsigned int index)
{
DOTRACE("MultiEvent::eraseEventAt");
  if (index >= itsEvents.size())
    throw Util::Error("index out of bounds");

  itsEvents.erase(itsEvents.begin() + index);
}

void MultiEvent::clearEvents()
{
DOTRACE("MultiEvent::clearEvents");
  itsEvents.clear();
}

static const char vcid_trialevent_cc[] = "$Header$";
#endif // !TRIALEVENT_CC_DEFINED
