///////////////////////////////////////////////////////////////////////
//
// thtcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jun  9 20:39:46 1999
// written: Wed Jul 11 19:53:12 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef THTCL_CC_DEFINED
#define THTCL_CC_DEFINED

#include "timinghandler.h"
#include "timinghdlr.h"
#include "trialevent.h"

#include "tcl/genericobjpkg.h"
#include "tcl/tclcmd.h"

#include "util/objfactory.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

namespace ThTcl {
  class AddEventCmd;
  class ThPkg;
}

//---------------------------------------------------------------------
//
// AddEventCmd --
//
//---------------------------------------------------------------------

class ThTcl::AddEventCmd : public Tcl::TclItemCmd<TimingHdlr> {
public:
  AddEventCmd(Tcl::CTclItemPkg<TimingHdlr>* pkg, const char* cmd_name,
              TimingHdlr::TimePoint time_point) :
    Tcl::TclItemCmd<TimingHdlr>(pkg, cmd_name,
                           "th_id event_type msec_delay", 4, 4),
    itsTimePoint(time_point) {}
protected:
  virtual void invoke(Context& ctx) {
    TimingHdlr* th = getItem(ctx);
    const char* event_type = ctx.getCstringFromArg(2);
    int msec = ctx.getIntFromArg(3);

    int eventid = th->addEventByName(event_type, itsTimePoint, msec);
    ctx.setResult(eventid);
  }
private:
  TimingHdlr::TimePoint itsTimePoint;
};

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
    addCommand( new AddEventCmd(this, "Th::addImmediateEvent",
                                TimingHdlr::IMMEDIATE));
    addCommand( new AddEventCmd(this, "Th::addStartEvent",
                                TimingHdlr::FROM_START));
    addCommand( new AddEventCmd(this, "Th::addResponseEvent",
                                TimingHdlr::FROM_RESPONSE));
    addCommand( new AddEventCmd(this, "Th::addAbortEvent",
                                TimingHdlr::FROM_ABORT));

    Tcl::TclPkg::eval("namespace eval Th { "
                      "    proc autosavePeriod {id args} { "
                      "        return [eval Expt::autosavePeriod $args] } }");
  }
};

///////////////////////////////////////////////////////////////////////
//
// SimpleThPkg class definition
//
///////////////////////////////////////////////////////////////////////

namespace SimpleThTcl {
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
int Th_Init(Tcl_Interp* interp) {
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
