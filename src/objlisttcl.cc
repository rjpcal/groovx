///////////////////////////////////////////////////////////////////////
// objlisttcl.cc
// Rob Peters
// created: Jan-99
// written: Sun Apr 25 13:13:28 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef OBJLISTTCL_CC_DEFINED
#define OBJLISTTCL_CC_DEFINED

#include "objlisttcl.h"

#include <tcl.h>
#include <string>
#include <strstream.h>
#include <typeinfo>
#include <vector>

#include "errmsg.h"
#include "objlist.h"

#define NO_TRACE
#include "trace.h"
#include "debug.h"

///////////////////////////////////////////////////////////////////////
// Objlist Tcl package
///////////////////////////////////////////////////////////////////////

namespace ObjlistTcl {
  Tcl_ObjCmdProc resetObjListCmd;
  Tcl_ObjCmdProc allObjsCmd;
  Tcl_ObjCmdProc objCountCmd;
  Tcl_ObjCmdProc objTypeCmd;
  Tcl_ObjCmdProc stringifyCmd;
  Tcl_ObjCmdProc destringifyCmd;
}

///////////////////////////////////////////////////////////////////////
// Objlist Tcl package definitions
///////////////////////////////////////////////////////////////////////

// utility functions

ObjList& ObjlistTcl::getObjList() {
  static const int DEFAULT_SIZE = 100;
  static ObjList theObjList(DEFAULT_SIZE);
  return theObjList;
}

GrObj* ObjlistTcl::getObjFromArg(Tcl_Interp *interp, Tcl_Obj *const objv[], 
                                 const ObjList& olist, int argn) {
DOTRACE("ObjlistTcl::getObjFromArg");
  int id;
  // Make sure we have a valid objid
  if ( Tcl_GetIntFromObj(interp, objv[argn], &id) != TCL_OK ) return NULL;
#ifdef LOCAL_DEBUG
  DUMP_VAL1(olist->objCount());
  DUMP_VAL2(id);
#endif
  if ( !olist.isValidObjid(id) ) {
    err_message(interp, objv, bad_objid_msg);
    return NULL;
  }

  // Get the object
  return olist.getObj(id);
}

int ObjlistTcl::allObjsCmd(ClientData, Tcl_Interp *interp,
                          int objc, Tcl_Obj *const objv[]) {
DOTRACE("ObjlistTcl::allObjsCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  ObjList& olist = getObjList();
  vector<int> objids;
  olist.getValidObjids(objids);

  Tcl_Obj *list_out = Tcl_NewListObj(0, (Tcl_Obj **) NULL);

  for (int i = 0; i < objids.size(); i++) {
	 Tcl_Obj *id_obj = Tcl_NewIntObj(objids[i]);
	 if (Tcl_ListObjAppendElement(interp, list_out, id_obj) != TCL_OK)
		return TCL_ERROR;
  }
  Tcl_SetObjResult(interp, list_out);
  return TCL_OK;
}

// clear the ObjList of all GrObj's by calling ObjList::clearObjs()
int ObjlistTcl::resetObjListCmd(ClientData, Tcl_Interp *interp,
                                int objc, Tcl_Obj *const objv[]) {
DOTRACE("ObjlistTcl::resetObjListCmd");
  if (objc > 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  ObjList& olist = getObjList();
  olist.clearObjs();

  return TCL_OK;
}

// return the number of objects in theObjList
int ObjlistTcl::objCountCmd(ClientData, Tcl_Interp *interp,
                           int objc, Tcl_Obj *const objv[]) {
DOTRACE("ObjlistTcl::objCountCmd");
  if (objc > 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  const ObjList& olist = getObjList();
  Tcl_SetObjResult(interp, Tcl_NewIntObj(olist.objCount()));
  return TCL_OK;
}

// return an object's name
int ObjlistTcl::objTypeCmd(ClientData, Tcl_Interp *interp,
                           int objc, Tcl_Obj *const objv[]) {
DOTRACE("ObjlistTcl::objTypeCmd");
  if (objc != 2)  {
    Tcl_WrongNumArgs(interp, 1, objv, "objid");
    return TCL_ERROR;
  }

  const ObjList& olist = getObjList();
  GrObj *g = getObjFromArg(interp, objv, olist, 1);
  if ( g == NULL ) return TCL_ERROR;

  const char *name = typeid(*g).name();
  if ( name != NULL ) {
    Tcl_SetObjResult(interp, Tcl_NewStringObj(name,-1));
  }
  return TCL_OK;
}

int ObjlistTcl::stringifyCmd(ClientData, Tcl_Interp *interp,
                                     int objc, Tcl_Obj *const objv[]) {
DOTRACE("ObjlistTcl::stringifyCmd");
  if (objc != 1)  {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  const ObjList& olist = getObjList();

  // Try to guess (conservatively) how much space we'll need to avoid
  // overflows when possible
  //
  //   20 chars for first line with ObjList ...
  //   40 chars for each object in list
  //   10 chars for last line
  int BUF_SIZE = 20 + 40*olist.objCount() + 10;

  auto_ptr<char> buf(new char[BUF_SIZE]);
  ostrstream ost(buf.get(), BUF_SIZE);

  try {
	 olist.serialize(ost, IO::BASES|IO::TYPENAME);
	 ost << '\0';
  }
  catch (IoError& err) {
	 string msg = ": " + err.info();
	 err_message(interp, objv, msg.c_str());
	 return TCL_ERROR;
  }	 
  Tcl_SetObjResult(interp, Tcl_NewStringObj(buf.get(), -1));
  return TCL_OK;
}

int ObjlistTcl::destringifyCmd(ClientData, Tcl_Interp *interp,
                                     int objc, Tcl_Obj *const objv[]) {
DOTRACE("ObjlistTcl::destringifyCmd");
  if (objc != 2)  {
    Tcl_WrongNumArgs(interp, 1, objv, "string");
    return TCL_ERROR;
  }

  ObjList& olist = getObjList();

  const char* buf = Tcl_GetString(objv[1]);
  if (buf == NULL) return TCL_ERROR;

  istrstream ist(buf);

  try {
	 olist.deserialize(ist, IO::BASES|IO::TYPENAME);
  }
  catch (IoError& err) {
	 err_message(interp, objv, err.info().c_str());
	 return TCL_ERROR;
  }
  return TCL_OK;
}

int ObjlistTcl::Objlist_Init(Tcl_Interp *interp) {
DOTRACE("ObjlistTcl::Objlist_Init");
  // Add all commands to the ::ObjList namespace
  Tcl_CreateObjCommand(interp, "ObjList::resetObjList", resetObjListCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "ObjList::objCount", objCountCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "ObjList::objType", objTypeCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "ObjList::stringify", stringifyCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "ObjList::destringify", destringifyCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);

  Tcl_Eval(interp, 
			  "namespace eval ObjList {\n"
			  "  namespace export objType\n"
			  "  namespace export resetObjList\n"
			  "  namespace export objCount\n"
			  "}");
  Tcl_Eval(interp,
			  "namespace import ObjList::objType\n"
			  "namespace import ObjList::resetObjList\n"
			  "namespace import ObjList::objCount\n");

  Tcl_PkgProvide(interp, "Objlist", "2.1");
  return TCL_OK;
}

static const char vcid_objlisttcl_cc[] = "$Header$";
#endif // !OBJLISTTCL_CC_DEFINED
