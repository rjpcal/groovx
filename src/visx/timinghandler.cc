///////////////////////////////////////////////////////////////////////
//
// timinghandler.cc
// Rob Peters
// created: Wed May 19 21:39:51 1999
// written: Wed Mar 15 10:17:25 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TIMINGHANDLER_CC_DEFINED
#define TIMINGHANDLER_CC_DEFINED

#include "timinghandler.h"

#include "trialevent.h"

#include <cstring>
#include <iostream.h>

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

void TimingHandler::serialize(ostream &os, IOFlag flag) const {
DOTRACE("TimingHandler::serialize");
  char sep = ' ';
  if (flag & TYPENAME) { os << ioTag << sep; }
  
  if (true || (flag & BASES)) { TimingHdlr::serialize(os, flag|TYPENAME); }

  if (os.fail()) throw OutputError(ioTag);
}

void TimingHandler::deserialize(istream &is, IOFlag flag) {
DOTRACE("TimingHandler::deserialize");
  if (flag & TYPENAME) { IO::readTypename(is, ioTag); }

  if (true || (flag & BASES)) { TimingHdlr::deserialize(is, flag|TYPENAME); }

  if (is.fail()) throw InputError(ioTag);
}

int TimingHandler::charCount() const {
DOTRACE("TimingHandler::charCount"); 
  return (strlen(ioTag) + 1
			 + TimingHdlr::charCount()
			 + 1); // fudge factor
}

void TimingHandler::readFrom(Reader* reader) {
DOTRACE("TimingHandler::readFrom");
  TimingHdlr::readFrom(reader);
}

void TimingHandler::writeTo(Writer* writer) const {
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
