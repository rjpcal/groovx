///////////////////////////////////////////////////////////////////////
//
// thtcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jun  9 20:39:46 1999
// written: Thu Jun 24 19:21:10 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef THTCL_CC_DEFINED
#define THTCL_CC_DEFINED

#include <tcl.h>
#include <string>

#include "thlist.h"
#include "tclcmd.h"
#include "timinghandler.h"
#include "timinghdlr.h"
#include "tclitempkg.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

namespace ThTcl {
  class TimingHdlrCmd;
  class AddEventCmd;
  class ThPkg;
}

namespace SimpleThTcl {
  class TimingHandlerCmd;
  class SimpleThPkg;
}

namespace ThlistTcl {
  class ThListPkg;
}

//---------------------------------------------------------------------
//
// TimingHdlrCmd --
//
//---------------------------------------------------------------------

class ThTcl::TimingHdlrCmd : public TclCmd {
public:
  TimingHdlrCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name, NULL, 1, 1) {}
protected:
  virtual void invoke() {
	 TimingHdlr* th = new TimingHdlr();
	 int thid = ThList::theThList().insert(th);
	 returnInt(thid);
  }
};

//---------------------------------------------------------------------
//
// AddEventCmd --
//
//---------------------------------------------------------------------

class ThTcl::AddEventCmd : public TclItemCmd<TimingHdlr> {
public:
  AddEventCmd(TclItemPkg* pkg, const char* cmd_name,
				  TimingHdlr::TimePoint time_point) :
	 TclItemCmd<TimingHdlr>(pkg, cmd_name, 
									"th_id event_type msec_delay", 4, 4),
	 itsTimePoint(time_point) {}
protected:
  virtual void invoke() {
	 TimingHdlr* th = getItem();
	 const char* event_type = getCstringFromArg(2);
	 int msec = getIntFromArg(3);

	 int eventid = th->addEventByName(event_type, itsTimePoint, msec);
  }
private:
  TimingHdlr::TimePoint itsTimePoint;
};

///////////////////////////////////////////////////////////////////////
//
// ThPkg class definition
//
///////////////////////////////////////////////////////////////////////

class ThTcl::ThPkg: public CTclIoItemPkg<TimingHdlr> {
public:
  ThPkg(Tcl_Interp* interp) :
	 CTclIoItemPkg<TimingHdlr>(interp, "Th", "1.1")
  {
	 addCommand( new TimingHdlrCmd(interp, "Th::timingHdlr") );
	 addCommand( new AddEventCmd(this, "Th::addImmediateEvent",
										  TimingHdlr::IMMEDIATE));
	 addCommand( new AddEventCmd(this, "Th::addStartEvent",
										  TimingHdlr::FROM_START));
	 addCommand( new AddEventCmd(this, "Th::addResponseEvent",
										  TimingHdlr::FROM_RESPONSE));
	 addCommand( new AddEventCmd(this, "Th::addAbortEvent",
										  TimingHdlr::FROM_ABORT));
	 declareAttrib("autosavePeriod",
						new CAttrib<TimingHandler, int> (
									 &TimingHdlr::getAutosavePeriod,
									 &TimingHdlr::setAutosavePeriod));
  }

  virtual TimingHdlr* getCItemFromId(int id) {
	 if ( !ThList::theThList().isValidId(id) ) {
		throw TclError("invalid timing handler id");
	 }
	 return ThList::theThList().getPtr(id);
  }

  virtual IO& getIoFromId(int id) {
	 return dynamic_cast<IO&>( *(getCItemFromId(id)) );
  }
};

//--------------------------------------------------------------------
//
// TimingHandlerCmd --
//
//--------------------------------------------------------------------

class SimpleThTcl::TimingHandlerCmd : public TclCmd {
public:
  TimingHandlerCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name, NULL, 1, 1) {}
protected:
  virtual void invoke() {
	 TimingHandler* th = new TimingHandler();
	 int thid = ThList::theThList().insert(th);
	 returnInt(thid);
  }
};

///////////////////////////////////////////////////////////////////////
//
// SimpleThPkg class definition
//
///////////////////////////////////////////////////////////////////////

class SimpleThTcl::SimpleThPkg : public CTclIoItemPkg<TimingHandler> {
public:
  SimpleThPkg(Tcl_Interp* interp) :
	 CTclIoItemPkg<TimingHandler>(interp, "Simpleth", "1.1")
  {
	 addCommand( new TimingHandlerCmd(interp, "SimpleTh::timingHandler") );
	 declareAttrib("abortWait", 
						new CAttrib<TimingHandler, int> (
									 &TimingHandler::getAbortWait,
									 &TimingHandler::setAbortWait));
	 declareAttrib("interTrialInterval",
						new CAttrib<TimingHandler, int> (
									 &TimingHandler::getInterTrialInterval,
									 &TimingHandler::setInterTrialInterval));
	 declareAttrib("stimDur",
						new CAttrib<TimingHandler, int> (
									 &TimingHandler::getStimDur,
									 &TimingHandler::setStimDur));
	 declareAttrib("timeout",
						new CAttrib<TimingHandler, int> (
									 &TimingHandler::getTimeout,
									 &TimingHandler::setTimeout));
  }

  virtual TimingHandler* getCItemFromId(int id) {
	 if ( !ThList::theThList().isValidId(id) ) {
		throw TclError("invalid timing handler id");
	 }
	 TimingHandler* th = 
		dynamic_cast<TimingHandler*>(ThList::theThList().getPtr(id));
	 if (th == 0) {
		throw TclError("timing handler not of correct type");
	 }
	 return th;
  }

  virtual IO& getIoFromId(int id) {
	 return dynamic_cast<IO&>( *(getCItemFromId(id)) );
  }
};

///////////////////////////////////////////////////////////////////////
//
// ThListPkg class definition
//
///////////////////////////////////////////////////////////////////////

class ThlistTcl::ThListPkg : public CTclIoItemPkg<ThList> {
public:
  ThListPkg(Tcl_Interp* interp) :
	 CTclIoItemPkg<ThList>(interp, "ThList", "1.1", 0)
  {
	 ThList::theThList().insertAt(0, new TimingHandler());

	 declareGetter("count", new CGetter<ThList, int>(&ThList::count));
	 declareAction("reset", new CAction<ThList>(&ThList::clear));
  }

  virtual IO& getIoFromId(int) { return ThList::theThList(); }

  virtual ThList* getCItemFromId(int) {
	 return &ThList::theThList();
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

  return TCL_OK;
}

static const char vcid_thtcl_cc[] = "$Header$";
#endif // !THTCL_CC_DEFINED
