///////////////////////////////////////////////////////////////////////
//
// timinghdlr.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jun 21 13:09:57 1999
// written: Wed Dec  4 18:41:16 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TIMINGHDLR_CC_DEFINED
#define TIMINGHDLR_CC_DEFINED

#include "visx/timinghdlr.h"

#include "visx/trialevent.h"

#include "io/readutils.h"
#include "io/writeutils.h"

#include "util/error.h"
#include "util/minivec.h"
#include "util/objmgr.h"
#include "util/ref.h"

#include <algorithm>

#include "util/trace.h"
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// File scope stuff
//
///////////////////////////////////////////////////////////////////////

namespace
{
  const IO::VersionId TIMINGHDLR_SERIAL_VERSION_ID = 1;
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
    itsImmediateEvents(),
    itsStartEvents(),
    itsResponseEvents(),
    itsAbortEvents(),
    itsTimer(),
    itsErrorHandler(0),
    itsTrial(0)
    {}

  typedef minivec<Ref<TrialEvent> > EventGroup;

  EventGroup itsImmediateEvents;
  EventGroup itsStartEvents;
  EventGroup itsResponseEvents;
  EventGroup itsAbortEvents;

  mutable StopWatch itsTimer;

  EventGroup& eventsAt(TimePoint time_point)
  {
    switch (time_point)
      {
      case IMMEDIATE:     return itsImmediateEvents;
      case FROM_START:    return itsStartEvents;
      case FROM_RESPONSE: return itsResponseEvents;
      case FROM_ABORT:    return itsAbortEvents;
      }

    throw Util::Error("unknown TimingHdlr::TimePoint enumerant");
    return itsAbortEvents; // but we'll never get here
  }

  const EventGroup& eventsAt(TimePoint time_point) const
  { return const_cast<Impl*>(this)->eventsAt(time_point); }

private:
  Util::ErrorHandler* itsErrorHandler;
  Trial* itsTrial;

  void scheduleAll(EventGroup& events);
  void cancelAll(EventGroup& events);

public:
  // Delegand functions
  void thHaltExpt();
  void thAbortTrial();
  void thResponseSeen();
  void thBeginTrial(Trial& trial, Util::ErrorHandler& eh);
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
DOTRACE("TimingHdlr::make");
  return new TimingHdlr;
}

TimingHdlr::TimingHdlr() :
  itsImpl(new Impl)
{
DOTRACE("TimingHdlr::TimingHdlr");
}

TimingHdlr::~TimingHdlr()
{
DOTRACE("TimingHdlr::~TimingHdlr");

  delete itsImpl;
}

IO::VersionId TimingHdlr::serialVersionId() const
{
DOTRACE("TimingHdlr::serialVersionId");
  return TIMINGHDLR_SERIAL_VERSION_ID;
}

void TimingHdlr::readFrom(IO::Reader* reader)
{
DOTRACE("TimingHdlr::readFrom");

  reader->ensureReadVersionId("TimingHdlr", 1, "Try grsh0.8a4");

  itsImpl->itsImmediateEvents.clear();
  IO::ReadUtils::readObjectSeq<TrialEvent>(reader, "immediateEvents",
            std::back_inserter(itsImpl->itsImmediateEvents));

  itsImpl->itsStartEvents.clear();
  IO::ReadUtils::readObjectSeq<TrialEvent>(reader, "startEvents",
            std::back_inserter(itsImpl->itsStartEvents));

  itsImpl->itsResponseEvents.clear();
  IO::ReadUtils::readObjectSeq<TrialEvent>(reader, "responseEvents",
            std::back_inserter(itsImpl->itsResponseEvents));

  itsImpl->itsAbortEvents.clear();
  IO::ReadUtils::readObjectSeq<TrialEvent>(reader, "abortEvents",
            std::back_inserter(itsImpl->itsAbortEvents));
}

void TimingHdlr::writeTo(IO::Writer* writer) const
{
DOTRACE("TimingHdlr::writeTo");

  writer->ensureWriteVersionId("TimingHdlr", TIMINGHDLR_SERIAL_VERSION_ID, 1,
                               "Try grsh0.8a4");

  IO::WriteUtils::writeObjectSeq(writer, "immediateEvents",
    itsImpl->itsImmediateEvents.begin(), itsImpl->itsImmediateEvents.end());

  IO::WriteUtils::writeObjectSeq(writer, "startEvents",
    itsImpl->itsStartEvents.begin(), itsImpl->itsStartEvents.end());

  IO::WriteUtils::writeObjectSeq(writer, "responseEvents",
    itsImpl->itsResponseEvents.begin(), itsImpl->itsResponseEvents.end());

  IO::WriteUtils::writeObjectSeq(writer, "abortEvents",
    itsImpl->itsAbortEvents.begin(), itsImpl->itsAbortEvents.end());
}

///////////////
// accessors //
///////////////

Ref<TrialEvent> TimingHdlr::getEvent(TimePoint time_point,
                                     unsigned int index) const
{
DOTRACE("TimingHdlr::getEvent");
  return itsImpl->eventsAt(time_point).at(index);
}

double TimingHdlr::getElapsedMsec() const
{
DOTRACE("TimingHdlr::getElapsedMsec");
  return itsImpl->itsTimer.elapsedMsec();
}

//////////////////
// manipulators //
//////////////////

unsigned int TimingHdlr::addEvent(Ref<TrialEvent> event_item,
                                  TimePoint time_point)
{
DOTRACE("TimingHdlr::addEvent");

  Impl::EventGroup& events = itsImpl->eventsAt(time_point);
  events.push_back(event_item);
  return events.size() - 1;
}

unsigned int TimingHdlr::addEventByName(const char* event_type,
                                        TimePoint timepoint, int msec_delay)
{
DOTRACE("TimingHdlr::addEventByName");

  Ref<TrialEvent> event_item(
                    Util::ObjMgr::newTypedObj<TrialEvent>(event_type));

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
  bool cmp_delay_less(const Ref<TrialEvent>& e1, const Ref<TrialEvent>& e2)
  {
    return (e1->getDelay() < e2->getDelay());
  }
}

void TimingHdlr::Impl::scheduleAll(EventGroup& events)
{
DOTRACE("TimingHdlr::Impl::scheduleAll");
  Precondition(itsTrial != 0);
  Precondition(itsErrorHandler != 0);

  // In order to ensure that events get scheduled in the proper order, even if
  // the whole event loop is getting bogged down, we do two things: (1) sort
  // the collection of events according to their requested delays, and (2)
  // keep track of the delay at which each was actually scheduled, and make
  // sure that the next event is scheduled no sooner than that

  std::stable_sort(events.begin(), events.end(), cmp_delay_less);

  int minimum_delay = 0;

  for (size_t i = 0; i < events.size(); ++i)
    {
      int scheduled_delay =
        events[i]->schedule(*itsTrial, *itsErrorHandler, minimum_delay);
      minimum_delay = scheduled_delay+1;
    }
}

void TimingHdlr::Impl::cancelAll(EventGroup& events)
{
DOTRACE("TimingHdlr::Impl::cancelAll");
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

void TimingHdlr::Impl::thBeginTrial(Trial& trial, Util::ErrorHandler& eh)
{
DOTRACE("TimingHdlr::Impl::thBeginTrial");

  itsTimer.restart();

  itsErrorHandler = &eh;
  itsTrial = &trial;

  cancelAll(itsResponseEvents);
  cancelAll(itsAbortEvents);
  scheduleAll(itsImmediateEvents);
  scheduleAll(itsStartEvents);
}

void TimingHdlr::Impl::thResponseSeen()
{
DOTRACE("TimingHdlr::Impl::thResponseSeen");
  if (itsResponseEvents.size() > 0)
    {
      cancelAll(itsStartEvents);
      scheduleAll(itsResponseEvents);
    }
}

void TimingHdlr::Impl::thAbortTrial()
{
DOTRACE("TimingHdlr::Impl::thAbortTrial");
  if (itsAbortEvents.size() > 0)
    {
      cancelAll(itsStartEvents);
      cancelAll(itsResponseEvents);
      scheduleAll(itsAbortEvents);
    }
}

void TimingHdlr::Impl::thHaltExpt()
{
DOTRACE("TimingHdlr::Impl::thHaltExpt");
  cancelAll(itsStartEvents);
  cancelAll(itsResponseEvents);
  cancelAll(itsAbortEvents);
}

///////////////////////////////////////////////////////////////////////
//
// TimingHdlr::Impl delegations
//
///////////////////////////////////////////////////////////////////////

void TimingHdlr::thHaltExpt()
  { itsImpl->thHaltExpt(); }

void TimingHdlr::thAbortTrial()
  { itsImpl->thAbortTrial(); }

void TimingHdlr::thResponseSeen()
  { itsImpl->thResponseSeen(); }

void TimingHdlr::thBeginTrial(Trial& trial, Util::ErrorHandler& eh)
  { itsImpl->thBeginTrial(trial, eh); }

static const char vcid_timinghdlr_cc[] = "$Header$";
#endif // !TIMINGHDLR_CC_DEFINED
