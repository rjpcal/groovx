///////////////////////////////////////////////////////////////////////
//
// timinghandler.cc
// Rob Peters
// created: Wed May 19 21:39:51 1999
// written: Tue Sep 26 19:12:26 2000
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

void TimingHandler::legacySrlz(IO::Writer* writer, STD_IO::ostream &os, IO::IOFlag flag) const {
DOTRACE("TimingHandler::legacySrlz");
  char sep = ' ';
  if (flag & IO::TYPENAME) { os << ioTag << sep; }
  
  if (true || (flag & IO::BASES)) { TimingHdlr::legacySrlz(writer, os, flag|IO::TYPENAME); }

  if (os.fail()) throw IO::OutputError(ioTag);
}

void TimingHandler::legacyDesrlz(IO::Reader* reader, STD_IO::istream &is, IO::IOFlag flag) {
DOTRACE("TimingHandler::legacyDesrlz");
  if (flag & IO::TYPENAME) { IO::IoObject::readTypename(is, ioTag); }

  if (true || (flag & IO::BASES)) { TimingHdlr::legacyDesrlz(reader, is, flag|IO::TYPENAME); }

  if (is.fail()) throw IO::InputError(ioTag);
}

int TimingHandler::legacyCharCount() const {
DOTRACE("TimingHandler::legacyCharCount"); 
  return (strlen(ioTag) + 1
			 + TimingHdlr::legacyCharCount()
			 + 1); // fudge factor
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
