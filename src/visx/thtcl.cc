///////////////////////////////////////////////////////////////////////
//
// thtcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jun  9 20:39:46 1999
// written: Tue Feb  1 18:09:57 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef THTCL_CC_DEFINED
#define THTCL_CC_DEFINED

#include <tcl.h>
#include <string>

#include "iofactory.h"
#include "thlist.h"
#include "tclcmd.h"
#include "timinghandler.h"
#include "timinghdlr.h"
#include "trialevent.h"
#include "listitempkg.h"
#include "listpkg.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

namespace ThTcl {
  class AddEventCmd;
  class ThPkg;
}

namespace ThlistTcl {
  class ThListPkg;
}

//---------------------------------------------------------------------
//
// AddEventCmd --
//
//---------------------------------------------------------------------

class ThTcl::AddEventCmd : public Tcl::TclItemCmd<TimingHdlr> {
public:
  AddEventCmd(Tcl::TclItemPkg* pkg, const char* cmd_name,
				  TimingHdlr::TimePoint time_point) :
	 Tcl::TclItemCmd<TimingHdlr>(pkg, cmd_name, 
									"th_id event_type msec_delay", 4, 4),
	 itsTimePoint(time_point) {}
protected:
  virtual void invoke() {
	 TimingHdlr* th = getItem();
	 const char* event_type = getCstringFromArg(2);
	 int msec = getIntFromArg(3);

	 int eventid = th->addEventByName(event_type, itsTimePoint, msec);
	 returnInt(eventid);
  }
private:
  TimingHdlr::TimePoint itsTimePoint;
};

///////////////////////////////////////////////////////////////////////
//
// ThPkg class definition
//
///////////////////////////////////////////////////////////////////////

class ThTcl::ThPkg: public Tcl::ListItemPkg<TimingHdlr, ThList> {
public:
  ThPkg(Tcl_Interp* interp) :
	 Tcl::ListItemPkg<TimingHdlr, ThList>(interp, ThList::theThList(),
													  "Th", "1.1", "timingHdlr")
  {
	 addCommand( new AddEventCmd(this, "Th::addImmediateEvent",
										  TimingHdlr::IMMEDIATE));
	 addCommand( new AddEventCmd(this, "Th::addStartEvent",
										  TimingHdlr::FROM_START));
	 addCommand( new AddEventCmd(this, "Th::addResponseEvent",
										  TimingHdlr::FROM_RESPONSE));
	 addCommand( new AddEventCmd(this, "Th::addAbortEvent",
										  TimingHdlr::FROM_ABORT));
	 declareCAttrib("autosavePeriod",
						 &TimingHdlr::getAutosavePeriod,
						 &TimingHdlr::setAutosavePeriod);
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

class SimpleThTcl::SimpleThPkg : public Tcl::ListItemPkg<TimingHandler, ThList> {
public:
  SimpleThPkg(Tcl_Interp* interp) :
	 Tcl::ListItemPkg<TimingHandler, ThList>(interp, ThList::theThList(),
														  "SimpleTh", "1.1", "timingHandler")
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

///////////////////////////////////////////////////////////////////////
//
// ThListPkg class definition
//
///////////////////////////////////////////////////////////////////////

class ThlistTcl::ThListPkg : public Tcl::IoPtrListPkg {
public:
  ThListPkg(Tcl_Interp* interp) :
	 Tcl::IoPtrListPkg(interp, ThList::theThList(), "ThList", "3.0")
  {
	 ThList::theThList().insertAt(0, ThList::Ptr(new TimingHandler()));
  }
};

//---------------------------------------------------------------------
//
// ThTcl::Th_Init
//
//---------------------------------------------------------------------

extern "C" Tcl_PackageInitProc Th_Init;

int Th_Init(Tcl_Interp* interp) {
DOTRACE("Th_Init");

  new ThTcl::ThPkg(interp);
  new SimpleThTcl::SimpleThPkg(interp);
  new ThlistTcl::ThListPkg(interp);

  FactoryRegistrar<IO, TimingHdlr>       :: registerWith(IoFactory::theOne());
  FactoryRegistrar<IO, TimingHandler>    :: registerWith(IoFactory::theOne());

  FactoryRegistrar<IO, AbortTrialEvent>  :: registerWith(IoFactory::theOne());
  FactoryRegistrar<IO, DrawEvent>        :: registerWith(IoFactory::theOne());
  FactoryRegistrar<IO, UndrawEvent>      :: registerWith(IoFactory::theOne());
  FactoryRegistrar<IO, EndTrialEvent>    :: registerWith(IoFactory::theOne());
  FactoryRegistrar<IO, SwapBuffersEvent> :: registerWith(IoFactory::theOne());
  FactoryRegistrar<IO, RenderBackEvent>  :: registerWith(IoFactory::theOne());
  FactoryRegistrar<IO, RenderFrontEvent> :: registerWith(IoFactory::theOne());
  FactoryRegistrar<IO, ClearBufferEvent> :: registerWith(IoFactory::theOne());

  return TCL_OK;
}

static const char vcid_thtcl_cc[] = "$Header$";
#endif // !THTCL_CC_DEFINED
