///////////////////////////////////////////////////////////////////////
//
// thtcl.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Jun  9 20:39:46 1999
// written: Wed Mar 19 12:46:28 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef THTCL_CC_DEFINED
#define THTCL_CC_DEFINED

#include "visx/timinghandler.h"
#include "visx/timinghdlr.h"
#include "visx/trialevent.h"

#include "tcl/objpkg.h"
#include "tcl/tclpkg.h"

#include "util/objfactory.h"

#include <typeinfo>

#include "util/trace.h"

namespace
{
  unsigned int addNewEvent(Util::Ref<TimingHdlr> th, const char* event_type,
                           int msec, TimingHdlr::TimePoint time_point)
  {
    return th->addEventByName(event_type, time_point, msec);
  }

  template <class EventType>
  Tcl::Pkg* initEventType(Tcl_Interp* interp)
  {
    Util::ObjFactory::theOne().registerCreatorFunc(&EventType::make);

    Tcl::Pkg* pkg = new Tcl::Pkg(interp,
                                 demangle_cstr(typeid(EventType).name()),
                                 "$Revision$");
    pkg->inherit("TrialEvent");

    return pkg;
  }
}

extern "C"
int Th_Init(Tcl_Interp* interp)
{
DOTRACE("Th_Init");

  Util::ObjFactory::theOne().registerCreatorFunc(&TimingHdlr::make);
  Util::ObjFactory::theOne().registerCreatorFunc(&TimingHandler::make);

  Tcl::Pkg* pkg1 = new Tcl::Pkg(interp, "TimingHdlr", "$Revision$");
  pkg1->inherit("IO");
  Tcl::defGenericObjCmds<TimingHdlr>(pkg1);

  pkg1->def( "addImmediateEvent", "th_id event_type msec_delay",
             Util::bindLast(&addNewEvent, TimingHdlr::IMMEDIATE) );
  pkg1->def( "addStartEvent", "th_id event_type msec_delay",
             Util::bindLast(&addNewEvent, TimingHdlr::FROM_START) );
  pkg1->def( "addResponseEvent", "th_id event_type msec_delay",
             Util::bindLast(&addNewEvent, TimingHdlr::FROM_RESPONSE) );
  pkg1->def( "addAbortEvent", "th_id event_type msec_delay",
             Util::bindLast(&addNewEvent, TimingHdlr::FROM_ABORT) );

  pkg1->def( "addImmediateEvent", "th_id event_id",
             Util::bindLast(Util::memFunc(&TimingHdlr::addEvent),
                            TimingHdlr::IMMEDIATE) );
  pkg1->def( "addStartEvent", "th_id event_id",
             Util::bindLast(Util::memFunc(&TimingHdlr::addEvent),
                            TimingHdlr::FROM_START) );
  pkg1->def( "addResponseEvent", "th_id event_id",
             Util::bindLast(Util::memFunc(&TimingHdlr::addEvent),
                            TimingHdlr::FROM_RESPONSE) );
  pkg1->def( "addAbortEvent", "th_id event_id",
             Util::bindLast(Util::memFunc(&TimingHdlr::addEvent),
                            TimingHdlr::FROM_ABORT) );

  pkg1->namespaceAlias("Th");

  Tcl::Pkg* pkg2 = new Tcl::Pkg(interp, "TimingHandler", "$Revision$");
  pkg2->inherit("IO");
  Tcl::defGenericObjCmds<TimingHandler>(pkg2);

  pkg2->defAttrib("abortWait",
                  &TimingHandler::getAbortWait,
                  &TimingHandler::setAbortWait);
  pkg2->defAttrib("interTrialInterval",
                  &TimingHandler::getInterTrialInterval,
                  &TimingHandler::setInterTrialInterval);
  pkg2->defAttrib("stimDur",
                  &TimingHandler::getStimDur,
                  &TimingHandler::setStimDur);
  pkg2->defAttrib("timeout",
                  &TimingHandler::getTimeout,
                  &TimingHandler::setTimeout);

  pkg2->namespaceAlias("SimpleTh");

  Tcl::Pkg* pkg3 = new Tcl::Pkg(interp, "TrialEvent", "$Revision$");
  Tcl::defGenericObjCmds<TrialEvent>(pkg3);

  pkg3->defAttrib("delay", &TrialEvent::getDelay, &TrialEvent::setDelay);

  initEventType<AbortTrialEvent>(interp);
  initEventType<DrawEvent>(interp);
  initEventType<RenderEvent>(interp);
  initEventType<UndrawEvent>(interp);
  initEventType<EndTrialEvent>(interp);
  initEventType<NextNodeEvent>(interp);
  initEventType<AllowResponsesEvent>(interp);
  initEventType<DenyResponsesEvent>(interp);
  initEventType<SwapBuffersEvent>(interp);
  initEventType<RenderBackEvent>(interp);
  initEventType<RenderFrontEvent>(interp);
  initEventType<ClearBufferEvent>(interp);

  Tcl::Pkg* pkg4 = initEventType<GenericEvent>(interp);

  pkg4->defGetter("callback", &GenericEvent::getCallback);
  pkg4->defSetter("callback", &GenericEvent::setCallback);

  return Tcl::Pkg::initStatus(pkg1, pkg2, pkg3, pkg4);
}

static const char vcid_thtcl_cc[] = "$Header$";
#endif // !THTCL_CC_DEFINED
