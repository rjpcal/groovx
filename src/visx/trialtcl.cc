///////////////////////////////////////////////////////////////////////
//
// trialtcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun 21 09:51:54 1999
// written: Thu Jul  8 10:56:44 1999
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

class TrialTcl::AddCmd : public TclItemCmd<Trial> {
public:
  AddCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<Trial>(pkg, cmd_name, "trialid objid posid", 4, 4) {}
protected:
  virtual void invoke() {
	 Trial* t = getItem();

	 int objid = getIntFromArg(2);
	 if (objid < 0) { throw TclError("invalid id"); }

	 int posid = getIntFromArg(3);
	 if (posid < 0) { throw TclError("invalid id"); }

	 t->add(objid, posid);
  }
};

///////////////////////////////////////////////////////////////////////
//
// TrialPkg class definition
//
///////////////////////////////////////////////////////////////////////

class TrialTcl::TrialPkg : public ListItemPkg<Trial, Tlist> {
public:
  TrialPkg(Tcl_Interp* interp) :
	 ListItemPkg<Trial, Tlist>(interp, Tlist::theTlist(), "Trial", "1.1")
  {
	 declareAttrib("responseHdlr",
						new CAttrib<Trial, int>(&Trial::getResponseHandler,
														&Trial::setResponseHandler));
	 declareAttrib("timingHdlr",
						new CAttrib<Trial, int>(&Trial::getTimingHdlr,
														&Trial::setTimingHdlr));
	 declareAttrib("type", new CAttrib<Trial, int>(&Trial::trialType,
																  &Trial::setType));
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

  FactoryRegistrar<IO, Trial> registrar(IoFactory::theOne());

  return TCL_OK;
}

static const char vcid_trialtcl_cc[] = "$Header$";
#endif // !TRIALTCL_CC_DEFINED
