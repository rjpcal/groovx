///////////////////////////////////////////////////////////////////////
//
// timinghandler.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed May 19 21:39:51 1999
// written: Tue Nov 14 15:10:59 2000
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
  addEventByName("DrawEvent", IMMEDIATE, 0);
  stimdur_start_id = addEventByName("UndrawEvent", FROM_START, 2000);
  timeout_start_id = addEventByName("AbortTrialEvent", FROM_START, 4000);
  addEventByName("UndrawEvent", FROM_RESPONSE, 0);
  iti_response_id = addEventByName("EndTrialEvent", FROM_RESPONSE, 1000);
  abortwait_abort_id = addEventByName("EndTrialEvent", FROM_ABORT, 1000);
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

  reader->ensureReadVersionId("TimingHandler", 2, "Try grsh0.8a4");

  IO::IoProxy<TimingHdlr> baseclass(this);
  reader->readBaseClass("TimingHdlr", &baseclass);
}

void TimingHandler::writeTo(IO::Writer* writer) const {
DOTRACE("TimingHandler::writeTo");

  writer->ensureWriteVersionId("TimingHandler",
		      TIMINGHANDLER_SERIAL_VERSION_ID, 2, "Try grsh0.8a4");

  IO::ConstIoProxy<TimingHdlr> baseclass(this);
  writer->writeBaseClass("TimingHdlr", &baseclass);
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
