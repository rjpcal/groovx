/** @file visx/timinghandler.cc specialized TimingHdlr subclass for
    object categorization experiments */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed May 19 21:39:51 1999
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#include "timinghandler.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include "nub/ref.h"

#include "visx/trialevent.h"

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

namespace
{
  const io::version_id TIMINGHANDLER_SVID = 2;
}

///////////////////////////////////////////////////////////////////////
//
// TimingHandler creator method definitions
//
///////////////////////////////////////////////////////////////////////

TimingHandler* TimingHandler::make()
{
GVX_TRACE("TimingHandler::make");
  return new TimingHandler;
}

TimingHandler::TimingHandler() :
  stimdur_start_id(0),
  timeout_start_id(0),
  iti_response_id(0),
  abortwait_abort_id(0)
{
GVX_TRACE("TimingHandler::TimingHandler");
  addEventByName("DrawEvent", IMMEDIATE, 0);
  stimdur_start_id = addEventByName("UndrawEvent", FROM_START, 2000);
  timeout_start_id = addEventByName("AbortTrialEvent", FROM_START, 4000);
  addEventByName("UndrawEvent", FROM_RESPONSE, 0);
  iti_response_id = addEventByName("EndTrialEvent", FROM_RESPONSE, 1000);
  abortwait_abort_id = addEventByName("EndTrialEvent", FROM_ABORT, 1000);
}

TimingHandler::~TimingHandler() noexcept
{
GVX_TRACE("TimingHandler::~TimingHandler");
}

io::version_id TimingHandler::class_version_id() const
{
GVX_TRACE("TimingHandler::class_version_id");
 return TIMINGHANDLER_SVID;
}

void TimingHandler::read_from(io::reader& reader)
{
GVX_TRACE("TimingHandler::read_from");

  reader.ensure_version_id("TimingHandler", 2, "Try groovx0.8a4", SRC_POS);

  reader.read_base_class("TimingHdlr", io::make_proxy<TimingHdlr>(this));
}

void TimingHandler::write_to(io::writer& writer) const
{
GVX_TRACE("TimingHandler::write_to");

  writer.ensure_output_version_id("TimingHandler",
            TIMINGHANDLER_SVID, 2, "Try groovx0.8a4", SRC_POS);

  writer.write_base_class("TimingHdlr", io::make_const_proxy<TimingHdlr>(this));
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
