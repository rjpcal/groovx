///////////////////////////////////////////////////////////////////////
// poslisttcl.cc
// Rob Peters
// created: Sat Mar 13 12:46:09 1999
// written: Tue Mar 16 19:34:07 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef POSLISTTCL_CC_DEFINED
#define POSLISTTCL_CC_DEFINED

#include "poslisttcl.h"

#include <tcl.h>
#include <strstream.h>

#include "errmsg.h"
#include "poslist.h"

#define NO_TRACE
#include "trace.h"
#include "debug.h"

///////////////////////////////////////////////////////////////////////
// Poslist Tcl package
///////////////////////////////////////////////////////////////////////

namespace PoslistTcl {
  Tcl_ObjCmdProc stringify_poslistCmd;
  Tcl_ObjCmdProc destringify_poslistCmd;
}

PosList& PoslistTcl::getPosList() {
  static const int DEFAULT_SIZE = 10;
  static PosList thePosList(DEFAULT_SIZE);
  return thePosList;
}

Position* PoslistTcl::getPosFromArg(Tcl_Interp *interp, Tcl_Obj *const objv[], 
                                    const PosList& plist, int argn) {
DOTRACE("PoslistTcl::getPosFromArg");
  // Make sure we have a valid posid
  int id;
  if ( Tcl_GetIntFromObj(interp, objv[argn], &id) != TCL_OK ) return NULL;

  if ( !plist.isValidPosid(id) ) {
    err_message(interp, objv, bad_posid_msg);
    return NULL;
  }

  // Get the position
  return plist.getPos(id);
}

int PoslistTcl::stringify_poslistCmd(ClientData, Tcl_Interp *interp,
                                     int objc, Tcl_Obj *const objv[]) {
DOTRACE("PoslistTcl::stringify_poslistCmd");
  if (objc != 1)  {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  const PosList& plist = getPosList();

  const int BUF_SIZE = 200;
  char buf[BUF_SIZE];
  ostrstream ost(buf, BUF_SIZE);

  plist.serialize(ost, IO::IOFlag(IO::BASES|IO::TYPENAME));
  Tcl_SetObjResult(interp, Tcl_NewStringObj(buf, -1));
  return TCL_OK;
}

int PoslistTcl::destringify_poslistCmd(ClientData, Tcl_Interp *interp,
                                     int objc, Tcl_Obj *const objv[]) {
DOTRACE("PoslistTcl::destringify_poslistCmd");
  if (objc != 2)  {
    Tcl_WrongNumArgs(interp, 1, objv, "string");
    return TCL_ERROR;
  }

  PosList& plist = getPosList();

  const char* buf = Tcl_GetString(objv[1]);
  if (buf == NULL) return TCL_ERROR;

  istrstream ist(buf);

  plist.deserialize(ist, IO::IOFlag(IO::BASES|IO::TYPENAME));
  return TCL_OK;
}

int PoslistTcl::Poslist_Init(Tcl_Interp *interp) {
DOTRACE("PoslistTcl::Poslist_Init");
  Tcl_CreateObjCommand(interp, "stringify_poslist", stringify_poslistCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "destringify_poslist", destringify_poslistCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);

  Tcl_PkgProvide(interp, "Poslist", "2.1");
  return TCL_OK;
}

static const char vcid_poslisttcl_cc[] = "$Header$";
#endif // !POSLISTTCL_CC_DEFINED
