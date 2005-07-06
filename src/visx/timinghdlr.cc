/** @file visx/timinghdlr.cc specify a trial's timing sequence in a
    psychophysics experiment */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Jun 21 13:09:57 1999
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_VISX_TIMINGHDLR_CC_UTC20050626084016_DEFINED
#define GROOVX_VISX_TIMINGHDLR_CC_UTC20050626084016_DEFINED

#include "timinghdlr.h"

#include "io/readutils.h"
#include "io/writeutils.h"

#include "nub/log.h"
#include "nub/objmgr.h"
#include "nub/ref.h"

#include "tcl/timerscheduler.h"

#include "rutz/error.h"
#include "rutz/sharedptr.h"
#include "rutz/stopwatch.h"

#include "visx/trialevent.h"

#include <algorithm>
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
  const IO::VersionId TIMINGHDLR_SVID = 1;
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
    scheduler(rutz::make_shared(new tcl::timer_scheduler)),
    immediateEvents(),
    startEvents(),
    responseEvents(),
    abortEvents(),
    timer(),
    trial(0)
    {}

  rutz::shared_ptr<nub::scheduler> scheduler;

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
                          rutz::shared_ptr<nub::scheduler> s,
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

TimingHdlr::~TimingHdlr() throw()
{
GVX_TRACE("TimingHdlr::~TimingHdlr");

  delete rep;
}

IO::VersionId TimingHdlr::serialVersionId() const
{
GVX_TRACE("TimingHdlr::serialVersionId");
  return TIMINGHDLR_SVID;
}

void TimingHdlr::readFrom(IO::Reader& reader)
{
GVX_TRACE("TimingHdlr::readFrom");

  reader.ensureReadVersionId("TimingHdlr", 1, "Try groovx0.8a4", SRC_POS);

  rep->immediateEvents.clear();
  IO::ReadUtils::readObjectSeq<TrialEvent>(reader, "immediateEvents",
            std::back_inserter(rep->immediateEvents));

  rep->startEvents.clear();
  IO::ReadUtils::readObjectSeq<TrialEvent>(reader, "startEvents",
            std::back_inserter(rep->startEvents));

  rep->responseEvents.clear();
  IO::ReadUtils::readObjectSeq<TrialEvent>(reader, "responseEvents",
            std::back_inserter(rep->responseEvents));

  rep->abortEvents.clear();
  IO::ReadUtils::readObjectSeq<TrialEvent>(reader, "abortEvents",
            std::back_inserter(rep->abortEvents));
}

void TimingHdlr::writeTo(IO::Writer& writer) const
{
GVX_TRACE("TimingHdlr::writeTo");

  writer.ensureWriteVersionId("TimingHdlr", TIMINGHDLR_SVID, 1,
                              "Try groovx0.8a4", SRC_POS);

  IO::WriteUtils::writeObjectSeq(writer, "immediateEvents",
    rep->immediateEvents.begin(), rep->immediateEvents.end());

  IO::WriteUtils::writeObjectSeq(writer, "startEvents",
    rep->startEvents.begin(), rep->startEvents.end());

  IO::WriteUtils::writeObjectSeq(writer, "responseEvents",
    rep->responseEvents.begin(), rep->responseEvents.end());

  IO::WriteUtils::writeObjectSeq(writer, "abortEvents",
    rep->abortEvents.begin(), rep->abortEvents.end());
}

///////////////
// accessors //
///////////////

nub::ref<TrialEvent> TimingHdlr::getEvent(TimePoint time_point,
                                          unsigned int index) const
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

unsigned int TimingHdlr::addEvent(nub::ref<TrialEvent> event_item,
                                  TimePoint time_point)
{
GVX_TRACE("TimingHdlr::addEvent");

  Impl::EventGroup& events = rep->eventsAt(time_point);
  events.push_back(event_item);
  return events.size() - 1;
}

unsigned int TimingHdlr::addEventByName(const char* event_type,
                                        TimePoint timepoint, int msec_delay)
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
                                   rutz::shared_ptr<nub::scheduler> s,
                                   Trial* trial)
{
GVX_TRACE("TimingHdlr::Impl::scheduleAll");
  GVX_PRECONDITION(trial != 0);

  // In order to ensure that events get scheduled in the proper order,
  // even if the whole event loop is getting bogged down, we do two
  // things: (1) sort the collection of events according to their
  // requested delays, and (2) keep track of the delay at which each
  // was actually scheduled, and make sure that the next event is
  // scheduled no sooner than that

  std::stable_sort(events.begin(), events.end(), cmp_delay_less);

  unsigned int minimum_delay = 0;

  for (size_t i = 0; i < events.size(); ++i)
    {
      unsigned int scheduled_delay =
        events[i]->schedule(s, *trial, minimum_delay);
      minimum_delay = scheduled_delay+1;

      rutz::fstring info("scheduled @ ", scheduled_delay,
                         ": ", events[i]->unique_name());
      nub::log(info);
    }
}

void TimingHdlr::Impl::cancelAll(EventGroup& events)
{
GVX_TRACE("TimingHdlr::Impl::cancelAll");
  for (size_t i = 0; i < events.size(); ++i)
    {
      events[i]->cancel();
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

static const char vcid_groovx_visx_timinghdlr_cc_utc20050626084016[] = "$Id$ $HeadURL$";
#endif // !GROOVX_VISX_TIMINGHDLR_CC_UTC20050626084016_DEFINED
