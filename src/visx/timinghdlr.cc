///////////////////////////////////////////////////////////////////////
//
// timinghdlr.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun 21 13:09:57 1999
// written: Mon Oct  9 19:46:57 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TIMINGHDLR_CC_DEFINED
#define TIMINGHDLR_CC_DEFINED

#include "timinghdlr.h"

#include "trialevent.h"

#include "io/iomgr.h"
#include "io/iolegacy.h"
#include "io/readutils.h"
#include "io/writeutils.h"

#include <cstring>
#include <vector>

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
 	 itsDummyAutosavePeriod(10),
	 itsTimer(),
	 itsWidget(0),
	 itsErrorHandler(0),
	 itsTrial(0)
	 {}

  std::vector<TrialEvent*> itsImmediateEvents;
  std::vector<TrialEvent*> itsStartEvents;
  std::vector<TrialEvent*> itsResponseEvents;
  std::vector<TrialEvent*> itsAbortEvents;

  int itsDummyAutosavePeriod;
  
  mutable StopWatch itsTimer;

private:
  GWT::Widget* itsWidget;
  Util::ErrorHandler* itsErrorHandler;
  TrialBase* itsTrial;

  void scheduleAll(std::vector<TrialEvent*>& events);
  void cancelAll(std::vector<TrialEvent*>& events);

public:
  void deleteAll(std::vector<TrialEvent*>& events);

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

TimingHdlr::TimingHdlr() :
  itsImpl(new Impl)
{
DOTRACE("TimingHdlr::TimingHdlr");
}

TimingHdlr::~TimingHdlr() {
DOTRACE("TimingHdlr::~TimingHdlr");
  itsImpl->deleteAll(itsImpl->itsImmediateEvents);
  itsImpl->deleteAll(itsImpl->itsStartEvents);
  itsImpl->deleteAll(itsImpl->itsResponseEvents);
  itsImpl->deleteAll(itsImpl->itsAbortEvents);

  delete itsImpl;
}

void TimingHdlr::legacySrlz(IO::LegacyWriter* lwriter) const {
DOTRACE("TimingHdlr::legacySrlz");

  lwriter->writeValue("autosavePeriod", itsImpl->itsDummyAutosavePeriod);

  IO::WriteUtils::writeObjectSeq(lwriter, "immediateEvents",
     itsImpl->itsImmediateEvents.begin(), itsImpl->itsImmediateEvents.end());

  IO::WriteUtils::writeObjectSeq(lwriter, "startEvents",
     itsImpl->itsStartEvents.begin(), itsImpl->itsStartEvents.end());

  IO::WriteUtils::writeObjectSeq(lwriter, "responseEvents",
     itsImpl->itsResponseEvents.begin(), itsImpl->itsResponseEvents.end());

  IO::WriteUtils::writeObjectSeq(lwriter, "abortEvents",
     itsImpl->itsAbortEvents.begin(), itsImpl->itsAbortEvents.end());
}

void TimingHdlr::legacyDesrlz(IO::LegacyReader* lreader) {
DOTRACE("TimingHdlr::legacyDesrlz");

  lreader->readValue("autosavePeriod", itsImpl->itsDummyAutosavePeriod);

  itsImpl->deleteAll(itsImpl->itsImmediateEvents);
  IO::ReadUtils::template readObjectSeq<TrialEvent>(lreader, "immediateEvents",
                        std::back_inserter(itsImpl->itsImmediateEvents));

  itsImpl->deleteAll(itsImpl->itsStartEvents);
  IO::ReadUtils::template readObjectSeq<TrialEvent>(lreader, "startEvents",
                        std::back_inserter(itsImpl->itsStartEvents));

  itsImpl->deleteAll(itsImpl->itsResponseEvents);
  IO::ReadUtils::template readObjectSeq<TrialEvent>(lreader, "responseEvents",
                        std::back_inserter(itsImpl->itsResponseEvents));

  itsImpl->deleteAll(itsImpl->itsAbortEvents);
  IO::ReadUtils::template readObjectSeq<TrialEvent>(lreader, "abortEvents",
                        std::back_inserter(itsImpl->itsAbortEvents));
}

IO::VersionId TimingHdlr::serialVersionId() const {
DOTRACE("TimingHdlr::serialVersionId");
  return TIMINGHDLR_SERIAL_VERSION_ID;
}

void TimingHdlr::readFrom(IO::Reader* reader) {
DOTRACE("TimingHdlr::readFrom");

  IO::LegacyReader* lreader = dynamic_cast<IO::LegacyReader*>(reader); 
  if (lreader != 0) {
	 legacyDesrlz(lreader);
	 return;
  }

  IO::VersionId svid = reader->readSerialVersionId(); 
  if (svid == 0)
	 reader->readValue("autosavePeriod", itsImpl->itsDummyAutosavePeriod);

  itsImpl->deleteAll(itsImpl->itsImmediateEvents);
  IO::ReadUtils::template readObjectSeq<TrialEvent>(reader, "immediateEvents",
								std::back_inserter(itsImpl->itsImmediateEvents));

  itsImpl->deleteAll(itsImpl->itsStartEvents);
  IO::ReadUtils::template readObjectSeq<TrialEvent>(reader, "startEvents",
								std::back_inserter(itsImpl->itsStartEvents));

  itsImpl->deleteAll(itsImpl->itsResponseEvents);
  IO::ReadUtils::template readObjectSeq<TrialEvent>(reader, "responseEvents",
								std::back_inserter(itsImpl->itsResponseEvents));

  itsImpl->deleteAll(itsImpl->itsAbortEvents);
  IO::ReadUtils::template readObjectSeq<TrialEvent>(reader, "abortEvents",
								std::back_inserter(itsImpl->itsAbortEvents));
}

void TimingHdlr::writeTo(IO::Writer* writer) const {
DOTRACE("TimingHdlr::writeTo");

  IO::LegacyWriter* lwriter = dynamic_cast<IO::LegacyWriter*>(writer);
  if (lwriter != 0) {
	 legacySrlz(lwriter);
	 return;
  }

  if (TIMINGHDLR_SERIAL_VERSION_ID == 0)
	 writer->writeValue("autosavePeriod", itsImpl->itsDummyAutosavePeriod);

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

TrialEvent* TimingHdlr::getEvent(TimePoint time_point, int index) const {
DOTRACE("TimingHdlr::getEvent");
  switch (time_point) {
  case IMMEDIATE:
	 return itsImpl->itsImmediateEvents[index];
	 break;
  case FROM_START:
	 return itsImpl->itsStartEvents[index];
	 break;
  case FROM_RESPONSE:
	 return itsImpl->itsResponseEvents[index];
	 break;
  case FROM_ABORT:
	 return itsImpl->itsAbortEvents[index];
	 break;
  default:
	 break;
  }
  return 0;
}

int TimingHdlr::getElapsedMsec() const {
DOTRACE("TimingHdlr::getElapsedMsec");
  return itsImpl->itsTimer.elapsedMsec();
}

//////////////////
// manipulators //
//////////////////

int TimingHdlr::addEvent(TrialEvent* event, TimePoint time_point) {
DOTRACE("TimingHdlr::addEvent");
  switch (time_point) {
  case IMMEDIATE:
	 event->setDelay(0);
	 itsImpl->itsImmediateEvents.push_back(event);
	 return itsImpl->itsImmediateEvents.size() - 1;
	 break;
  case FROM_START:
	 itsImpl->itsStartEvents.push_back(event);
	 return itsImpl->itsStartEvents.size() - 1;
	 break;
  case FROM_RESPONSE:
	 itsImpl->itsResponseEvents.push_back(event);
	 return itsImpl->itsResponseEvents.size() - 1;
	 break;
  case FROM_ABORT:
	 itsImpl->itsAbortEvents.push_back(event);
	 return itsImpl->itsAbortEvents.size() - 1;
	 break;
  default:
	 break;
  }

  // This is necessary to prevent a resource leak, since this function
  // unconditionally assumes ownership of 'event'; if it does not make
  // it onto one of the lists then we must get rid of it now.
  delete event;

  return -1;
}

int TimingHdlr::addEventByName(const char* event_type, TimePoint timepoint,
										 int msec_delay) {
DOTRACE("TimingHdlr::addEventByName");
  TrialEvent* e = dynamic_cast<TrialEvent*>(IO::IoMgr::newIO(event_type));
  if (e == 0) { return -1; }

  e->setDelay(msec_delay);
  return addEvent(e, timepoint);
}

///////////////////////////////////////////////////////////////////////
//
// TimingHdlr helper function definitions
//
///////////////////////////////////////////////////////////////////////

void TimingHdlr::Impl::scheduleAll(std::vector<TrialEvent*>& events) {
DOTRACE("TimingHdlr::Impl::scheduleAll");
  Precondition(itsTrial != 0);
  Precondition(itsWidget != 0);
  Precondition(itsErrorHandler != 0);

  for (size_t i = 0; i < events.size(); ++i) {
	 events[i]->schedule(*itsWidget, *itsErrorHandler, *itsTrial);
  }
}

void TimingHdlr::Impl::cancelAll(std::vector<TrialEvent*>& events) {
DOTRACE("TimingHdlr::Impl::cancelAll");
  for (size_t i = 0; i < events.size(); ++i) {
	 events[i]->cancel();
  }
}

void TimingHdlr::Impl::deleteAll(std::vector<TrialEvent*>& events) {
DOTRACE("TimingHdlr::Impl::deleteAll");
  for (size_t i = 0; i < events.size(); ++i) {
	 delete events[i];
	 events[i] = 0;
  }
  events.resize(0);
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
