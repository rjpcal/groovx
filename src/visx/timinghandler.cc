///////////////////////////////////////////////////////////////////////
//
// timinghandler.cc
// Rob Peters
// created: Wed May 19 21:39:51 1999
// written: Wed Sep 27 11:41:16 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TIMINGHANDLER_CC_DEFINED
#define TIMINGHANDLER_CC_DEFINED

#include "timinghandler.h"

#include "io/iolegacy.h"
#include "trialevent.h"

#include <cstring>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace {
  const char* ioTag = "TimingHandler";
}

///////////////////////////////////////////////////////////////////////
//
// TimingHandler creator method definitions
//
///////////////////////////////////////////////////////////////////////

TimingHandler::TimingHandler() :
  stimdur_start_id(0),
  timeout_start_id(0),
  iti_response_id(0),
  abortwait_abort_id(0)
{
DOTRACE("TimingHandler::TimingHandler");
  addEvent(new DrawEvent(0), IMMEDIATE);
  stimdur_start_id = addEvent(new UndrawEvent(2000), FROM_START);
  timeout_start_id = addEvent(new AbortTrialEvent(4000), FROM_START);
  addEvent(new UndrawEvent(0), FROM_RESPONSE);
  iti_response_id = addEvent(new EndTrialEvent(1000), FROM_RESPONSE);
  abortwait_abort_id = addEvent(new EndTrialEvent(1000), FROM_ABORT);
}

TimingHandler::~TimingHandler() {
DOTRACE("TimingHandler::~TimingHandler");
}

void TimingHandler::legacySrlz(IO::Writer* writer) const {
DOTRACE("TimingHandler::legacySrlz");
  IO::LegacyWriter* lwriter = dynamic_cast<IO::LegacyWriter*>(writer);
  if (lwriter != 0) {
	 ostream& os = lwriter->output();
	 char sep = ' ';
	 if (lwriter->flags() & IO::TYPENAME) { os << ioTag << sep; }
  
	 {
		IO::LWFlagJanitor jtr_(*lwriter, lwriter->flags() | IO::TYPENAME);
		TimingHdlr::legacySrlz(writer);
	 }

	 if (os.fail()) throw IO::OutputError(ioTag);
  }
}

void TimingHandler::legacyDesrlz(IO::Reader* reader) {
DOTRACE("TimingHandler::legacyDesrlz");
  IO::LegacyReader* lreader = dynamic_cast<IO::LegacyReader*>(reader); 
  if (lreader != 0) {
	 istream& is = lreader->input();
	 if (lreader->flags() & IO::TYPENAME) { IO::IoObject::readTypename(is, ioTag); }

	 {
		IO::LRFlagJanitor jtr_(*lreader, lreader->flags() | IO::TYPENAME);
		TimingHdlr::legacyDesrlz(reader);
	 }

	 if (is.fail()) throw IO::InputError(ioTag);
  }
}

void TimingHandler::readFrom(IO::Reader* reader) {
DOTRACE("TimingHandler::readFrom");
  TimingHdlr::readFrom(reader);
}

void TimingHandler::writeTo(IO::Writer* writer) const {
DOTRACE("TimingHandler::writeTo");
  TimingHdlr::writeTo(writer);
}

int TimingHandler::getAbortWait() const { 
  return getEvent(FROM_ABORT, abortwait_abort_id)->getDelay();
}
int TimingHandler::getInterTrialInterval() const { 
  return getEvent(FROM_RESPONSE, iti_response_id)->getDelay();
}
int TimingHandler::getStimDur() const { 
  return getEvent(FROM_START, stimdur_start_id)->getDelay();
}
int TimingHandler::getTimeout() const { 
  return getEvent(FROM_START, timeout_start_id)->getDelay();
}

void TimingHandler::setAbortWait(int val) { 
  getEvent(FROM_ABORT, abortwait_abort_id)->setDelay(val);
}
void TimingHandler::setInterTrialInterval(int val) { 
  getEvent(FROM_RESPONSE, iti_response_id)->setDelay(val);
}
void TimingHandler::setStimDur(int val) { 
  getEvent(FROM_START, stimdur_start_id)->setDelay(val);
}
void TimingHandler::setTimeout(int val) { 
  getEvent(FROM_START, timeout_start_id)->setDelay(val);
}

static const char vcid_timinghandler_cc[] = "$Header$";
#endif // !TIMINGHANDLER_CC_DEFINED
