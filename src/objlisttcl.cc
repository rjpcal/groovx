///////////////////////////////////////////////////////////////////////
// objlisttcl.cc
// Rob Peters
// created: Jan-99
// written: Sun Mar 14 21:13:24 1999
///////////////////////////////////////////////////////////////////////

#ifndef OBJLISTTCL_CC_DEFINED
#define OBJLISTTCL_CC_DEFINED

#include "objlisttcl.h"

#include <strstream.h>
#include <typeinfo>
#include <tcl.h>

#include "errmsg.h"
#include "objlist.h"

#define NO_TRACE
#include "trace.h"
#include "debug.h"

///////////////////////////////////////////////////////////////////////
// Objlist Tcl package
///////////////////////////////////////////////////////////////////////

namespace ObjlistTcl {
  // global ObjList pointer
  ObjList *theObjList = NULL;

  Tcl_ObjCmdProc resetObjListCmd;
  Tcl_ObjCmdProc objTypeCmd;
  Tcl_ObjCmdProc stringify_objlistCmd;
  Tcl_ObjCmdProc destringify_objlistCmd;
}

///////////////////////////////////////////////////////////////////////
// Objlist Tcl package definitions
///////////////////////////////////////////////////////////////////////

// utility functions

GrObj* ObjlistTcl::getObjFromArg(Tcl_Interp *interp, Tcl_Obj *const objv[], 
											ObjList *olist, int argn) {
DOTRACE("ObjlistTcl::getObjFromArg");
  int id;
  // Make sure we have a valid objid
  if ( Tcl_GetIntFromObj(interp, objv[argn], &id) != TCL_OK ) return NULL;
#ifdef LOCAL_DEBUG
  DUMP_VAL1(olist->nobjs());
  DUMP_VAL2(id);
#endif
  if ( !olist->isValidObjid(id) ) {
    err_message(interp, objv, bad_objid_msg);
    return NULL;
  }

  // Get the object
  return olist->getObj(id);
}

// clear the ObjList of all GrObj's by calling ObjList::clearObjs()
int ObjlistTcl::resetObjListCmd(ClientData, Tcl_Interp *interp,
										  int objc, Tcl_Obj *const objv[]) {
DOTRACE("ObjlistTcl::resetObjListCmd");
  if (objc > 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  ObjList *olist = getObjList();
  olist->clearObjs();

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

  ObjList *olist = getObjList();
  GrObj *g = getObjFromArg(interp, objv, olist, 1);
  if ( g == NULL ) return TCL_ERROR;

  const char *name = typeid(*g).name();
  if ( name != NULL ) {
    Tcl_SetObjResult(interp, Tcl_NewStringObj(name,-1));
  }
  return TCL_OK;
}

int ObjlistTcl::stringify_objlistCmd(ClientData, Tcl_Interp *interp,
												 int objc, Tcl_Obj *const objv[]) {
DOTRACE("ObjlistTcl::stringify_objlistCmd");
  if (objc != 1)  {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  ObjList *olist = getObjList();

  const int BUF_SIZE = 200;
  char buf[BUF_SIZE];
  ostrstream ost(buf, BUF_SIZE);

  olist->serialize(ost, IO::IOFlag(IO::BASES|IO::TYPENAME));
  Tcl_SetObjResult(interp, Tcl_NewStringObj(buf, -1));
  return TCL_OK;
}

int ObjlistTcl::destringify_objlistCmd(ClientData, Tcl_Interp *interp,
												 int objc, Tcl_Obj *const objv[]) {
DOTRACE("ObjlistTcl::destringify_objlistCmd");
  if (objc != 2)  {
    Tcl_WrongNumArgs(interp, 1, objv, "string");
    return TCL_ERROR;
  }

  ObjList *olist = getObjList();

  const char* buf = Tcl_GetString(objv[1]);
  if (buf == NULL) return TCL_ERROR;

  istrstream ist(buf);

  olist->deserialize(ist, IO::IOFlag(IO::BASES|IO::TYPENAME));
  return TCL_OK;
}

int ObjlistTcl::Objlist_Init(Tcl_Interp *interp) {
DOTRACE("ObjlistTcl::Objlist_Init");
  static const int DEFAULT_SIZE = 100;
  ObjlistTcl::theObjList = new ObjList(DEFAULT_SIZE);

  Tcl_CreateObjCommand(interp, "resetObjList", resetObjListCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "objType", objTypeCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "stringify_objlist", stringify_objlistCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "destringify_objlist", destringify_objlistCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);

  Tcl_PkgProvide(interp, "Objlist", "2.1");
  return TCL_OK;
}

static const char vcid_objlisttcl_cc[] = "$Id$";
#endif // !OBJLISTTCL_CC_DEFINED
