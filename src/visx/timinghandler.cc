///////////////////////////////////////////////////////////////////////
//
// timinghandler.cc
// Rob Peters
// created: Wed May 19 21:39:51 1999
// written: Fri Oct 20 17:46:15 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TIMINGHANDLER_CC_DEFINED
#define TIMINGHANDLER_CC_DEFINED

#include "timinghandler.h"

#include "trialevent.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include <cstring>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace {
  const IO::VersionId TIMINGHANDLER_SERIAL_VERSION_ID = 2;
}

///////////////////////////////////////////////////////////////////////
//
// TimingHandler creator method definitions
//
///////////////////////////////////////////////////////////////////////

TimingHandler* TimingHandler::make() {
DOTRACE("TimingHandler::make");
  return new TimingHandler;
}

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

IO::VersionId TimingHandler::serialVersionId() const {
DOTRACE("TimingHandler::serialVersionId");
 return TIMINGHANDLER_SERIAL_VERSION_ID;
}

void TimingHandler::readFrom(IO::Reader* reader) {
DOTRACE("TimingHandler::readFrom");

  IO::VersionId svid = reader->readSerialVersionId();
  if (svid < 2)
	 TimingHdlr::readFrom(reader);
  else if (svid == 2)
	 {
		IO::IoProxy<TimingHdlr> baseclass(this);
		reader->readBaseClass("TimingHdlr", &baseclass);
	 }
}

void TimingHandler::writeTo(IO::Writer* writer) const {
DOTRACE("TimingHandler::writeTo");

  if (TIMINGHANDLER_SERIAL_VERSION_ID < 2)
	 TimingHdlr::writeTo(writer);
  else if (TIMINGHANDLER_SERIAL_VERSION_ID == 2)
	 {
		IO::ConstIoProxy<TimingHdlr> baseclass(this);
		writer->writeBaseClass("TimingHdlr", &baseclass);
	 }
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
