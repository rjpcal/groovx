///////////////////////////////////////////////////////////////////////
//
// thtcl.cc
//
// Copyright (c) 1999-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Jun  9 20:39:46 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef THTCL_CC_DEFINED
#define THTCL_CC_DEFINED

#include "tcl/objpkg.h"
#include "tcl/tclpkg.h"

#include "util/objfactory.h"

#include "visx/timinghandler.h"
#include "visx/timinghdlr.h"
#include "visx/trialevent.h"

#include "util/trace.h"

namespace
{
  unsigned int addNewEvent(Util::Ref<TimingHdlr> th, const char* event_type,
                           int msec, TimingHdlr::TimePoint time_point)
  {
    return th->addEventByName(event_type, time_point, msec);
  }
}

extern "C"
int Timinghdlr_Init(Tcl_Interp* interp)
{
DOTRACE("Timinghdlr_Init");

  PKG_CREATE(interp, "TimingHdlr", "$Revision$");
  Tcl::defCreator<TimingHdlr>(pkg);
  pkg->inheritPkg("IO");
  Tcl::defGenericObjCmds<TimingHdlr>(pkg, SRC_POS);

  pkg->def( "addImmediateEvent", "th_id event_type msec_delay",
            Util::bindLast(&addNewEvent, TimingHdlr::IMMEDIATE),
            SRC_POS );
  pkg->def( "addStartEvent", "th_id event_type msec_delay",
            Util::bindLast(&addNewEvent, TimingHdlr::FROM_START),
            SRC_POS );
  pkg->def( "addResponseEvent", "th_id event_type msec_delay",
            Util::bindLast(&addNewEvent, TimingHdlr::FROM_RESPONSE),
            SRC_POS );
  pkg->def( "addAbortEvent", "th_id event_type msec_delay",
            Util::bindLast(&addNewEvent, TimingHdlr::FROM_ABORT),
            SRC_POS );

  pkg->def( "addImmediateEvent", "th_id event_id",
            Util::bindLast(Util::memFunc(&TimingHdlr::addEvent),
                           TimingHdlr::IMMEDIATE),
            SRC_POS );
  pkg->def( "addStartEvent", "th_id event_id",
            Util::bindLast(Util::memFunc(&TimingHdlr::addEvent),
                           TimingHdlr::FROM_START),
            SRC_POS );
  pkg->def( "addResponseEvent", "th_id event_id",
            Util::bindLast(Util::memFunc(&TimingHdlr::addEvent),
                           TimingHdlr::FROM_RESPONSE),
            SRC_POS );
  pkg->def( "addAbortEvent", "th_id event_id",
            Util::bindLast(Util::memFunc(&TimingHdlr::addEvent),
                           TimingHdlr::FROM_ABORT),
            SRC_POS );

  pkg->namespaceAlias("Th");

  PKG_RETURN;
}

extern "C"
int Timinghandler_Init(Tcl_Interp* interp)
{
DOTRACE("Timinghandler_Init");

  PKG_CREATE(interp, "TimingHandler", "$Revision$");
  Tcl::defCreator<TimingHandler>(pkg);
  pkg->inheritPkg("IO");
  Tcl::defGenericObjCmds<TimingHandler>(pkg, SRC_POS);

  pkg->defAttrib("abortWait",
                 &TimingHandler::getAbortWait,
                 &TimingHandler::setAbortWait,
                 SRC_POS);
  pkg->defAttrib("interTrialInterval",
                 &TimingHandler::getInterTrialInterval,
                 &TimingHandler::setInterTrialInterval,
                 SRC_POS);
  pkg->defAttrib("stimDur",
                 &TimingHandler::getStimDur,
                 &TimingHandler::setStimDur,
                 SRC_POS);
  pkg->defAttrib("timeout",
                 &TimingHandler::getTimeout,
                 &TimingHandler::setTimeout,
                 SRC_POS);

  pkg->namespaceAlias("SimpleTh");

  PKG_RETURN;
}

static const char vcid_thtcl_cc[] = "$Header$";
#endif // !THTCL_CC_DEFINED
