///////////////////////////////////////////////////////////////////////
//
// timinghdlr.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun 21 13:09:57 1999
// written: Wed Mar 15 10:17:25 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TIMINGHDLR_CC_DEFINED
#define TIMINGHDLR_CC_DEFINED

#include "timinghdlr.h"

#include "iomgr.h"
#include "readutils.h"
#include "trialevent.h"
#include "writeutils.h"

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
public:
  Impl() :
	 itsImmediateEvents(),
	 itsStartEvents(),
	 itsResponseEvents(),
	 itsAbortEvents(),
	 itsAutosavePeriod(10),
	 itsTimer()
	 {}

  vector<TrialEvent*> itsImmediateEvents;
  vector<TrialEvent*> itsStartEvents;
  vector<TrialEvent*> itsResponseEvents;
  vector<TrialEvent*> itsAbortEvents;

  int itsAutosavePeriod;
  
  mutable StopWatch itsTimer;
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
  const char* ioTag = "TimingHdlr";

  void scheduleAll(vector<TrialEvent*>& events, Experiment* expt) {
  DOTRACE("scheduleAll");
    for (size_t i = 0; i < events.size(); ++i) {
		events[i]->schedule(expt);
	 }
#if 0
	 for_each(events.begin(), events.end(), mem_fun(&TrialEvent::schedule));
#endif
  }

  void cancelAll(vector<TrialEvent*>& events) {
  DOTRACE("cancelAll");
    for (size_t i = 0; i < events.size(); ++i) {
		events[i]->cancel();
	 }
#if 0
	 for_each(events.begin(), events.end(), mem_fun(&TrialEvent::cancel));
#endif
  }

  void deleteAll(vector<TrialEvent*>& events) {
  DOTRACE("deleteAll");
    for (size_t i = 0; i < events.size(); ++i) {
		delete events[i];
		events[i] = 0;
	 }
	 events.resize(0);
  }

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
  deleteAll(itsImpl->itsImmediateEvents);
  deleteAll(itsImpl->itsStartEvents);
  deleteAll(itsImpl->itsResponseEvents);
  deleteAll(itsImpl->itsAbortEvents);

  delete itsImpl;
}

void TimingHdlr::serialize(ostream &os, IOFlag flag) const {
DOTRACE("TimingHdlr::serialize");
  if (flag & BASES) { /* no bases to serialize */ }

  char sep = ' ';
  if (flag & TYPENAME) { os << ioTag << sep; }
  
  os << itsImpl->itsAutosavePeriod << sep;

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

  if (os.fail()) throw OutputError(ioTag);
}

void TimingHdlr::deserialize(istream &is, IOFlag flag) {
DOTRACE("TimingHdlr::deserialize");
  if (flag & BASES) { /* no bases to deserialize */ }
  if (flag & TYPENAME) { IO::readTypename(is, ioTag); }

  is >> itsImpl->itsAutosavePeriod;
  DebugEvalNL(itsImpl->itsAutosavePeriod);

  int size, i;

  const int SIZE_SANITY_CHECK = 1000;

  deleteAll(itsImpl->itsImmediateEvents);
  is >> size;
  DebugEvalNL(size);
  if (size < 0 || size > SIZE_SANITY_CHECK) throw IoLogicError(ioTag);
  itsImpl->itsImmediateEvents.resize(size);
  for (i = 0; i < size; ++i) {
	 TrialEvent* e = dynamic_cast<TrialEvent*>(IoMgr::newIO(is, flag));
	 if (!e) throw InputError(ioTag);
	 itsImpl->itsImmediateEvents[i] = e;
  }

  deleteAll(itsImpl->itsStartEvents);
  is >> size;
  DebugEvalNL(size);
  if (size < 0 || size > SIZE_SANITY_CHECK) throw IoLogicError(ioTag);
  itsImpl->itsStartEvents.resize(size);
  for (i = 0; i < size; ++i) {
	 TrialEvent* e = dynamic_cast<TrialEvent*>(IoMgr::newIO(is, flag));
	 if (!e) throw InputError(ioTag);
	 itsImpl->itsStartEvents[i] = e;
  }

  deleteAll(itsImpl->itsResponseEvents);
  is >> size;
  DebugEvalNL(size);
  if (size < 0 || size > SIZE_SANITY_CHECK) throw IoLogicError(ioTag);
  itsImpl->itsResponseEvents.resize(size);
  for (i = 0; i < size; ++i) {
	 TrialEvent* e = dynamic_cast<TrialEvent*>(IoMgr::newIO(is, flag));
	 if (!e) throw InputError(ioTag);
	 itsImpl->itsResponseEvents[i] = e;
  }

  deleteAll(itsImpl->itsAbortEvents);
  is >> size;
  DebugEvalNL(size);
  if (size < 0 || size > SIZE_SANITY_CHECK) throw IoLogicError(ioTag);
  itsImpl->itsAbortEvents.resize(size);
  for (i = 0; i < size; ++i) {
	 TrialEvent* e = dynamic_cast<TrialEvent*>(IoMgr::newIO(is, flag));
	 if (!e) throw InputError(ioTag);
	 itsImpl->itsAbortEvents[i] = e;
  }

  if (is.fail()) throw InputError(ioTag);
}

int TimingHdlr::charCount() const {
DOTRACE("TimingHdlr::charCount"); 

  // XXX not implemented!

  return 256; 
}

void TimingHdlr::readFrom(Reader* reader) {
DOTRACE("TimingHdlr::readFrom");
  reader->readValue("autosavePeriod", itsImpl->itsAutosavePeriod);

  deleteAll(itsImpl->itsImmediateEvents);
  ReadUtils::readObjectSeq(reader, "immediateEvents",
								back_inserter(itsImpl->itsImmediateEvents), (TrialEvent*)0);

  deleteAll(itsImpl->itsStartEvents);
  ReadUtils::readObjectSeq(reader, "startEvents",
								back_inserter(itsImpl->itsStartEvents), (TrialEvent*)0);

  deleteAll(itsImpl->itsResponseEvents);
  ReadUtils::readObjectSeq(reader, "responseEvents",
								back_inserter(itsImpl->itsResponseEvents), (TrialEvent*)0);

  deleteAll(itsImpl->itsAbortEvents);
  ReadUtils::readObjectSeq(reader, "abortEvents",
								back_inserter(itsImpl->itsAbortEvents), (TrialEvent*)0);
}

void TimingHdlr::writeTo(Writer* writer) const {
DOTRACE("TimingHdlr::writeTo");
  writer->writeValue("autosavePeriod", itsImpl->itsAutosavePeriod);

  WriteUtils::writeObjectSeq(writer, "immediateEvents",
								 itsImpl->itsImmediateEvents.begin(), itsImpl->itsImmediateEvents.end());

  WriteUtils::writeObjectSeq(writer, "startEvents",
								 itsImpl->itsStartEvents.begin(), itsImpl->itsStartEvents.end());

  WriteUtils::writeObjectSeq(writer, "responseEvents",
								 itsImpl->itsResponseEvents.begin(), itsImpl->itsResponseEvents.end());

  WriteUtils::writeObjectSeq(writer, "abortEvents",
								 itsImpl->itsAbortEvents.begin(), itsImpl->itsAbortEvents.end());
}

///////////////
// accessors //
///////////////

int TimingHdlr::getAutosavePeriod() const {
DOTRACE("TimingHdlr::getAutosavePeriod");
  return itsImpl->itsAutosavePeriod;
}

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
  TrialEvent* e = dynamic_cast<TrialEvent*>(IoMgr::newIO(event_type));
  if (e == 0) { return -1; }

  e->setDelay(msec_delay);
  return addEvent(e, timepoint);
}

void TimingHdlr::setAutosavePeriod(int val) {
DOTRACE("TimingHdlr::setAutosavePeriod");
  itsImpl->itsAutosavePeriod = val;
}

///////////////////////////////////////////////////////////////////////
//
// TimingHdlr action method definitions
//
///////////////////////////////////////////////////////////////////////

void TimingHdlr::thBeginTrial(Experiment* expt) {
DOTRACE("TimingHdlr::thBeginTrial");

  itsImpl->itsTimer.restart(); 

  cancelAll(itsImpl->itsResponseEvents);
  cancelAll(itsImpl->itsAbortEvents);
  scheduleAll(itsImpl->itsImmediateEvents, expt);
  scheduleAll(itsImpl->itsStartEvents, expt);
}

void TimingHdlr::thResponseSeen(Experiment* expt) {
DOTRACE("TimingHdlr::thResponseSeen");
  if (itsImpl->itsResponseEvents.size() > 0) { 
	 cancelAll(itsImpl->itsStartEvents);
	 scheduleAll(itsImpl->itsResponseEvents, expt);
  }
}

void TimingHdlr::thAbortTrial(Experiment* expt) {
DOTRACE("TimingHdlr::thAbortTrial");
  cancelAll(itsImpl->itsStartEvents);
  scheduleAll(itsImpl->itsAbortEvents, expt);
}

void TimingHdlr::thHaltExpt(Experiment* /*expt*/) {
DOTRACE("TimingHdlr::thHaltExpt");
  cancelAll(itsImpl->itsStartEvents);
  cancelAll(itsImpl->itsResponseEvents);
  cancelAll(itsImpl->itsAbortEvents);
}

static const char vcid_timinghdlr_cc[] = "$Header$";
#endif // !TIMINGHDLR_CC_DEFINED
