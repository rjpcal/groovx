///////////////////////////////////////////////////////////////////////
//
// timinghdlr.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun 21 13:09:57 1999
// written: Wed Dec  1 14:26:48 1999
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

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

const TimingHdlr::TimePoint TimingHdlr::IMMEDIATE;
const TimingHdlr::TimePoint TimingHdlr::FROM_START;
const TimingHdlr::TimePoint TimingHdlr::FROM_RESPONSE;
const TimingHdlr::TimePoint TimingHdlr::FROM_ABORT;

namespace {
  const string ioTag = "TimingHdlr";

  void scheduleAll(vector<TrialEvent*>& events, Experiment* expt) {
  DOTRACE("scheduleAll");
    for (int i = 0; i < events.size(); ++i) {
		events[i]->schedule(expt);
	 }
#if 0
	 for_each(events.begin(), events.end(), mem_fun(&TrialEvent::schedule));
#endif
  }

  void cancelAll(vector<TrialEvent*>& events) {
  DOTRACE("cancelAll");
    for (int i = 0; i < events.size(); ++i) {
		events[i]->cancel();
	 }
#if 0
	 for_each(events.begin(), events.end(), mem_fun(&TrialEvent::cancel));
#endif
  }

  void deleteAll(vector<TrialEvent*>& events) {
  DOTRACE("deleteAll");
    for (int i = 0; i < events.size(); ++i) {
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
  itsAutosavePeriod(10)
{
DOTRACE("TimingHdlr::TimingHdlr");
}

TimingHdlr::~TimingHdlr() {
DOTRACE("TimingHdlr::~TimingHdlr");
  deleteAll(itsImmediateEvents);
  deleteAll(itsStartEvents);
  deleteAll(itsResponseEvents);
  deleteAll(itsAbortEvents);
}

void TimingHdlr::serialize(ostream &os, IOFlag flag) const {
DOTRACE("TimingHdlr::serialize");
  if (flag & BASES) { /* no bases to serialize */ }

  char sep = ' ';
  if (flag & TYPENAME) { os << ioTag << sep; }
  
  os << itsAutosavePeriod << sep;

  size_t i;

  os << itsImmediateEvents.size() << sep;
  for (i = 0; i < itsImmediateEvents.size(); ++i) {
	 itsImmediateEvents[i]->serialize(os, flag);
  }

  os << itsStartEvents.size() << sep;
  for (i = 0; i < itsStartEvents.size(); ++i) {
	 itsStartEvents[i]->serialize(os, flag);
  }

  os << itsResponseEvents.size() << sep;
  for (i = 0; i < itsResponseEvents.size(); ++i) {
	 itsResponseEvents[i]->serialize(os, flag);
  }

  os << itsAbortEvents.size() << sep;
  for (i = 0; i < itsAbortEvents.size(); ++i) {
	 itsAbortEvents[i]->serialize(os, flag);
  }

  if (os.fail()) throw OutputError(ioTag);
}

void TimingHdlr::deserialize(istream &is, IOFlag flag) {
DOTRACE("TimingHdlr::deserialize");
  if (flag & BASES) { /* no bases to deserialize */ }
  if (flag & TYPENAME) { IO::readTypename(is, ioTag); }

  is >> itsAutosavePeriod;
  DebugEvalNL(itsAutosavePeriod);

  int size, i;

  const int SIZE_SANITY_CHECK = 1000;

  deleteAll(itsImmediateEvents);
  is >> size;
  DebugEvalNL(size);
  if (size < 0 || size > SIZE_SANITY_CHECK) throw IoLogicError(ioTag);
  itsImmediateEvents.resize(size);
  for (i = 0; i < size; ++i) {
	 TrialEvent* e = dynamic_cast<TrialEvent*>(IoMgr::newIO(is, flag));
	 if (!e) throw InputError(ioTag);
	 itsImmediateEvents[i] = e;
  }

  deleteAll(itsStartEvents);
  is >> size;
  DebugEvalNL(size);
  if (size < 0 || size > SIZE_SANITY_CHECK) throw IoLogicError(ioTag);
  itsStartEvents.resize(size);
  for (i = 0; i < size; ++i) {
	 TrialEvent* e = dynamic_cast<TrialEvent*>(IoMgr::newIO(is, flag));
	 if (!e) throw InputError(ioTag);
	 itsStartEvents[i] = e;
  }

  deleteAll(itsResponseEvents);
  is >> size;
  DebugEvalNL(size);
  if (size < 0 || size > SIZE_SANITY_CHECK) throw IoLogicError(ioTag);
  itsResponseEvents.resize(size);
  for (i = 0; i < size; ++i) {
	 TrialEvent* e = dynamic_cast<TrialEvent*>(IoMgr::newIO(is, flag));
	 if (!e) throw InputError(ioTag);
	 itsResponseEvents[i] = e;
  }

  deleteAll(itsAbortEvents);
  is >> size;
  DebugEvalNL(size);
  if (size < 0 || size > SIZE_SANITY_CHECK) throw IoLogicError(ioTag);
  itsAbortEvents.resize(size);
  for (i = 0; i < size; ++i) {
	 TrialEvent* e = dynamic_cast<TrialEvent*>(IoMgr::newIO(is, flag));
	 if (!e) throw InputError(ioTag);
	 itsAbortEvents[i] = e;
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
  reader->readValue("autosavePeriod", itsAutosavePeriod);

  deleteAll(itsImmediateEvents);
  ReadUtils::readObjectSeq(reader, "immediateEvents",
								back_inserter(itsImmediateEvents), (TrialEvent*)0);

  deleteAll(itsStartEvents);
  ReadUtils::readObjectSeq(reader, "startEvents",
								back_inserter(itsStartEvents), (TrialEvent*)0);

  deleteAll(itsResponseEvents);
  ReadUtils::readObjectSeq(reader, "responseEvents",
								back_inserter(itsResponseEvents), (TrialEvent*)0);

  deleteAll(itsAbortEvents);
  ReadUtils::readObjectSeq(reader, "abortEvents",
								back_inserter(itsAbortEvents), (TrialEvent*)0);
}

void TimingHdlr::writeTo(Writer* writer) const {
DOTRACE("TimingHdlr::writeTo");
  writer->writeValue("autosavePeriod", itsAutosavePeriod);

  WriteUtils::writeObjectSeq(writer, "immediateEvents",
								 itsImmediateEvents.begin(), itsImmediateEvents.end());

  WriteUtils::writeObjectSeq(writer, "startEvents",
								 itsStartEvents.begin(), itsStartEvents.end());

  WriteUtils::writeObjectSeq(writer, "responseEvents",
								 itsResponseEvents.begin(), itsResponseEvents.end());

  WriteUtils::writeObjectSeq(writer, "abortEvents",
								 itsAbortEvents.begin(), itsAbortEvents.end());
}

///////////////
// accessors //
///////////////

TrialEvent* TimingHdlr::getEvent(TimePoint time_point, int index) const {
DOTRACE("TimingHdlr::getEvent");
  switch (time_point) {
  case IMMEDIATE:
	 return itsImmediateEvents[index];
	 break;
  case FROM_START:
	 return itsStartEvents[index];
	 break;
  case FROM_RESPONSE:
	 return itsResponseEvents[index];
	 break;
  case FROM_ABORT:
	 return itsAbortEvents[index];
	 break;
  default:
	 return 0;
	 break;
  }
}

int TimingHdlr::getElapsedMsec() const {
DOTRACE("TimingHdlr::getElapsedMsec");
  return itsTimer.elapsedMsec();
}

//////////////////
// manipulators //
//////////////////

int TimingHdlr::addEvent(TrialEvent* event, TimePoint time_point) {
DOTRACE("TimingHdlr::addEvent");
  switch (time_point) {
  case IMMEDIATE:
	 event->setDelay(0);
	 itsImmediateEvents.push_back(event);
	 return itsImmediateEvents.size() - 1;
	 break;
  case FROM_START:
	 itsStartEvents.push_back(event);
	 return itsStartEvents.size() - 1;
	 break;
  case FROM_RESPONSE:
	 itsResponseEvents.push_back(event);
	 return itsResponseEvents.size() - 1;
	 break;
  case FROM_ABORT:
	 itsAbortEvents.push_back(event);
	 return itsAbortEvents.size() - 1;
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

///////////////////////////////////////////////////////////////////////
//
// TimingHdlr action method definitions
//
///////////////////////////////////////////////////////////////////////

void TimingHdlr::thBeginTrial(Experiment* expt) {
DOTRACE("TimingHdlr::thBeginTrial");

  itsTimer.restart(); 

  cancelAll(itsResponseEvents);
  cancelAll(itsAbortEvents);
  scheduleAll(itsImmediateEvents, expt);
  scheduleAll(itsStartEvents, expt);
}

void TimingHdlr::thResponseSeen(Experiment* expt) {
DOTRACE("TimingHdlr::thResponseSeen");
  if (itsResponseEvents.size() > 0) { 
	 cancelAll(itsStartEvents);
	 scheduleAll(itsResponseEvents, expt);
  }
}

void TimingHdlr::thAbortTrial(Experiment* expt) {
DOTRACE("TimingHdlr::thAbortTrial");
  cancelAll(itsStartEvents);
  scheduleAll(itsAbortEvents, expt);
}

void TimingHdlr::thHaltExpt(Experiment* expt) {
DOTRACE("TimingHdlr::thHaltExpt");
  cancelAll(itsStartEvents);
  cancelAll(itsResponseEvents);
  cancelAll(itsAbortEvents);
}

static const char vcid_timinghdlr_cc[] = "$Header$";
#endif // !TIMINGHDLR_CC_DEFINED
