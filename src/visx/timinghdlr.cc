///////////////////////////////////////////////////////////////////////
//
// timinghdlr.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun 21 13:09:57 1999
// written: Tue Oct  5 16:32:07 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TIMINGHDLR_CC_DEFINED
#define TIMINGHDLR_CC_DEFINED

#include "timinghdlr.h"

#include "iomgr.h"
#include "trialevent.h"
#include "timeutils.h"

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

  int i;

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

  itsImmediateEvents.clear();
  is >> size;
  DebugEvalNL(size);
  if (size < 0 || size > SIZE_SANITY_CHECK) throw IoLogicError(ioTag);
  itsImmediateEvents.resize(size);
  for (i = 0; i < size; ++i) {
	 TrialEvent* e = dynamic_cast<TrialEvent*>(IoMgr::newIO(is, flag));
	 if (!e) throw InputError(ioTag);
	 itsImmediateEvents[i] = e;
  }

  itsStartEvents.clear();
  is >> size;
  DebugEvalNL(size);
  if (size < 0 || size > SIZE_SANITY_CHECK) throw IoLogicError(ioTag);
  itsStartEvents.resize(size);
  for (i = 0; i < size; ++i) {
	 TrialEvent* e = dynamic_cast<TrialEvent*>(IoMgr::newIO(is, flag));
	 if (!e) throw InputError(ioTag);
	 itsStartEvents[i] = e;
  }

  itsResponseEvents.clear();
  is >> size;
  DebugEvalNL(size);
  if (size < 0 || size > SIZE_SANITY_CHECK) throw IoLogicError(ioTag);
  itsResponseEvents.resize(size);
  for (i = 0; i < size; ++i) {
	 TrialEvent* e = dynamic_cast<TrialEvent*>(IoMgr::newIO(is, flag));
	 if (!e) throw InputError(ioTag);
	 itsResponseEvents[i] = e;
  }

  itsAbortEvents.clear();
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

  int i;
  int ch_count = ioTag.length() + 1
	 + gCharCount<int>(itsAutosavePeriod) + 1;

  for (i = 0; i < itsImmediateEvents.size(); ++i) {
  }

  return 256; 
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
  return elapsedMsecSince(itsBeginTime);
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

void TimingHdlr::thBeginTrial() {
DOTRACE("TimingHdlr::thBeginTrial");

  gettimeofday(&itsBeginTime, NULL);

  cancelAll(itsResponseEvents);
  cancelAll(itsAbortEvents);
  scheduleAll(itsImmediateEvents);
  scheduleAll(itsStartEvents);
}

void TimingHdlr::thResponseSeen() {
DOTRACE("TimingHdlr::thResponseSeen");
  if (itsResponseEvents.size() > 0) { 
	 cancelAll(itsStartEvents);
	 scheduleAll(itsResponseEvents);
  }
}

void TimingHdlr::thAbortTrial() {
DOTRACE("TimingHdlr::thAbortTrial");
  cancelAll(itsStartEvents);
  scheduleAll(itsAbortEvents);
}

void TimingHdlr::thHaltExpt() {
DOTRACE("TimingHdlr::thHaltExpt");
  cancelAll(itsStartEvents);
  cancelAll(itsResponseEvents);
  cancelAll(itsAbortEvents);
}

///////////////////////////////////////////////////////////////////////
//
// TimingHdlr private utility functions
//
///////////////////////////////////////////////////////////////////////

void TimingHdlr::scheduleAll(vector<TrialEvent*>& events) {
DOTRACE("TimingHdlr::scheduleAll");
  for (int i = 0; i < events.size(); ++i) {
	 events[i]->schedule();
  }
#if 0
  for_each(events.begin(), events.end(), mem_fun(&TrialEvent::schedule));
#endif
}

void TimingHdlr::cancelAll(vector<TrialEvent*>& events) {
DOTRACE("TimingHdlr::cancelAll");
  for (int i = 0; i < events.size(); ++i) {
	 events[i]->cancel();
  }
#if 0
  for_each(events.begin(), events.end(), mem_fun(&TrialEvent::cancel));
#endif
}

void TimingHdlr::deleteAll(vector<TrialEvent*>& events) {
DOTRACE("TimingHdlr::deleteAll");
  for (int i = 0; i < events.size(); ++i) {
	 delete events[i];
	 events[i] = 0;
  }
  events.resize(0);
}

void TimingHdlr::cancelAll() {
DOTRACE("TimingHdlr::cancelAll");
  cancelAll(itsImmediateEvents);
  cancelAll(itsStartEvents);
  cancelAll(itsResponseEvents);
  cancelAll(itsAbortEvents);
}

static const char vcid_timinghdlr_cc[] = "$Header$";
#endif // !TIMINGHDLR_CC_DEFINED
