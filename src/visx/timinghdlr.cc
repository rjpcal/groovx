///////////////////////////////////////////////////////////////////////
//
// timinghdlr.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jun 21 13:09:57 1999
// written: Mon Jun 11 14:49:18 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TIMINGHDLR_CC_DEFINED
#define TIMINGHDLR_CC_DEFINED

#include "timinghdlr.h"

#include "trialevent.h"

#include "io/readutils.h"
#include "io/writeutils.h"

#include "util/error.h"
#include "util/iditem.h"
#include "util/minivec.h"
#include "util/objmgr.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// File scope stuff
//
///////////////////////////////////////////////////////////////////////

namespace {
  const IO::VersionId TIMINGHDLR_SERIAL_VERSION_ID = 1;
}

///////////////////////////////////////////////////////////////////////
//
// TimingHdlr::Impl definition
//
///////////////////////////////////////////////////////////////////////

class TimingHdlr::Impl {
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
	 itsWidget(0),
	 itsErrorHandler(0),
	 itsTrial(0)
	 {}

  typedef minivec<Ref<TrialEvent> > EventGroup;

  EventGroup itsImmediateEvents;
  EventGroup itsStartEvents;
  EventGroup itsResponseEvents;
  EventGroup itsAbortEvents;

  mutable StopWatch itsTimer;

private:
  GWT::Widget* itsWidget;
  Util::ErrorHandler* itsErrorHandler;
  TrialBase* itsTrial;

  void scheduleAll(EventGroup& events);
  void cancelAll(EventGroup& events);

public:
  // Delegand functions
  void thHaltExpt();
  void thAbortTrial();
  void thResponseSeen();
  void thBeginTrial(GWT::Widget& widget, Util::ErrorHandler& eh, TrialBase& trial);
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

TimingHdlr* TimingHdlr::make() {
DOTRACE("TimingHdlr::make");
  return new TimingHdlr;
}

TimingHdlr::TimingHdlr() :
  itsImpl(new Impl)
{
DOTRACE("TimingHdlr::TimingHdlr");
}

TimingHdlr::~TimingHdlr() {
DOTRACE("TimingHdlr::~TimingHdlr");

  delete itsImpl;
}

IO::VersionId TimingHdlr::serialVersionId() const {
DOTRACE("TimingHdlr::serialVersionId");
  return TIMINGHDLR_SERIAL_VERSION_ID;
}

void TimingHdlr::readFrom(IO::Reader* reader) {
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

void TimingHdlr::writeTo(IO::Writer* writer) const {
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

Ref<TrialEvent> TimingHdlr::getEvent(TimePoint time_point, int index) const {
DOTRACE("TimingHdlr::getEvent");
  switch (time_point) {
  case IMMEDIATE:
	 return itsImpl->itsImmediateEvents.at(index);
	 break;
  case FROM_START:
	 return itsImpl->itsStartEvents.at(index);
	 break;
  case FROM_RESPONSE:
	 return itsImpl->itsResponseEvents.at(index);
	 break;
  case FROM_ABORT:
	 return itsImpl->itsAbortEvents.at(index);
	 break;
  default:
	 break;
  }  
  return Ref<TrialEvent>(Util::UID(0)); // will raise an exception
}

int TimingHdlr::getElapsedMsec() const {
DOTRACE("TimingHdlr::getElapsedMsec");
  return itsImpl->itsTimer.elapsedMsec();
}

//////////////////
// manipulators //
//////////////////

int TimingHdlr::addEvent(Ref<TrialEvent> event_item, TimePoint time_point) {
DOTRACE("TimingHdlr::addEvent");

  switch (time_point) {
  case IMMEDIATE:
	 event_item->setDelay(0);
	 itsImpl->itsImmediateEvents.push_back(event_item);
	 return itsImpl->itsImmediateEvents.size() - 1;
	 break;
  case FROM_START:
	 itsImpl->itsStartEvents.push_back(event_item);
	 return itsImpl->itsStartEvents.size() - 1;
	 break;
  case FROM_RESPONSE:
	 itsImpl->itsResponseEvents.push_back(event_item);
	 return itsImpl->itsResponseEvents.size() - 1;
	 break;
  case FROM_ABORT:
	 itsImpl->itsAbortEvents.push_back(event_item);
	 return itsImpl->itsAbortEvents.size() - 1;
	 break;
  default:
	 break;
  }

  return -1;
}

int TimingHdlr::addEventByName(const char* event_type, TimePoint timepoint,
										 int msec_delay) {
DOTRACE("TimingHdlr::addEventByName");

  try {
	 Ref<TrialEvent> event_item(
                         Util::ObjMgr::newTypedObj<TrialEvent>(event_type));

	 event_item->setDelay(msec_delay);
	 return addEvent(event_item, timepoint);
  }
  catch(...) { return -1; }
}

///////////////////////////////////////////////////////////////////////
//
// TimingHdlr helper function definitions
//
///////////////////////////////////////////////////////////////////////

void TimingHdlr::Impl::scheduleAll(EventGroup& events) {
DOTRACE("TimingHdlr::Impl::scheduleAll");
  Precondition(itsTrial != 0);
  Precondition(itsWidget != 0);
  Precondition(itsErrorHandler != 0);

  for (size_t i = 0; i < events.size(); ++i) {
	 events[i]->schedule(*itsWidget, *itsErrorHandler, *itsTrial);
  }
}

void TimingHdlr::Impl::cancelAll(EventGroup& events) {
DOTRACE("TimingHdlr::Impl::cancelAll");
  for (size_t i = 0; i < events.size(); ++i) {
	 events[i]->cancel();
  }
}

///////////////////////////////////////////////////////////////////////
//
// TimingHdlr action method definitions
//
///////////////////////////////////////////////////////////////////////

void TimingHdlr::Impl::thBeginTrial(GWT::Widget& widget,
												Util::ErrorHandler& eh, TrialBase& trial) {
DOTRACE("TimingHdlr::Impl::thBeginTrial");

  itsTimer.restart();

  itsWidget = &widget;
  itsErrorHandler = &eh;
  itsTrial = &trial;

  cancelAll(itsResponseEvents);
  cancelAll(itsAbortEvents);
  scheduleAll(itsImmediateEvents);
  scheduleAll(itsStartEvents);
}

void TimingHdlr::Impl::thResponseSeen() {
DOTRACE("TimingHdlr::Impl::thResponseSeen");
  if (itsResponseEvents.size() > 0) { 
	 cancelAll(itsStartEvents);
	 scheduleAll(itsResponseEvents);
  }
}

void TimingHdlr::Impl::thAbortTrial() {
DOTRACE("TimingHdlr::Impl::thAbortTrial");
  cancelAll(itsStartEvents);
  cancelAll(itsResponseEvents);
  scheduleAll(itsAbortEvents);
}

void TimingHdlr::Impl::thHaltExpt() {
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

void TimingHdlr::thBeginTrial(GWT::Widget& widget,
										Util::ErrorHandler& eh, TrialBase& trial)
  { itsImpl->thBeginTrial(widget, eh, trial); }

static const char vcid_timinghdlr_cc[] = "$Header$";
#endif // !TIMINGHDLR_CC_DEFINED
