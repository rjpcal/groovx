///////////////////////////////////////////////////////////////////////
//
// trialtcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun 21 09:51:54 1999
// written: Thu Jun 24 19:21:09 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRIALTCL_CC_DEFINED
#define TRIALTCL_CC_DEFINED

#include <tcl.h>

#include "trial.h"
#include "tlist.h"
#include "tclitempkg.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

namespace TrialTcl {
  class AddCmd;
  class TrialPkg;
}

///////////////////////////////////////////////////////////////////////
//
// TrialPkg class definition
//
///////////////////////////////////////////////////////////////////////

class TrialTcl::TrialPkg : public CTclIoItemPkg<Trial> {
public:
  TrialPkg(Tcl_Interp* interp) :
	 CTclIoItemPkg<Trial>(interp, "Trial", "1.1")
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

  virtual Trial* getCItemFromId(int id) {
	 // Generate an error if the id is out of range
	 if ( !Tlist::theTlist().isValidTrial(id) ) {
		throw TclError("invalid trial id");
	 }
	 return &(Tlist::theTlist().getTrial(id));
  }

  virtual IO& getIoFromId(int id) {
	 return dynamic_cast<IO&>( *(getCItemFromId(id)) );
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

  return TCL_OK;
}

static const char vcid_trialtcl_cc[] = "$Header$";
#endif // !TRIALTCL_CC_DEFINED
