/** @file visx/tclpkg-th.cc tcl interface package for class TimingHdlr
    and derivatives */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Jun  9 20:39:46 1999
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

#include "visx/tclpkg-th.h"

#include "nub/objfactory.h"

#include "tcl/objpkg.h"
#include "tcl/pkg.h"

#include "visx/timinghandler.h"
#include "visx/timinghdlr.h"
#include "visx/trialevent.h"

#include "rutz/trace.h"

namespace
{
  size_t addNewEvent(nub::ref<TimingHdlr> th, const char* event_type,
                     unsigned int msec, TimingHdlr::TimePoint time_point)
  {
    return th->addEventByName(event_type, time_point, msec);
  }
}

extern "C"
int Timinghdlr_Init(Tcl_Interp* interp)
{
GVX_TRACE("Timinghdlr_Init");

  return tcl::pkg::init
    (interp, "TimingHdlr", "4.0",
     [](tcl::pkg* pkg) {
      tcl::def_creator<TimingHdlr>(pkg);
      pkg->inherit_pkg("io");
      tcl::def_basic_type_cmds<TimingHdlr>(pkg, SRC_POS);

      pkg->def( "addImmediateEvent", "th_id event_type msec_delay",
                rutz::bind_last(&addNewEvent, TimingHdlr::IMMEDIATE),
                SRC_POS );
      pkg->def( "addStartEvent", "th_id event_type msec_delay",
                rutz::bind_last(&addNewEvent, TimingHdlr::FROM_START),
                SRC_POS );
      pkg->def( "addResponseEvent", "th_id event_type msec_delay",
                rutz::bind_last(&addNewEvent, TimingHdlr::FROM_RESPONSE),
                SRC_POS );
      pkg->def( "addAbortEvent", "th_id event_type msec_delay",
                rutz::bind_last(&addNewEvent, TimingHdlr::FROM_ABORT),
                SRC_POS );

      pkg->def( "addImmediateEvent", "th_id event_id",
                rutz::bind_last(rutz::mem_func(&TimingHdlr::addEvent),
                                TimingHdlr::IMMEDIATE),
                SRC_POS );
      pkg->def( "addStartEvent", "th_id event_id",
                rutz::bind_last(rutz::mem_func(&TimingHdlr::addEvent),
                                TimingHdlr::FROM_START),
                SRC_POS );
      pkg->def( "addResponseEvent", "th_id event_id",
                rutz::bind_last(rutz::mem_func(&TimingHdlr::addEvent),
                                TimingHdlr::FROM_RESPONSE),
                SRC_POS );
      pkg->def( "addAbortEvent", "th_id event_id",
                rutz::bind_last(rutz::mem_func(&TimingHdlr::addEvent),
                                TimingHdlr::FROM_ABORT),
                SRC_POS );

      pkg->namesp_alias("Th");
    });
}

extern "C"
int Timinghandler_Init(Tcl_Interp* interp)
{
GVX_TRACE("Timinghandler_Init");

  return tcl::pkg::init
    (interp, "TimingHandler", "4.0",
     [](tcl::pkg* pkg) {
      tcl::def_creator<TimingHandler>(pkg);
      pkg->inherit_pkg("io");
      tcl::def_basic_type_cmds<TimingHandler>(pkg, SRC_POS);

      pkg->def_get_set("abortWait",
                       &TimingHandler::getAbortWait,
                       &TimingHandler::setAbortWait,
                       SRC_POS);
      pkg->def_get_set("interTrialInterval",
                       &TimingHandler::getInterTrialInterval,
                       &TimingHandler::setInterTrialInterval,
                       SRC_POS);
      pkg->def_get_set("stimDur",
                       &TimingHandler::getStimDur,
                       &TimingHandler::setStimDur,
                       SRC_POS);
      pkg->def_get_set("timeout",
                       &TimingHandler::getTimeout,
                       &TimingHandler::setTimeout,
                       SRC_POS);

      pkg->namesp_alias("SimpleTh");
    });
}
