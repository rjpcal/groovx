///////////////////////////////////////////////////////////////////////
//
// timinghandler.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed May 19 21:39:51 1999
// written: Wed Mar 19 12:46:28 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TIMINGHANDLER_CC_DEFINED
#define TIMINGHANDLER_CC_DEFINED

#include "visx/timinghandler.h"

#include "visx/trialevent.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include "util/ref.h"

#include "util/trace.h"
#include "util/debug.h"

namespace
{
  const IO::VersionId TIMINGHANDLER_SERIAL_VERSION_ID = 2;
}

///////////////////////////////////////////////////////////////////////
//
// TimingHandler creator method definitions
//
///////////////////////////////////////////////////////////////////////

TimingHandler* TimingHandler::make()
{
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

TimingHandler::~TimingHandler()
{
DOTRACE("TimingHandler::~TimingHandler");
}

IO::VersionId TimingHandler::serialVersionId() const
{
DOTRACE("TimingHandler::serialVersionId");
 return TIMINGHANDLER_SERIAL_VERSION_ID;
}

void TimingHandler::readFrom(IO::Reader* reader)
{
DOTRACE("TimingHandler::readFrom");

  reader->ensureReadVersionId("TimingHandler", 2, "Try grsh0.8a4");

  reader->readBaseClass("TimingHdlr", IO::makeProxy<TimingHdlr>(this));
}

void TimingHandler::writeTo(IO::Writer* writer) const
{
DOTRACE("TimingHandler::writeTo");

  writer->ensureWriteVersionId("TimingHandler",
            TIMINGHANDLER_SERIAL_VERSION_ID, 2, "Try grsh0.8a4");

  writer->writeBaseClass("TimingHdlr", IO::makeConstProxy<TimingHdlr>(this));
}

int TimingHandler::getAbortWait() const
{
  return getEvent(FROM_ABORT, abortwait_abort_id)->getDelay();
}

int TimingHandler::getInterTrialInterval() const
{
  return getEvent(FROM_RESPONSE, iti_response_id)->getDelay();
}

int TimingHandler::getStimDur() const
{
  return getEvent(FROM_START, stimdur_start_id)->getDelay();
}

int TimingHandler::getTimeout() const
{
  return getEvent(FROM_START, timeout_start_id)->getDelay();
}

void TimingHandler::setAbortWait(int msec)
{
  getEvent(FROM_ABORT, abortwait_abort_id)->setDelay(msec);
}

void TimingHandler::setInterTrialInterval(int msec)
{
  getEvent(FROM_RESPONSE, iti_response_id)->setDelay(msec);
}

void TimingHandler::setStimDur(int msec)
{
  getEvent(FROM_START, stimdur_start_id)->setDelay(msec);
}

void TimingHandler::setTimeout(int msec)
{
  getEvent(FROM_START, timeout_start_id)->setDelay(msec);
}

static const char vcid_timinghandler_cc[] = "$Header$";
#endif // !TIMINGHANDLER_CC_DEFINED
