///////////////////////////////////////////////////////////////////////
// fixpttcl.cc
// Rob Peters
// created: Jan-99
// written: Mon Mar 15 15:47:28 1999
static const char vcid_fixpttcl_cc[] = "$Id$";
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
  FixPt* getFixptFromArg(Tcl_Interp *interp, Tcl_Obj *const objv[],
								 const ObjList& olist, int argn);

  Tcl_ObjCmdProc fixptCmd;
  using ObjlistTcl::ObjlistTcl_SubcmdProc;
  ObjlistTcl_SubcmdProc lengthCmd;
  ObjlistTcl_SubcmdProc widthCmd;

  Tcl_PackageInitProc Fixpt_Init;

  // these constants are passed to err_message() in case of error
  using ObjlistTcl::cant_make_obj;
  using ObjlistTcl::wrong_type_msg;
  const char* const bad_command = ": invalid command";
}

///////////////////////////////////////////////////////////////////////
// FixPt Tcl package definitions
///////////////////////////////////////////////////////////////////////

FixPt* FixptTcl::getFixptFromArg(Tcl_Interp *interp, Tcl_Obj *const objv[],
											const ObjList& olist, int argn) {
DOTRACE("FixptTcl::getFixptFromArg");
  // Get the object
  GrObj *g = ObjlistTcl::getObjFromArg(interp, objv, olist, argn);
  
  // Make sure it's a fixpt object
  FixPt *p = dynamic_cast<FixPt *>(g);

  if ( p == NULL ) {
	 err_message(interp, objv, wrong_type_msg);
  }
  return p;
}

int FixptTcl::fixptCmd(ClientData, Tcl_Interp *interp,
                       int objc, Tcl_Obj *const objv[]) {
DOTRACE("FixptTcl::fixptCmd");
  ObjList& olist = ObjlistTcl::getObjList();
  int id;

  float len=0.1;
  int wid=1;
  if (objc == 1) {              // make new object
    FixPt *p = new FixPt(len, wid);

    if ( (id = olist.addObj(p)) < 0) {
      err_message(interp, objv, cant_make_obj);
      delete p;
      return TCL_ERROR;
    }

    Tcl_SetObjResult(interp, Tcl_NewIntObj(id));
    return TCL_OK;
  }
  else if (objc >= 2 ) {        // subcommand
    char* cmd = Tcl_GetString(objv[1]);

    if ( strcmp(cmd, "length") == 0 ) {
      lengthCmd(olist, interp, objc, objv);
    }
    else if ( strcmp(cmd, "width") == 0 ) {
      widthCmd(olist, interp, objc, objv);
    }
    else {
      err_message(interp, objv, bad_command);
      return TCL_ERROR;
    }
  }
  return TCL_OK;
}

int FixptTcl::lengthCmd(const ObjList& olist, Tcl_Interp *interp,
                        int objc, Tcl_Obj *const objv[]) {
DOTRACE("FixptTcl::lengthCmd");
  FixPt *p = NULL;
  double len;
  
  if (objc != 4) {
    Tcl_WrongNumArgs(interp, 2, objv, "objid length_val");
    return TCL_ERROR;
  }
  
  // Get the fixpt
  if ( (p = getFixptFromArg(interp, objv, olist, 2)) == NULL )
    return TCL_ERROR;

  if ( Tcl_GetDoubleFromObj(interp, objv[3], &len) != TCL_OK ) return TCL_ERROR;

  p->setLength(len);
#ifdef LOCAL_DEBUG
  DUMP_VAL2(len);
#endif
  return TCL_OK;
}

int FixptTcl::widthCmd(const ObjList& olist, Tcl_Interp *interp,
                       int objc, Tcl_Obj *const objv[]) {
DOTRACE("FixptTcl::widthCmd");
  FixPt *p = NULL;
  int wid;
  
  if (objc != 4) {
    Tcl_WrongNumArgs(interp, 2, objv, "objid width_val");
    return TCL_ERROR;
  }
  
  // Get the fixpt
  if ( (p = getFixptFromArg(interp, objv, olist, 2)) == NULL )
    return TCL_ERROR;

  if ( Tcl_GetIntFromObj(interp, objv[3], &wid) != TCL_OK ) return TCL_ERROR;

  p->setWidth(wid);
#ifdef LOCAL_DEBUG
  DUMP_VAL1(wid);
#endif
  return TCL_OK;
}

int FixptTcl::Fixpt_Init(Tcl_Interp *interp) {
DOTRACE("FixptTcl::Fixpt_Init");
  Tcl_CreateObjCommand(interp, "fixpt", fixptCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  return TCL_OK;
}

#endif // !FIXPTTCL_CC_DEFINED
