///////////////////////////////////////////////////////////////////////
//
// rhtcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jun  9 20:39:46 1999
// written: Mon Jun 14 13:50:18 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef THTCL_CC_DEFINED
#define THTCL_CC_DEFINED

#include <tcl.h>
#include <strstream.h>

#include "rhlist.h"
#include "responsehandler.h"
#include "tclcmd.h"
#include "attribcommand.h"
#include "tclpkg.h"
#include "tcllistpkg.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

//---------------------------------------------------------------------
//
// RhAttribCommand class
//
// This is a base class for other command classes that get/set
// attributes of ResponseHandler's.
//
//---------------------------------------------------------------------

class RhAttribCommand : public AttribCommand {
public:
  RhAttribCommand(Tcl_Interp* interp, const char* cmd_name) :
	 AttribCommand(interp, cmd_name, "rhid ?new_value?", 2, 3), itsRh(NULL) {}
protected:
  virtual void get() = 0;
  virtual void set() = 0;

  ResponseHandler* itsRh;
private:
  
  virtual void invoke() {
	 int id = getIntFromArg(1);
	 
	 static const char* const bad_rhid_msg = "invalid response handler id";
	 if ( !RhList::theRhList().isValidId(id) ) {
		throw TclError(bad_rhid_msg);
	 }

	 itsRh = RhList::theRhList().getPtr(id);
	 
	 AttribCommand::invoke();
  }
};

//--------------------------------------------------------------------
//
// ResponseHandlerCmd --
//
//
//--------------------------------------------------------------------

class ResponseHandlerCmd : public TclCmd {
public:
  ResponseHandlerCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name, NULL, 1, 1) {}
protected:
  virtual void invoke() {
	 ResponseHandler* rh = new ResponseHandler();
	 int rhid = RhList::theRhList().insert(rh);
	 returnInt(rhid);
  }
};

//--------------------------------------------------------------------
//
// KeyRespPairsCmd --
//
//
//--------------------------------------------------------------------

class KeyRespPairsCmd : public RhAttribCommand {
public:
  KeyRespPairsCmd(Tcl_Interp* interp, const char* cmd_name) :
	 RhAttribCommand(interp, cmd_name) {}
protected:
  virtual void get() { returnString(itsRh->getKeyRespPairs().c_str()); }
  virtual void set() { itsRh->setKeyRespPairs(getStringFromArg(2)); }
};

//--------------------------------------------------------------------
//
// UseFeedbackCmd --
//
// Retrieve the current value or set a new value for whether feedback
// is given in a specified ResponseHandler. If feedback is turned on,
// then a user-defined function named "feedback" must be defined to
// take a single argument (the trial response). This function will be
// called upon each successful trial completion, and the function
// should provide feedback as to whether the response was correct or
// incorrect. (However, note that the response does not need to deal
// with *invalid* responses; these are filtered out before "feedback"
// is called.)
//
// Results: 
// If the current value is being retrieved, returns a boolean
// indicating whether feedback is used.
//
// Side effects:
// If a new value is provided, feedback may be turned on or off.
//
//--------------------------------------------------------------------

class UseFeedbackCmd : public RhAttribCommand {
public:
  UseFeedbackCmd(Tcl_Interp* interp, const char* cmd_name) :
	 RhAttribCommand(interp, cmd_name) {}
protected:
  virtual void get() { returnBool(itsRh->getUseFeedback()); }
  virtual void set() { itsRh->setUseFeedback(getBoolFromArg(2)); }
};

///////////////////////////////////////////////////////////////////////
//
// RhListPkg class definition
//
///////////////////////////////////////////////////////////////////////

class RhListPkg : public TclListPkg {
public:
  RhListPkg(Tcl_Interp* interp) :
	 TclListPkg(interp, "RhList", "1.1")
  {
	 RhList::theRhList().setInterp(interp);
	 RhList::theRhList().insertAt(0, new ResponseHandler());
  }

  virtual IO& getList() { return RhList::theRhList(); }
  virtual int getBufSize() { return 200; }

  virtual void reset() { RhList::theRhList().clear(); }
  virtual int count() { return RhList::theRhList().count(); }
};

///////////////////////////////////////////////////////////////////////
//
// RhPkg class definition
//
///////////////////////////////////////////////////////////////////////

class RhPkg : public TclPkg {
public:
  RhPkg(Tcl_Interp* interp) :
	 TclPkg(interp, "Rh", "1.1") 
  {
	 addCommand( new ResponseHandlerCmd(interp, "Rh::responseHandler") );
	 addCommand( new UseFeedbackCmd(interp, "Rh::useFeedback") );
	 addCommand( new KeyRespPairsCmd(interp, "Rh::keyRespPairs") );	 
  }
};

//--------------------------------------------------------------------
//
// RhTcl::Rh_Init --
//
//--------------------------------------------------------------------

namespace RhTcl {
  Tcl_PackageInitProc Rh_Init;
}

int RhTcl::Rh_Init(Tcl_Interp* interp) {
DOTRACE("RhTcl::Rh_Init");

  new RhListPkg(interp);
  new RhPkg(interp);

  return TCL_OK;
}

static const char vcid_rhtcl_cc[] = "$Header$";
#endif // !THTCL_CC_DEFINED
