///////////////////////////////////////////////////////////////////////
// poslisttcl.cc
// Rob Peters
// created: Sat Mar 13 12:46:09 1999
// written: Sun Apr 25 13:13:28 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef POSLISTTCL_CC_DEFINED
#define POSLISTTCL_CC_DEFINED

#include "poslisttcl.h"

#include <typeinfo>
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
  Tcl_ObjCmdProc posCountCmd;
  Tcl_ObjCmdProc resetPosListCmd;
  Tcl_ObjCmdProc posTypeCmd;
  Tcl_ObjCmdProc stringifyCmd;
  Tcl_ObjCmdProc destringifyCmd;
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

// return the number of objects in thePosList
int PoslistTcl::posCountCmd(ClientData, Tcl_Interp *interp,
                           int objc, Tcl_Obj *const objv[]) {
DOTRACE("PoslistTcl::posCountCmd");
  if (objc > 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  const PosList& plist = getPosList();
  Tcl_SetObjResult(interp, Tcl_NewIntObj(plist.posCount()));
  return TCL_OK;
}

// clear the ObjList of all GrObj's by calling ObjList::clearObjs()
int PoslistTcl::resetPosListCmd(ClientData, Tcl_Interp *interp,
                                int objc, Tcl_Obj *const objv[]) {
DOTRACE("PoslistTcl::resetPosListCmd");
  if (objc > 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  PosList& plist = getPosList();
  plist.clearPos();

  return TCL_OK;
}

// return an object's name
int PoslistTcl::posTypeCmd(ClientData, Tcl_Interp *interp,
                           int objc, Tcl_Obj *const objv[]) {
DOTRACE("PoslistTcl::posTypeCmd");
  if (objc != 2)  {
    Tcl_WrongNumArgs(interp, 1, objv, "posid");
    return TCL_ERROR;
  }

  const PosList& plist = getPosList();
  Position *p = getPosFromArg(interp, objv, plist, 1);
  if ( p == NULL ) return TCL_ERROR;

  const char *name = typeid(*p).name();
  if ( name != NULL ) {
    Tcl_SetObjResult(interp, Tcl_NewStringObj(name,-1));
  }
  return TCL_OK;
}

int PoslistTcl::stringifyCmd(ClientData, Tcl_Interp *interp,
                                     int objc, Tcl_Obj *const objv[]) {
DOTRACE("PoslistTcl::stringifyCmd");
  if (objc != 1)  {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  const PosList& plist = getPosList();

  const int BUF_SIZE = 200;
  char buf[BUF_SIZE];
  ostrstream ost(buf, BUF_SIZE);

  try {
	 plist.serialize(ost, IO::BASES|IO::TYPENAME);
	 ost << '\0';
  }
  catch (IoError& err) {
	 err_message(interp, objv, err.info().c_str());
	 return TCL_ERROR;
  }
  Tcl_SetObjResult(interp, Tcl_NewStringObj(buf, -1));
  return TCL_OK;
}

int PoslistTcl::destringifyCmd(ClientData, Tcl_Interp *interp,
                                     int objc, Tcl_Obj *const objv[]) {
DOTRACE("PoslistTcl::destringifyCmd");
  if (objc != 2)  {
    Tcl_WrongNumArgs(interp, 1, objv, "string");
    return TCL_ERROR;
  }

  PosList& plist = getPosList();

  const char* buf = Tcl_GetString(objv[1]);
  if (buf == NULL) return TCL_ERROR;

  istrstream ist(buf);

  try {
	 plist.deserialize(ist, IO::BASES|IO::TYPENAME);
  }
  catch (IoError& err) {
	 err_message(interp, objv, err.info().c_str());
	 return TCL_ERROR;
  }
  return TCL_OK;
}

int PoslistTcl::Poslist_Init(Tcl_Interp *interp) {
DOTRACE("PoslistTcl::Poslist_Init");
  // Add all commands to the ::PosList namespace
  Tcl_CreateObjCommand(interp, "PosList::posCount", posCountCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "PosList::resetPosList", resetPosListCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "PosList::posType", posTypeCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "PosList::stringify", stringifyCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "PosList::destringify", destringifyCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);

  Tcl_Eval(interp, 
			  "namespace eval PosList {\n"
			  "  namespace export posType\n"
			  "  namespace export resetPosList\n"
			  "  namespace export posCount\n"
			  "}");
  Tcl_Eval(interp,
			  "namespace import PosList::posType\n"
			  "namespace import PosList::resetPosList\n"
			  "namespace import PosList::posCount\n");

  Tcl_PkgProvide(interp, "Poslist", "2.1");
  return TCL_OK;
}

static const char vcid_poslisttcl_cc[] = "$Header$";
#endif // !POSLISTTCL_CC_DEFINED
