///////////////////////////////////////////////////////////////////////
// fixpttcl.cc
// Rob Peters
// created: Jan-99
// written: Sun Jun 20 17:51:59 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef FIXPTTCL_CC_DEFINED
#define FIXPTTCL_CC_DEFINED

#include <tcl.h>
#include <cstring>

#include "errmsg.h"

#include "objlist.h"
#include "objlisttcl.h"
#include "fixpt.h"

#define NO_TRACE
#include "trace.h"
#include "debug.h"

///////////////////////////////////////////////////////////////////////
// FixPt Tcl package declarations 
///////////////////////////////////////////////////////////////////////

namespace FixptTcl {
  FixPt* getFixptFromArg(Tcl_Interp* interp, Tcl_Obj* const objv[],
                         int argn);

  Tcl_ObjCmdProc fixptCmd;
  using ObjlistTcl::ObjlistTcl_SubcmdProc;
  ObjlistTcl_SubcmdProc lengthCmd;
  ObjlistTcl_SubcmdProc widthCmd;

  // these constants are passed to err_message() in case of error
  using ObjlistTcl::wrong_type_msg;
  const char* const bad_command = "invalid command";
}

///////////////////////////////////////////////////////////////////////
// FixPt Tcl package definitions
///////////////////////////////////////////////////////////////////////

FixPt* FixptTcl::getFixptFromArg(Tcl_Interp* interp, Tcl_Obj* const objv[],
                                 int argn) {
DOTRACE("FixptTcl::getFixptFromArg");
  // Get the object
  GrObj* g = ObjlistTcl::getObjFromArg(interp, objv, argn);
  if ( g == NULL ) return NULL;

  // Make sure it's a fixpt object
  FixPt* p = dynamic_cast<FixPt *>(g);
  if ( p == NULL ) {
    err_message(interp, objv, wrong_type_msg);
  }
  return p;
}

int FixptTcl::fixptCmd(ClientData, Tcl_Interp* interp,
                       int objc, Tcl_Obj* const objv[]) {
DOTRACE("FixptTcl::fixptCmd");
  double len=0.1;
  int wid=1;
  if (objc == 1) {              // make new object
    FixPt* p = new FixPt(len, wid);

	 ObjId objid = ObjList::theObjList().addObj(p);

    Tcl_SetObjResult(interp, Tcl_NewIntObj(objid.toInt()));
    return TCL_OK;
  }
  else if (objc >= 2 ) {        // subcommand
    char* cmd = Tcl_GetString(objv[1]);

    if ( strcmp(cmd, "length") == 0 ) {
      lengthCmd(interp, objc, objv);
    }
    else if ( strcmp(cmd, "width") == 0 ) {
      widthCmd(interp, objc, objv);
    }
    else {
      err_message(interp, objv, bad_command);
      return TCL_ERROR;
    }
  }
  return TCL_OK;
}

int FixptTcl::lengthCmd(Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) {
DOTRACE("FixptTcl::lengthCmd");
  if (objc != 4) {
    Tcl_WrongNumArgs(interp, 2, objv, "objid length_val");
    return TCL_ERROR;
  }
  
  // Get the fixpt
  FixPt* p = getFixptFromArg(interp, objv, 2);
  if ( p == NULL ) return TCL_ERROR;

  double len;
  if ( Tcl_GetDoubleFromObj(interp, objv[3], &len) != TCL_OK ) return TCL_ERROR;

  p->setLength(len);

  DebugEvalNL(len);

  return TCL_OK;
}

int FixptTcl::widthCmd(Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) {
DOTRACE("FixptTcl::widthCmd");
  if (objc != 4) {
    Tcl_WrongNumArgs(interp, 2, objv, "objid width_val");
    return TCL_ERROR;
  }
  
  // Get the fixpt
  FixPt* p = getFixptFromArg(interp, objv, 2);
  if ( p == NULL ) return TCL_ERROR;

  int wid;
  if ( Tcl_GetIntFromObj(interp, objv[3], &wid) != TCL_OK ) return TCL_ERROR;

  p->setWidth(wid);

  DebugEval(wid);

  return TCL_OK;
}

extern "C" Tcl_PackageInitProc Fixpt_Init;

int Fixpt_Init(Tcl_Interp* interp) {
DOTRACE("Fixpt_Init");
  Tcl_CreateObjCommand(interp, "fixpt", FixptTcl::fixptCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_PkgProvide(interp, "Fixpt", "2.1");
  return TCL_OK;
}

static const char vcid_fixpttcl_cc[] = "$Header$";
#endif // !FIXPTTCL_CC_DEFINED
