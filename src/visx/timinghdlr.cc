/** @file visx/timinghdlr.cc specify a trial's timing sequence in a
    psychophysics experiment */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Mon Jun 21 13:09:57 1999
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

#include "timinghdlr.h"

#include "io/readutils.h"
#include "io/writeutils.h"

#include "nub/log.h"
#include "nub/objmgr.h"
#include "nub/ref.h"

#include "tcl/timerscheduler.h"

#include "rutz/error.h"
#include "rutz/sfmt.h"
#include "rutz/stopwatch.h"

#include "visx/trialevent.h"

#include <algorithm>
#include <memory>
#include <vector>

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

///////////////////////////////////////////////////////////////////////
//
// File scope stuff
//
///////////////////////////////////////////////////////////////////////

namespace
{
  const io::version_id TIMINGHDLR_SVID = 1;
}

///////////////////////////////////////////////////////////////////////
//
// TimingHdlr::Impl definition
//
///////////////////////////////////////////////////////////////////////

class TimingHdlr::Impl
{
private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:
  Impl() :
    scheduler(std::make_shared<tcl::timer_scheduler>()),
    immediateEvents(),
    startEvents(),
    responseEvents(),
    abortEvents(),
    timer(),
    trial(nullptr)
    {}

  std::shared_ptr<nub::scheduler> scheduler;

  typedef std::vector<nub::ref<TrialEvent> > EventGroup;

  EventGroup immediateEvents;
  EventGroup startEvents;
  EventGroup responseEvents;
  EventGroup abortEvents;

  mutable rutz::stopwatch timer;

  EventGroup& eventsAt(TimePoint time_point)
  {
    switch (time_point)
      {
      case IMMEDIATE:     return immediateEvents;
      case FROM_START:    return startEvents;
      case FROM_RESPONSE: return responseEvents;
      case FROM_ABORT:    return abortEvents;
      }

    GVX_ASSERT(false);

    return abortEvents; // but we'll never get here
  }

  Trial* trial;

  static void scheduleAll(EventGroup& events,
                          std::shared_ptr<nub::scheduler> s,
                          Trial* trial);
  static void cancelAll(EventGroup& events);
};

///////////////////////////////////////////////////////////////////////
//
// TimingHdlr definitions
//
///////////////////////////////////////////////////////////////////////

const TimingHdlr::TimePoint TimingHdlr::IMMEDIATE;
const TimingHdlr::TimePoint TimingHdlr::FROM_START;
const TimingHdlr::TimePoint TimingHdlr::FROM_RESPONSE;
const TimingHdlr::TimePoint TimingHdlr::FROM_ABORT;

///////////////////////////////////////////////////////////////////////
//
// TimingHdlr creator method definitions
//
///////////////////////////////////////////////////////////////////////

TimingHdlr* TimingHdlr::make()
{
GVX_TRACE("TimingHdlr::make");
  return new TimingHdlr;
}

TimingHdlr::TimingHdlr() :
  rep(new Impl)
{
GVX_TRACE("TimingHdlr::TimingHdlr");
}

TimingHdlr::~TimingHdlr() noexcept
{
GVX_TRACE("TimingHdlr::~TimingHdlr");

  delete rep;
}

io::version_id TimingHdlr::class_version_id() const
{
GVX_TRACE("TimingHdlr::class_version_id");
  return TIMINGHDLR_SVID;
}

void TimingHdlr::read_from(io::reader& reader)
{
GVX_TRACE("TimingHdlr::read_from");

  reader.ensure_version_id("TimingHdlr", 1, "Try groovx0.8a4", SRC_POS);

  rep->immediateEvents.clear();
  io::read_utils::read_object_seq<TrialEvent>(reader, "immediateEvents",
            std::back_inserter(rep->immediateEvents));

  rep->startEvents.clear();
  io::read_utils::read_object_seq<TrialEvent>(reader, "startEvents",
            std::back_inserter(rep->startEvents));

  rep->responseEvents.clear();
  io::read_utils::read_object_seq<TrialEvent>(reader, "responseEvents",
            std::back_inserter(rep->responseEvents));

  rep->abortEvents.clear();
  io::read_utils::read_object_seq<TrialEvent>(reader, "abortEvents",
            std::back_inserter(rep->abortEvents));
}

void TimingHdlr::write_to(io::writer& writer) const
{
GVX_TRACE("TimingHdlr::write_to");

  writer.ensure_output_version_id("TimingHdlr", TIMINGHDLR_SVID, 1,
                              "Try groovx0.8a4", SRC_POS);

  io::write_utils::write_object_seq(writer, "immediateEvents",
    rep->immediateEvents.begin(), rep->immediateEvents.end());

  io::write_utils::write_object_seq(writer, "startEvents",
    rep->startEvents.begin(), rep->startEvents.end());

  io::write_utils::write_object_seq(writer, "responseEvents",
    rep->responseEvents.begin(), rep->responseEvents.end());

  io::write_utils::write_object_seq(writer, "abortEvents",
    rep->abortEvents.begin(), rep->abortEvents.end());
}

///////////////
// accessors //
///////////////

nub::ref<TrialEvent> TimingHdlr::getEvent(TimePoint time_point,
                                          size_t index) const
{
GVX_TRACE("TimingHdlr::getEvent");
  return rep->eventsAt(time_point).at(index);
}

double TimingHdlr::getElapsedMsec() const
{
GVX_TRACE("TimingHdlr::getElapsedMsec");
  return rep->timer.elapsed().msec();
}

//////////////////
// manipulators //
//////////////////

size_t TimingHdlr::addEvent(nub::ref<TrialEvent> event_item,
                            TimePoint time_point)
{
GVX_TRACE("TimingHdlr::addEvent");

  Impl::EventGroup& events = rep->eventsAt(time_point);
  events.push_back(event_item);
  return events.size() - 1;
}

size_t TimingHdlr::addEventByName(const char* event_type,
                                  TimePoint timepoint, unsigned int msec_delay)
{
GVX_TRACE("TimingHdlr::addEventByName");

  nub::ref<TrialEvent> event_item
    (nub::obj_mgr::new_typed_obj<TrialEvent>(event_type));

  event_item->setDelay(msec_delay);
  return addEvent(event_item, timepoint);
}

///////////////////////////////////////////////////////////////////////
//
// TimingHdlr helper function definitions
//
///////////////////////////////////////////////////////////////////////

namespace
{
  bool cmp_delay_less(const nub::ref<TrialEvent>& e1,
                      const nub::ref<TrialEvent>& e2)
  {
    return (e1->getDelay() < e2->getDelay());
  }
}

void TimingHdlr::Impl::scheduleAll(EventGroup& events,
                                   std::shared_ptr<nub::scheduler> s,
                                   Trial* trial)
{
GVX_TRACE("TimingHdlr::Impl::scheduleAll");
  GVX_PRECONDITION(trial != nullptr);

  // In order to ensure that events get scheduled in the proper order,
  // even if the whole event loop is getting bogged down, we do two
  // things: (1) sort the collection of events according to their
  // requested delays, and (2) keep track of the delay at which each
  // was actually scheduled, and make sure that the next event is
  // scheduled no sooner than that

  std::stable_sort(events.begin(), events.end(), cmp_delay_less);

  unsigned int minimum_delay = 0;

  for (const nub::ref<TrialEvent>& ev: events)
    {
      unsigned int scheduled_delay = ev->schedule(s, *trial, minimum_delay);
      minimum_delay = scheduled_delay+1;

      nub::log(rutz::sfmt("scheduled @ %u: %s", scheduled_delay,
                          ev->unique_name().c_str()));
    }
}

void TimingHdlr::Impl::cancelAll(EventGroup& events)
{
GVX_TRACE("TimingHdlr::Impl::cancelAll");
  for (const nub::ref<TrialEvent>& ev: events)
    {
      ev->cancel();
    }
}

///////////////////////////////////////////////////////////////////////
//
// TimingHdlr action method definitions
//
///////////////////////////////////////////////////////////////////////

void TimingHdlr::thBeginTrial(Trial& trial)
{
GVX_TRACE("TimingHdlr::thBeginTrial");

  rep->timer.restart();

  rep->trial = &trial;

  Impl::cancelAll(rep->responseEvents);
  Impl::cancelAll(rep->abortEvents);
  Impl::scheduleAll(rep->immediateEvents, rep->scheduler, rep->trial);
  Impl::scheduleAll(rep->startEvents, rep->scheduler, rep->trial);
}

void TimingHdlr::thResponseSeen()
{
GVX_TRACE("TimingHdlr::thResponseSeen");
  if (rep->responseEvents.size() > 0)
    {
      Impl::cancelAll(rep->startEvents);
      Impl::scheduleAll(rep->responseEvents, rep->scheduler, rep->trial);
    }
}

void TimingHdlr::thAbortTrial()
{
GVX_TRACE("TimingHdlr::thAbortTrial");
  if (rep->abortEvents.size() > 0)
    {
      Impl::cancelAll(rep->startEvents);
      Impl::cancelAll(rep->responseEvents);
      Impl::scheduleAll(rep->abortEvents, rep->scheduler, rep->trial);
    }
}

void TimingHdlr::thEndTrial()
{
GVX_TRACE("TimingHdlr::thEndTrial");
  Impl::cancelAll(rep->immediateEvents);
  Impl::cancelAll(rep->startEvents);
  Impl::cancelAll(rep->responseEvents);
  Impl::cancelAll(rep->abortEvents);
}

void TimingHdlr::thHaltExpt()
{
GVX_TRACE("TimingHdlr::thHaltExpt");
  Impl::cancelAll(rep->startEvents);
  Impl::cancelAll(rep->responseEvents);
  Impl::cancelAll(rep->abortEvents);
}
