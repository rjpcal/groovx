///////////////////////////////////////////////////////////////////////
//
// thtcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jun  9 20:39:46 1999
// written: Sat Jul 14 11:27:23 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef THTCL_CC_DEFINED
#define THTCL_CC_DEFINED

#include "timinghandler.h"
#include "timinghdlr.h"
#include "trialevent.h"

#include "tcl/genericobjpkg.h"
#include "tcl/objfunctor.h"
#include "tcl/tclcmd.h"

#include "util/objfactory.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace ThTcl
{
  int addImmediateEvent(Util::Ref<TimingHdlr> th, const char* event_type,
                        int msec)
  {
    return th->addEventByName(event_type, TimingHdlr::IMMEDIATE, msec);
  }
  int addStartEvent(Util::Ref<TimingHdlr> th, const char* event_type,
                    int msec)
  {
    return th->addEventByName(event_type, TimingHdlr::FROM_START, msec);
  }
  int addResponseEvent(Util::Ref<TimingHdlr> th, const char* event_type,
                       int msec)
  {
    return th->addEventByName(event_type, TimingHdlr::FROM_RESPONSE, msec);
  }
  int addAbortEvent(Util::Ref<TimingHdlr> th, const char* event_type,
                    int msec)
  {
    return th->addEventByName(event_type, TimingHdlr::FROM_ABORT, msec);
  }

  class ThPkg;
}

///////////////////////////////////////////////////////////////////////
//
// ThPkg class definition
//
///////////////////////////////////////////////////////////////////////

class ThTcl::ThPkg: public Tcl::GenericObjPkg<TimingHdlr> {
public:
  ThPkg(Tcl_Interp* interp) :
    Tcl::GenericObjPkg<TimingHdlr>(interp, "Th", "$Revision$")
  {
    Tcl::def( this, &ThTcl::addImmediateEvent,
              "Th::addImmediateEvent", "th_id event_type msec_delay" );
    Tcl::def( this, &ThTcl::addStartEvent,
              "Th::addStartEvent", "th_id event_type msec_delay" );
    Tcl::def( this, &ThTcl::addResponseEvent,
              "Th::addResponseEvent", "th_id event_type msec_delay" );
    Tcl::def( this, &ThTcl::addAbortEvent,
              "Th::addAbortEvent", "th_id event_type msec_delay" );

    Tcl::TclPkg::eval("namespace eval Th { "
                      "    proc autosavePeriod {id args} { "
                      "        error {use Expt::autosavePeriod instead} } }");
  }
};

///////////////////////////////////////////////////////////////////////
//
// SimpleThPkg class definition
//
///////////////////////////////////////////////////////////////////////

namespace SimpleThTcl
{
  class SimpleThPkg;
}

class SimpleThTcl::SimpleThPkg :
  public Tcl::GenericObjPkg<TimingHandler> {
public:
  SimpleThPkg(Tcl_Interp* interp) :
    Tcl::GenericObjPkg<TimingHandler>(interp, "SimpleTh", "$Revision$")
  {
    declareCAttrib("abortWait",
                   &TimingHandler::getAbortWait,
                   &TimingHandler::setAbortWait);
    declareCAttrib("interTrialInterval",
                   &TimingHandler::getInterTrialInterval,
                   &TimingHandler::setInterTrialInterval);
    declareCAttrib("stimDur",
                   &TimingHandler::getStimDur,
                   &TimingHandler::setStimDur);
    declareCAttrib("timeout",
                   &TimingHandler::getTimeout,
                   &TimingHandler::setTimeout);
  }
};

//---------------------------------------------------------------------
//
// ThTcl::Th_Init
//
//---------------------------------------------------------------------

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

  Tcl::TclPkg* pkg1 = new ThTcl::ThPkg(interp);
  Tcl::TclPkg* pkg2 = new SimpleThTcl::SimpleThPkg(interp);

  return pkg1->combineStatus(pkg2->initStatus());
}

static const char vcid_thtcl_cc[] = "$Header$";
#endif // !THTCL_CC_DEFINED
