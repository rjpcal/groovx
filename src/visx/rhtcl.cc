///////////////////////////////////////////////////////////////////////
//
// rhtcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jun  9 20:39:46 1999
// written: Tue Jun 15 14:54:11 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef RHTCL_CC_DEFINED
#define RHTCL_CC_DEFINED

#include <tcl.h>
#include <strstream.h>

#include "rhlist.h"
#include "responsehandler.h"
#include "attribcmd.h"
#include "tclitempkg.h"
#include "tcllistpkg.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

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

///////////////////////////////////////////////////////////////////////
//
// RhListPkg class definition
//
///////////////////////////////////////////////////////////////////////

class RhListPkg : public TclListPkg {
public:
  RhListPkg(Tcl_Interp* interp) :
	 TclListPkg(interp, "RhList", "1.4")
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

class RhPkg : public TclItemPkg {
public:
  RhPkg(Tcl_Interp* interp) :
	 TclItemPkg(interp, "Rh", "1.4")
  {
	 addCommand( new ResponseHandlerCmd(interp, "Rh::responseHandler") );
	 declareBoolAttrib("useFeedback",
							 new TBoolAttrib<ResponseHandler> (
										&ResponseHandler::getUseFeedback,
										&ResponseHandler::setUseFeedback));
	 declareStringAttrib("keyRespPairs",
								new TStringAttrib<ResponseHandler> (
									   &ResponseHandler::getKeyRespPairs,
										&ResponseHandler::setKeyRespPairs));
  }

  virtual void* getItemFromId(int id) {
	 if ( !RhList::theRhList().isValidId(id) ) {
		throw TclError("invalid response handler id");
	 }
	 return static_cast<void*>(RhList::theRhList().getPtr(id));
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
#endif // !RHTCL_CC_DEFINED
