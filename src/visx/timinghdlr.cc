///////////////////////////////////////////////////////////////////////
//
// timinghdlr.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun 21 13:09:57 1999
// written: Wed May 17 13:59:20 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TIMINGHDLR_CC_DEFINED
#define TIMINGHDLR_CC_DEFINED

#include "timinghdlr.h"

#include "trialevent.h"

#include "io/iomgr.h"
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

  vector<TrialEvent*> itsImmediateEvents;
  vector<TrialEvent*> itsStartEvents;
  vector<TrialEvent*> itsResponseEvents;
  vector<TrialEvent*> itsAbortEvents;

  int itsDummyAutosavePeriod;
  
  mutable StopWatch itsTimer;

private:
  GWT::Widget* itsWidget;
  Util::ErrorHandler* itsErrorHandler;
  TrialBase* itsTrial;

  void scheduleAll(vector<TrialEvent*>& events) {
  DOTRACE("scheduleAll");
    Assert(itsTrial != 0);
	 Assert(itsWidget != 0);
	 Assert(itsErrorHandler != 0);

    for (size_t i = 0; i < events.size(); ++i) {
		events[i]->schedule(*itsWidget, *itsErrorHandler, *itsTrial);
	 }
  }

  void cancelAll(vector<TrialEvent*>& events) {
  DOTRACE("cancelAll");
    for (size_t i = 0; i < events.size(); ++i) {
		events[i]->cancel();
	 }
  }

public:
  void deleteAll(vector<TrialEvent*>& events) {
  DOTRACE("deleteAll");
    for (size_t i = 0; i < events.size(); ++i) {
		delete events[i];
		events[i] = 0;
	 }
	 events.resize(0);
  }

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

namespace {

  const unsigned long TIMINGHDLR_SERIAL_VERSION_ID = 1;

  const char* ioTag = "TimingHdlr";
}

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

void TimingHdlr::serialize(ostream &os, IO::IOFlag flag) const {
DOTRACE("TimingHdlr::serialize");
  if (flag & IO::BASES) { /* no bases to serialize */ }

  char sep = ' ';
  if (flag & IO::TYPENAME) { os << ioTag << sep; }
  
  os << itsImpl->itsDummyAutosavePeriod << sep;

  size_t i;

  os << itsImpl->itsImmediateEvents.size() << sep;
  for (i = 0; i < itsImpl->itsImmediateEvents.size(); ++i) {
	 itsImpl->itsImmediateEvents[i]->serialize(os, flag);
  }

  os << itsImpl->itsStartEvents.size() << sep;
  for (i = 0; i < itsImpl->itsStartEvents.size(); ++i) {
	 itsImpl->itsStartEvents[i]->serialize(os, flag);
  }

  os << itsImpl->itsResponseEvents.size() << sep;
  for (i = 0; i < itsImpl->itsResponseEvents.size(); ++i) {
	 itsImpl->itsResponseEvents[i]->serialize(os, flag);
  }

  os << itsImpl->itsAbortEvents.size() << sep;
  for (i = 0; i < itsImpl->itsAbortEvents.size(); ++i) {
	 itsImpl->itsAbortEvents[i]->serialize(os, flag);
  }

  if (os.fail()) throw IO::OutputError(ioTag);
}

void TimingHdlr::deserialize(istream &is, IO::IOFlag flag) {
DOTRACE("TimingHdlr::deserialize");
  if (flag & IO::BASES) { /* no bases to deserialize */ }
  if (flag & IO::TYPENAME) { IO::IoObject::readTypename(is, ioTag); }

  is >> itsImpl->itsDummyAutosavePeriod;
  DebugEvalNL(itsImpl->itsDummyAutosavePeriod);

  int size, i;

  const int SIZE_SANITY_CHECK = 1000;

  itsImpl->deleteAll(itsImpl->itsImmediateEvents);
  is >> size;
  DebugEvalNL(size);
  if (size < 0 || size > SIZE_SANITY_CHECK) throw IO::LogicError(ioTag);
  itsImpl->itsImmediateEvents.resize(size);
  for (i = 0; i < size; ++i) {
	 TrialEvent* e = dynamic_cast<TrialEvent*>(IO::IoMgr::newIO(is, flag));
	 if (!e) throw IO::InputError(ioTag);
	 itsImpl->itsImmediateEvents[i] = e;
  }

  itsImpl->deleteAll(itsImpl->itsStartEvents);
  is >> size;
  DebugEvalNL(size);
  if (size < 0 || size > SIZE_SANITY_CHECK) throw IO::LogicError(ioTag);
  itsImpl->itsStartEvents.resize(size);
  for (i = 0; i < size; ++i) {
	 TrialEvent* e = dynamic_cast<TrialEvent*>(IO::IoMgr::newIO(is, flag));
	 if (!e) throw IO::InputError(ioTag);
	 itsImpl->itsStartEvents[i] = e;
  }

  itsImpl->deleteAll(itsImpl->itsResponseEvents);
  is >> size;
  DebugEvalNL(size);
  if (size < 0 || size > SIZE_SANITY_CHECK) throw IO::LogicError(ioTag);
  itsImpl->itsResponseEvents.resize(size);
  for (i = 0; i < size; ++i) {
	 TrialEvent* e = dynamic_cast<TrialEvent*>(IO::IoMgr::newIO(is, flag));
	 if (!e) throw IO::InputError(ioTag);
	 itsImpl->itsResponseEvents[i] = e;
  }

  itsImpl->deleteAll(itsImpl->itsAbortEvents);
  is >> size;
  DebugEvalNL(size);
  if (size < 0 || size > SIZE_SANITY_CHECK) throw IO::LogicError(ioTag);
  itsImpl->itsAbortEvents.resize(size);
  for (i = 0; i < size; ++i) {
	 TrialEvent* e = dynamic_cast<TrialEvent*>(IO::IoMgr::newIO(is, flag));
	 if (!e) throw IO::InputError(ioTag);
	 itsImpl->itsAbortEvents[i] = e;
  }

  if (is.fail()) throw IO::InputError(ioTag);
}

int TimingHdlr::charCount() const {
DOTRACE("TimingHdlr::charCount"); 

  // XXX not implemented!

  return 256; 
}

unsigned long TimingHdlr::serialVersionId() const {
DOTRACE("TimingHdlr::serialVersionId");
  return TIMINGHDLR_SERIAL_VERSION_ID;
}

void TimingHdlr::readFrom(IO::Reader* reader) {
DOTRACE("TimingHdlr::readFrom");

  unsigned long svid = reader->readSerialVersionId(); 
  if (svid == 0)
	 reader->readValue("autosavePeriod", itsImpl->itsDummyAutosavePeriod);

  itsImpl->deleteAll(itsImpl->itsImmediateEvents);
  IO::ReadUtils::template readObjectSeq<TrialEvent>(reader, "immediateEvents",
								back_inserter(itsImpl->itsImmediateEvents));

  itsImpl->deleteAll(itsImpl->itsStartEvents);
  IO::ReadUtils::template readObjectSeq<TrialEvent>(reader, "startEvents",
								back_inserter(itsImpl->itsStartEvents));

  itsImpl->deleteAll(itsImpl->itsResponseEvents);
  IO::ReadUtils::template readObjectSeq<TrialEvent>(reader, "responseEvents",
								back_inserter(itsImpl->itsResponseEvents));

  itsImpl->deleteAll(itsImpl->itsAbortEvents);
  IO::ReadUtils::template readObjectSeq<TrialEvent>(reader, "abortEvents",
								back_inserter(itsImpl->itsAbortEvents));
}

void TimingHdlr::writeTo(IO::Writer* writer) const {
DOTRACE("TimingHdlr::writeTo");

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
	 return 0;
	 break;
  }
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
	 return -1;
	 break;
  }
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
