///////////////////////////////////////////////////////////////////////
//
// thtcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jun  9 20:39:46 1999
// written: Mon Oct 23 23:14:34 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef THTCL_CC_DEFINED
#define THTCL_CC_DEFINED

#include "thlist.h"
#include "timinghandler.h"
#include "timinghdlr.h"
#include "trialevent.h"

#include "io/iofactory.h"

#include "tcl/listitempkg.h"
#include "tcl/listpkg.h"
#include "tcl/tclcmd.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

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
													  "Th", "$Revision$")
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
							 "    proc timingHdlr {} { return [Th] } }");
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

class SimpleThTcl::SimpleThPkg : public Tcl::ListItemPkg<TimingHandler, ThList> {
public:
  SimpleThPkg(Tcl_Interp* interp) :
	 Tcl::ListItemPkg<TimingHandler, ThList>(interp, ThList::theThList(),
														  "SimpleTh", "$Revision$")
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

	 Tcl::TclPkg::eval("namespace eval SimpleTh { "
							 "    proc timingHandler {} { return [SimpleTh] } }");
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
	 Tcl::IoPtrListPkg(interp, ThList::theThList(), "ThList", "$Revision$")
  {
	 // FIXME
// 	 ThList::theThList().insertAt(0, ThList::Ptr(TimingHandler::make()));
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

  IO::IoFactory::theOne().registerCreatorFunc(&TimingHdlr::make);
  IO::IoFactory::theOne().registerCreatorFunc(&TimingHandler::make);

  IO::IoFactory::theOne().registerCreatorFunc(&AbortTrialEvent::make);
  IO::IoFactory::theOne().registerCreatorFunc(&DrawEvent::make);
  IO::IoFactory::theOne().registerCreatorFunc(&UndrawEvent::make);
  IO::IoFactory::theOne().registerCreatorFunc(&EndTrialEvent::make);
  IO::IoFactory::theOne().registerCreatorFunc(&SwapBuffersEvent::make);
  IO::IoFactory::theOne().registerCreatorFunc(&RenderBackEvent::make);
  IO::IoFactory::theOne().registerCreatorFunc(&RenderFrontEvent::make);
  IO::IoFactory::theOne().registerCreatorFunc(&ClearBufferEvent::make);

  Tcl::TclPkg* pkg1 = new ThTcl::ThPkg(interp);
  Tcl::TclPkg* pkg2 = new SimpleThTcl::SimpleThPkg(interp);
  Tcl::TclPkg* pkg3 = new ThlistTcl::ThListPkg(interp);

  return pkg1->combineStatus(pkg2->combineStatus(pkg3->initStatus()));
}

static const char vcid_thtcl_cc[] = "$Header$";
#endif // !THTCL_CC_DEFINED
