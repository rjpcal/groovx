///////////////////////////////////////////////////////////////////////
//
// trialtcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun 21 09:51:54 1999
// written: Tue Dec  7 19:05:50 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRIALTCL_CC_DEFINED
#define TRIALTCL_CC_DEFINED

#include <tcl.h>

#include "iomgr.h"
#include "trial.h"
#include "tlist.h"
#include "listitempkg.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

namespace TrialTcl {
  class AddCmd;
  class TrialPkg;
}

//---------------------------------------------------------------------
//
// TrialTcl::AddCmd --
//
//---------------------------------------------------------------------

class TrialTcl::AddCmd : public Tcl::TclItemCmd<Trial> {
public:
  AddCmd(Tcl::TclItemPkg* pkg, const char* cmd_name) :
	 Tcl::TclItemCmd<Trial>(pkg, cmd_name, "trialid objid posid", 4, 4) {}
protected:
  virtual void invoke() {
	 Trial* t = getItem();

	 int objid = getIntFromArg(2);
	 if (objid < 0) { throw Tcl::TclError("invalid id"); }

	 int posid = getIntFromArg(3);
	 if (posid < 0) { throw Tcl::TclError("invalid id"); }

	 t->add(objid, posid);
  }
};

///////////////////////////////////////////////////////////////////////
//
// TrialPkg class definition
//
///////////////////////////////////////////////////////////////////////

class TrialTcl::TrialPkg : public Tcl::ListItemPkg<Trial, Tlist> {
public:
  TrialPkg(Tcl_Interp* interp) :
	 Tcl::ListItemPkg<Trial, Tlist>(interp, Tlist::theTlist(), "Trial", "1.1")
  {
	 addCommand( new AddCmd(this, "Trial::add") );

	 declareCGetter("avgResponse", &Trial::avgResponse);
	 declareCGetter("avgRespTime", &Trial::avgRespTime);
	 declareCAction("clearObjs", &Trial::clearObjs);
	 declareCAction("clearResponses", &Trial::clearResponses);
	 declareCGetter("description", &Trial::description);
	 declareCGetter("lastResponse", &Trial::lastResponse);
	 declareCGetter("numResponses", &Trial::numResponses);
	 declareCAttrib("responseHdlr",
						 &Trial::getResponseHandler, &Trial::setResponseHandler);
	 declareCAttrib("timingHdlr",
						&Trial::getTimingHdlr, &Trial::setTimingHdlr);
	 declareCAttrib("type", &Trial::trialType, &Trial::setType);
	 declareCAction("undoLastResponse", &Trial::undoLastResponse);
  }
};

//---------------------------------------------------------------------
//
// Trial_Init --
//
//---------------------------------------------------------------------

extern "C" Tcl_PackageInitProc Trial_Init;

int Trial_Init(Tcl_Interp* interp) {
DOTRACE("Trial_Init");

  new TrialTcl::TrialPkg(interp);

  FactoryRegistrar<IO, Trial>::registerWith(IoFactory::theOne());

  return TCL_OK;
}

static const char vcid_trialtcl_cc[] = "$Header$";
#endif // !TRIALTCL_CC_DEFINED
