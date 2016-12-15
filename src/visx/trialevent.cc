/** @file visx/trialevent.cc events to be scheduled during a Trial */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Fri Jun 25 12:44:55 1999
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#include "visx/trialevent.h"

#include "io/ioproxy.h"
#include "io/outputfile.h"
#include "io/reader.h"
#include "io/readutils.h"
#include "io/writer.h"
#include "io/writeutils.h"

#include "nub/log.h"
#include "nub/ref.h"

#include "tcl/eventloop.h"
#include "tcl/timerscheduler.h"

#include "rutz/error.h"
#include "rutz/iter.h"
#include "rutz/sfmt.h"

#include "visx/trial.h"

#include <fstream>
#include <memory>
#include <utility>

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

using rutz::fstring;

///////////////////////////////////////////////////////////////////////
//
// TrialEvent method definitions
//
///////////////////////////////////////////////////////////////////////

TrialEvent::TrialEvent(unsigned int msec) :
  itsTimer(msec, false),
  itsRequestedDelay(msec),
  itsTrial(nullptr),
  itsEstimatedOffset(0.0),
  itsTotalOffset(0.0),
  itsTotalError(0.0),
  itsInvokeCount(0)
{
GVX_TRACE("TrialEvent::TrialEvent");

  itsTimer.sig_timeout.connect(this, &TrialEvent::invokeTemplate);
}

TrialEvent::~TrialEvent() noexcept
{
GVX_TRACE("TrialEvent::~TrialEvent");

  dbg_eval(3, itsTotalOffset);
  dbg_eval(3, itsTotalError);
  dbg_eval(3, itsInvokeCount);
  double averageError =
    itsInvokeCount ? itsTotalError/itsInvokeCount : 0.0;
  dbg_eval_nl(3, averageError);
}

void TrialEvent::read_from(io::reader& reader)
{
GVX_TRACE("TrialEvent::read_from");

  cancel(); // cancel since the event is changing state

  reader.read_value("requestedDelay", itsRequestedDelay);
}

void TrialEvent::write_to(io::writer& writer) const
{
GVX_TRACE("TrialEvent::write_to");

  writer.write_value("requestedDelay", itsRequestedDelay);
}

unsigned int TrialEvent::schedule(std::shared_ptr<nub::scheduler> s,
                                  Trial& trial,
                                  unsigned int minimum_msec)
{
GVX_TRACE("TrialEvent::schedule");

  // Remember the participants
  itsTrial = &trial;

  // Figure out the delay we want to request. If our requested delay
  // is zero, then just leave it as is, so that we get an immediate
  // callback from the scheduler. Otherwise, adjust the request
  // according to how much error we expect based on past observations.

  unsigned int actual_request = 0;

  if (itsRequestedDelay > 0)
    actual_request =
      std::max(itsRequestedDelay + (unsigned int)(itsEstimatedOffset),
                minimum_msec);

  itsTimer.set_delay_msec(actual_request);
  itsTimer.schedule(s);

  return actual_request;
}

void TrialEvent::cancel()
{
GVX_TRACE("TrialEvent::cancel");
  itsTimer.cancel();
}

void TrialEvent::invokeTemplate()
{
GVX_TRACE("TrialEvent::invokeTemplate");

  const double msec = itsTimer.elapsed_msec();
  const double error = itsTimer.delay_msec() - msec;

  nub::logging::add_obj_scope(*this);

  nub::log( rutz::sfmt("req %u - %f",
                       itsRequestedDelay, -itsEstimatedOffset) );

  itsTotalOffset += error;
  itsTotalError += (itsRequestedDelay - msec);

  ++itsInvokeCount;

  // Positive error: we expect the event to occur sooner than requested
  // Negative error: we expect the event to occur later than requested
  // (round towards negative infinity)
  const double moving_average_ratio =
    1.0 / std::min(10, itsInvokeCount);

  itsEstimatedOffset =
    (1.0 - moving_average_ratio) * itsEstimatedOffset +
    moving_average_ratio  * error;

  // Do the actual event callback.
  if ( itsTrial != nullptr )
    {
      invoke(*itsTrial);
    }

  nub::logging::remove_obj_scope(*this);
}

//---------------------------------------------------------------------
//
// NullTrialEvent
//
//---------------------------------------------------------------------

NullTrialEvent::NullTrialEvent(unsigned int msec) : TrialEvent(msec) {}

NullTrialEvent::~NullTrialEvent() noexcept {}

void NullTrialEvent::invoke(Trial&) {}

//---------------------------------------------------------------------
//
// TrialMemFuncEvent
//
//---------------------------------------------------------------------

fstring TrialMemFuncEvent::obj_typename() const
{
  return itsTypename;
}

TrialMemFuncEvent* TrialMemFuncEvent::make(CallbackType callback,
                                           const fstring& type,
                                           unsigned int msec)
{
  return new TrialMemFuncEvent(callback, type, msec);
}

TrialMemFuncEvent::TrialMemFuncEvent(CallbackType callback,
                                     const fstring& type,
                                     unsigned int msec) :
  TrialEvent(msec),
  itsCallback(callback),
  itsTypename(type)
{}

TrialMemFuncEvent::~TrialMemFuncEvent() noexcept {}

void TrialMemFuncEvent::invoke(Trial& trial)
{
GVX_TRACE("TrialMemFuncEvent::invoke");
  (trial.*itsCallback)();
}

#define MAKE_EVENT(EventName)                           \
TrialEvent* make##EventName##Event()                    \
{                                                       \
  return TrialMemFuncEvent::make(&Trial::tr##EventName, \
                                 #EventName "Event");   \
}

MAKE_EVENT(AbortTrial);
MAKE_EVENT(Draw);
MAKE_EVENT(Render);
MAKE_EVENT(EndTrial);
MAKE_EVENT(NextNode);
MAKE_EVENT(AllowResponses);
MAKE_EVENT(DenyResponses);
MAKE_EVENT(Undraw);
MAKE_EVENT(RenderBack);
MAKE_EVENT(RenderFront);
MAKE_EVENT(SwapBuffers);
MAKE_EVENT(ClearBuffer);
MAKE_EVENT(FinishDrawing);

#undef MAKE_EVENT

//---------------------------------------------------------------------
//
// FileWriteEvent
//
//---------------------------------------------------------------------

FileWriteEvent::FileWriteEvent(unsigned int msec) :
  TrialEvent(msec),
  itsFile(output_file::make())
{}

FileWriteEvent::~FileWriteEvent() noexcept {}

void FileWriteEvent::invoke(Trial& /*trial*/)
{
  if (itsFile->has_stream())
    {
      itsFile->stream().put(static_cast<char>(itsByte));
      itsFile->stream().flush();
      nub::log( rutz::sfmt("wrote '%d' to '%s'",
                           itsByte, itsFile->get_filename().c_str()) );
    }
}

void FileWriteEvent::read_from(io::reader& reader)
{
  itsFile = dyn_cast<output_file>(reader.read_object("file"));
  reader.read_value("byte", itsByte);

  reader.read_base_class("TrialEvent",
                       io::make_proxy<TrialEvent>(this));
}

void FileWriteEvent::write_to(io::writer& writer) const
{
  writer.write_object("file", itsFile);
  writer.write_value("byte", itsByte);

  writer.write_base_class("TrialEvent",
                        io::make_const_proxy<TrialEvent>(this));
}

int FileWriteEvent::getByte() const
{
  return itsByte;
}

void FileWriteEvent::setByte(int b)
{
  itsByte = b;
}

nub::ref<output_file> FileWriteEvent::getFile() const
{
  return itsFile;
}

void FileWriteEvent::setFile(nub::ref<output_file> file)
{
  itsFile = file;
}

//---------------------------------------------------------------------
//
// GenericEvent
//
//---------------------------------------------------------------------

GenericEvent::GenericEvent(unsigned int msec) :
  TrialEvent(msec),
  itsCallback(new tcl::ProcWrapper(tcl::event_loop::interp()))
{}

GenericEvent::~GenericEvent() noexcept {}

void GenericEvent::invoke(Trial& /*trial*/)
{
  itsCallback->invoke("");
}

void GenericEvent::read_from(io::reader& reader)
{
  reader.read_owned_object("callback", itsCallback);

  reader.read_base_class("TrialEvent",
                       io::make_proxy<TrialEvent>(this));
}

void GenericEvent::write_to(io::writer& writer) const
{
  writer.write_owned_object("callback", itsCallback);

  writer.write_base_class("TrialEvent",
                        io::make_const_proxy<TrialEvent>(this));
}

fstring GenericEvent::getCallback() const
{
  return itsCallback->fullSpec();
}

void GenericEvent::setCallback(const fstring& script)
{
  itsCallback->define("", script);
}

//---------------------------------------------------------------------
//
// MultiEvent
//
//---------------------------------------------------------------------

MultiEvent::MultiEvent(unsigned int msec) :
  TrialEvent(msec),
  itsEvents()
{}

MultiEvent::~MultiEvent() noexcept {}

void MultiEvent::invoke(Trial& trial)
{
  for (unsigned int i = 0; i < itsEvents.size(); ++i)
    {
      itsEvents[i]->setDelay(0);
      itsEvents[i]->invoke(trial);
    }
}

void MultiEvent::read_from(io::reader& reader)
{
  std::vector<nub::ref<TrialEvent> > newEvents;

  io::read_utils::read_object_seq<TrialEvent>
    (reader, "events", std::back_inserter(newEvents));

  itsEvents.swap(newEvents);

  reader.read_base_class("TrialEvent", io::make_proxy<TrialEvent>(this));
}

void MultiEvent::write_to(io::writer& writer) const
{
  io::write_utils::write_object_seq(writer, "events",
                                    itsEvents.begin(),
                                    itsEvents.end());

  writer.write_base_class("TrialEvent",
                        io::make_const_proxy<TrialEvent>(this));
}

rutz::fwd_iter<const nub::ref<TrialEvent> >
MultiEvent::getEvents() const
{
GVX_TRACE("MultiEvent::getEvents");

  return rutz::fwd_iter<const nub::ref<TrialEvent> >
    (itsEvents.begin(), itsEvents.end());
}

size_t MultiEvent::addEvent(nub::ref<TrialEvent> event)
{
GVX_TRACE("MultiEvent::addEvent");
  itsEvents.push_back(event);
  GVX_ASSERT(itsEvents.size() >= 1);
  return itsEvents.size() - 1;
}

void MultiEvent::eraseEventAt(unsigned int index)
{
GVX_TRACE("MultiEvent::eraseEventAt");
  if (index >= itsEvents.size())
    throw rutz::error("index out of bounds", SRC_POS);

  itsEvents.erase(itsEvents.begin() + index);
}

void MultiEvent::clearEvents()
{
GVX_TRACE("MultiEvent::clearEvents");
  itsEvents.clear();
}
