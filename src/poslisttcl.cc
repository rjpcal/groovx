///////////////////////////////////////////////////////////////////////
//
// poslisttcl.cc
// Rob Peters
// created: Sat Mar 13 12:46:09 1999
// written: Mon Jun  7 11:41:53 1999
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
  Tcl_ObjCmdProc posCountCmd;
  Tcl_ObjCmdProc resetPosListCmd;
  Tcl_ObjCmdProc posTypeCmd;
  Tcl_ObjCmdProc stringifyCmd;
  Tcl_ObjCmdProc destringifyCmd;
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

// return the number of objects in thePosList
int PoslistTcl::posCountCmd(ClientData, Tcl_Interp* interp,
                           int objc, Tcl_Obj* const objv[]) {
DOTRACE("PoslistTcl::posCountCmd");
  if (objc > 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  const PosList& plist = PosList::thePosList();
  Tcl_SetObjResult(interp, Tcl_NewIntObj(plist.posCount()));
  return TCL_OK;
}

// clear the ObjList of all GrObj's by calling ObjList::clearObjs()
int PoslistTcl::resetPosListCmd(ClientData, Tcl_Interp* interp,
                                int objc, Tcl_Obj* const objv[]) {
DOTRACE("PoslistTcl::resetPosListCmd");
  if (objc > 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  PosList::thePosList().clearPos();

  return TCL_OK;
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

int PoslistTcl::stringifyCmd(ClientData, Tcl_Interp* interp,
                                     int objc, Tcl_Obj* const objv[]) {
DOTRACE("PoslistTcl::stringifyCmd");
  if (objc != 1)  {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  const PosList& plist = PosList::thePosList();

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

int PoslistTcl::destringifyCmd(ClientData, Tcl_Interp* interp,
                                     int objc, Tcl_Obj* const objv[]) {
DOTRACE("PoslistTcl::destringifyCmd");
  if (objc != 2)  {
    Tcl_WrongNumArgs(interp, 1, objv, "string");
    return TCL_ERROR;
  }

  PosList& plist = PosList::thePosList();

  const char* buf = Tcl_GetString(objv[1]);
  Assert(buf);

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

int PoslistTcl::Poslist_Init(Tcl_Interp* interp) {
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
