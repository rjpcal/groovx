///////////////////////////////////////////////////////////////////////
//
// thtcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jun  9 20:39:46 1999
// written: Sat Sep  8 14:08:48 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef THTCL_CC_DEFINED
#define THTCL_CC_DEFINED

#include "timinghandler.h"
#include "timinghdlr.h"
#include "trialevent.h"

#include "tcl/tclpkg.h"

#include "util/objfactory.h"

#include "util/trace.h"

namespace
{
  unsigned int addEvent(Util::Ref<TimingHdlr> th, const char* event_type,
                        int msec, TimingHdlr::TimePoint time_point)
  {
    return th->addEventByName(event_type, time_point, msec);
  }
}

extern "C"
int Th_Init(Tcl_Interp* interp)
{
DOTRACE("Th_Init");

  Util::ObjFactory::theOne().registerCreatorFunc(&TimingHdlr::make);
  Util::ObjFactory::theOne().registerCreatorFunc(&TimingHandler::make);

  Util::ObjFactory::theOne().registerCreatorFunc(&AbortTrialEvent::make);
  Util::ObjFactory::theOne().registerCreatorFunc(&DrawEvent::make);
  Util::ObjFactory::theOne().registerCreatorFunc(&UndrawEvent::make);
  Util::ObjFactory::theOne().registerCreatorFunc(&EndTrialEvent::make);
  Util::ObjFactory::theOne().registerCreatorFunc(&NextNodeEvent::make);
  Util::ObjFactory::theOne().registerCreatorFunc(&AllowResponsesEvent::make);
  Util::ObjFactory::theOne().registerCreatorFunc(&DenyResponsesEvent::make);
  Util::ObjFactory::theOne().registerCreatorFunc(&SwapBuffersEvent::make);
  Util::ObjFactory::theOne().registerCreatorFunc(&RenderBackEvent::make);
  Util::ObjFactory::theOne().registerCreatorFunc(&RenderFrontEvent::make);
  Util::ObjFactory::theOne().registerCreatorFunc(&ClearBufferEvent::make);

  Tcl::Pkg* pkg1 = new Tcl::Pkg(interp, "Th", "$Revision$");
  Tcl::defGenericObjCmds<TimingHdlr>(pkg1);

  pkg1->def( "addImmediateEvent", "th_id event_type msec_delay",
             Util::bindLast(&addEvent, TimingHdlr::IMMEDIATE) );
  pkg1->def( "addStartEvent", "th_id event_type msec_delay",
             Util::bindLast(&addEvent, TimingHdlr::FROM_START) );
  pkg1->def( "addResponseEvent", "th_id event_type msec_delay",
             Util::bindLast(&addEvent, TimingHdlr::FROM_RESPONSE) );
  pkg1->def( "addAbortEvent", "th_id event_type msec_delay",
             Util::bindLast(&addEvent, TimingHdlr::FROM_ABORT) );

  pkg1->eval("namespace eval Th { "
             "    proc autosavePeriod {id args} { "
             "        error {use Expt::autosavePeriod instead} } }");



  Tcl::Pkg* pkg2 = new Tcl::Pkg(interp, "SimpleTh", "$Revision$");
  Tcl::defGenericObjCmds<TimingHandler>(pkg2);

  pkg2->defAttrib("abortWait",
                  &TimingHandler::getAbortWait,
                  &TimingHandler::setAbortWait);
  pkg2->defAttrib("interTrialInterval",
                  &TimingHandler::getInterTrialInterval,
                  &TimingHandler::setInterTrialInterval);
  pkg2->defAttrib("stimDur", &TimingHandler::getStimDur, &TimingHandler::setStimDur);
  pkg2->defAttrib("timeout", &TimingHandler::getTimeout, &TimingHandler::setTimeout);

  return Tcl::Pkg::initStatus(pkg1, pkg2);
}

static const char vcid_thtcl_cc[] = "$Header$";
#endif // !THTCL_CC_DEFINED
