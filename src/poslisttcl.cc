///////////////////////////////////////////////////////////////////////
// poslisttcl.cc
// Rob Peters
// created: Sat Mar 13 12:46:09 1999
// written: Sat Mar 13 13:35:52 1999
///////////////////////////////////////////////////////////////////////

#ifndef POSLISTTCL_CC_DEFINED
#define POSLISTTCL_CC_DEFINED

#include "poslisttcl.h"

#include <tcl.h>

#include "errmsg.h"
#include "poslist.h"

#define NO_TRACE
#include "trace.h"
#include "debug.h"

///////////////////////////////////////////////////////////////////////
// Poslist Tcl package
///////////////////////////////////////////////////////////////////////

namespace PoslistTcl {
  // global PosList pointer
  PosList *thePosList = NULL;

  Tcl_ObjCmdProc positionCmd;
}

Position* PoslistTcl::getPosFromArg(Tcl_Interp *interp, Tcl_Obj *const objv[], 
												PosList *plist, int argn) {
DOTRACE("PoslistTcl::getPosFromArg");
  // Make sure we have a valid posid
  int id;
  if ( Tcl_GetIntFromObj(interp, objv[argn], &id) != TCL_OK ) return NULL;

  if ( !plist->isValidPosid(id) ) {
    err_message(interp, objv, bad_posid_msg);
    return NULL;
  }

  // Get the position
  return plist->getPos(id);
}

int FaceTcl::positionCmd(ClientData, Tcl_Interp *interp,
								 int objc, Tcl_Obj *const objv[]) {
DOTRACE("FaceTcl::positionCmd");
  if (objc > 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }
  
  Position *p = new Position();
  
  PosList *plist = PoslistTcl::getPosList();
  int id = plist->addPos(p);
  if (id < 0) {
    err_message(interp, objv, cant_make_pos);
    delete p;
    return TCL_ERROR;
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(id));
  return TCL_OK;
}

int PoslistTcl::Poslist_Init(Tcl_Interp *interp) {
DOTRACE("PoslistTcl::Poslist_Init");
  static const int DEFAULT_SIZE = 10;
  PoslistTcl::thePosList = new PosList(DEFAULT_SIZE);

  Tcl_CreateObjCommand(interp, "position", positionCmd,
							  (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  return TCL_OK;
}

#endif // !POSLISTTCL_CC_DEFINED
