///////////////////////////////////////////////////////////////////////
//
// trialtcl.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jun 21 09:51:54 1999
// written: Fri Nov 10 17:03:58 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRIALTCL_CC_DEFINED
#define TRIALTCL_CC_DEFINED

#include "trial.h"

#include "io/iofactory.h"

#include "tcl/listitempkg.h"
#include "tcl/tracertcl.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

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
  AddCmd(Tcl::CTclItemPkg<Trial>* pkg, const char* cmd_name) :
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

class TrialTcl::TrialPkg : public Tcl::ItemPkg<Trial> {
public:
  TrialPkg(Tcl_Interp* interp) :
	 Tcl::ItemPkg<Trial>(interp, "Trial", "$Revision$")
  {
	 Tcl::addTracing(this, Trial::tracer);

	 addCommand( new AddCmd(this, "Trial::add") );

	 declareCGetter("avgResponse", &Trial::avgResponse);
	 declareCGetter("avgRespTime", &Trial::avgRespTime);
	 declareCAction("clearObjs", &Trial::clearObjs);
	 declareCAction("clearResponses", &Trial::clearResponses);
	 declareCAttrib("correctResponse",
						 &Trial::getCorrectResponse, &Trial::setCorrectResponse);
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

extern "C"
int Trial_Init(Tcl_Interp* interp) {
DOTRACE("Trial_Init");

  Tcl::TclPkg* pkg = new TrialTcl::TrialPkg(interp);

  IO::IoFactory::theOne().registerCreatorFunc(&Trial::make);

  return pkg->initStatus();
}

static const char vcid_trialtcl_cc[] = "$Header$";
#endif // !TRIALTCL_CC_DEFINED
