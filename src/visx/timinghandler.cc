///////////////////////////////////////////////////////////////////////
//
// timinghandler.cc
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed May 19 21:39:51 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef TIMINGHANDLER_CC_DEFINED
#define TIMINGHANDLER_CC_DEFINED

#include "timinghandler.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include "nub/ref.h"

#include "visx/trialevent.h"

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

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

TimingHandler::~TimingHandler() throw()
{
DOTRACE("TimingHandler::~TimingHandler");
}

IO::VersionId TimingHandler::serialVersionId() const
{
DOTRACE("TimingHandler::serialVersionId");
 return TIMINGHANDLER_SERIAL_VERSION_ID;
}

void TimingHandler::readFrom(IO::Reader& reader)
{
DOTRACE("TimingHandler::readFrom");

  reader.ensureReadVersionId("TimingHandler", 2, "Try groovx0.8a4", SRC_POS);

  reader.readBaseClass("TimingHdlr", IO::makeProxy<TimingHdlr>(this));
}

void TimingHandler::writeTo(IO::Writer& writer) const
{
DOTRACE("TimingHandler::writeTo");

  writer.ensureWriteVersionId("TimingHandler",
            TIMINGHANDLER_SERIAL_VERSION_ID, 2, "Try groovx0.8a4", SRC_POS);

  writer.writeBaseClass("TimingHdlr", IO::makeConstProxy<TimingHdlr>(this));
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
