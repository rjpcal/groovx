///////////////////////////////////////////////////////////////////////
//
// poslisttcl.cc
// Rob Peters
// created: Sat Mar 13 12:46:09 1999
// written: Mon Jun 14 14:26:03 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef POSLISTTCL_CC_DEFINED
#define POSLISTTCL_CC_DEFINED

#include "poslisttcl.h"

#include <typeinfo>
#include <tcl.h>
#include <strstream.h>

#include "errmsg.h"
#include "position.h"
#include "poslist.h"
#include "tcllistpkg.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_TRACE
#include "debug.h"

///////////////////////////////////////////////////////////////////////
//
// Poslist Tcl package declarations
//
///////////////////////////////////////////////////////////////////////

namespace PoslistTcl {
  Tcl_ObjCmdProc posTypeCmd;
}

///////////////////////////////////////////////////////////////////////
//
// Poslist Tcl package definitions
//
///////////////////////////////////////////////////////////////////////

Position* PoslistTcl::getPosFromArg(Tcl_Interp* interp, Tcl_Obj* const objv[], 
                                    const PosList&, int argn) {
DOTRACE("PoslistTcl::getPosFromArg");
// XXX get rid of PosList argument

  // Make sure we have a valid posid
  int id;
  if ( Tcl_GetIntFromObj(interp, objv[argn], &id) != TCL_OK ) return NULL;

  PosId posid(id);

  if ( !posid ) {
    err_message(interp, objv, bad_posid_msg);
    return NULL;
  }

  // Get the position
  return posid.get();
}

// return an object's name
int PoslistTcl::posTypeCmd(ClientData, Tcl_Interp* interp,
                           int objc, Tcl_Obj* const objv[]) {
DOTRACE("PoslistTcl::posTypeCmd");
  if (objc != 2)  {
    Tcl_WrongNumArgs(interp, 1, objv, "posid");
    return TCL_ERROR;
  }

  const PosList& plist = PosList::thePosList();
  Position* p = getPosFromArg(interp, objv, plist, 1);
  if ( p == NULL ) return TCL_ERROR;

  const char* name = typeid(*p).name();
  if ( name != NULL ) {
    Tcl_SetObjResult(interp, Tcl_NewStringObj(name,-1));
  }
  return TCL_OK;
}

///////////////////////////////////////////////////////////////////////
//
// PosListPkg class definition
//
///////////////////////////////////////////////////////////////////////

class PosListPkg : public TclListPkg {
public:
  PosListPkg(Tcl_Interp* interp) :
	 TclListPkg(interp, "PosList", "2.5")
  {
	 Tcl_CreateObjCommand(interp, "PosList::posType", PoslistTcl::posTypeCmd,
								 (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);

	 Tcl_Eval(interp, 
				 "namespace eval PosList {\n"
				 "  proc resetPosList {} { PosList::reset }\n"
				 "  proc posCount {} { return [PosList::count] }\n"
				 "  namespace export posType\n"
				 "  namespace export resetPosList\n"
				 "  namespace export posCount\n"
				 "}");
	 Tcl_Eval(interp,
				 "namespace import PosList::posType\n"
				 "namespace import PosList::resetPosList\n"
				 "namespace import PosList::posCount\n");
  }

  virtual IO& getList() { return PosList::thePosList(); }
  virtual int getBufSize() { return 200; }

  virtual void reset() { PosList::thePosList().clearPos(); }
  virtual int count() { return PosList::thePosList().posCount(); }
};

//---------------------------------------------------------------------
//
// PoslistTcl::Poslist_Init --
//
//---------------------------------------------------------------------

int PoslistTcl::Poslist_Init(Tcl_Interp* interp) {
DOTRACE("PoslistTcl::Poslist_Init");

  new PosListPkg(interp);

  return TCL_OK;
}

static const char vcid_poslisttcl_cc[] = "$Header$";
#endif // !POSLISTTCL_CC_DEFINED
