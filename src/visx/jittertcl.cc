///////////////////////////////////////////////////////////////////////
// jittertcl.cc
// Rob Peters
// created: Wed Apr  7 14:58:40 1999
// written: Fri Apr 16 18:25:53 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef JITTERTCL_CC_DEFINED
#define JITTERTCL_CC_DEFINED

#include "jittertcl.h"

#include <tcl.h>
#include <strstream.h>

#include "errmsg.h"
#include "jitter.h"
#include "poslist.h"
#include "poslisttcl.h"

#define NO_TRACE
#include "trace.h"
#include "debug.h"

namespace JitterTcl {
  Tcl_ObjCmdProc jitterCmd;
  Tcl_ObjCmdProc setJitterCmd;
}

int JitterTcl::jitterCmd(ClientData, Tcl_Interp *interp,
								 int objc, Tcl_Obj *const objv[]) {
DOTRACE("JitterTcl::jitterCmd");
  if (objc > 1) {
	 Tcl_WrongNumArgs(interp, 1, objv, NULL);
	 return TCL_ERROR;
  }

  Jitter *j = new Jitter();

  PosList& plist = PoslistTcl::getPosList();
  int id = plist.addPos(j);

  Tcl_SetObjResult(interp, Tcl_NewIntObj(id));
  return TCL_OK;
}

int JitterTcl::setJitterCmd(ClientData, Tcl_Interp *interp,
									 int objc, Tcl_Obj *const objv[]) {
DOTRACE("JitterTcl::setJitterCmd");
  if (objc != 5) {
	 Tcl_WrongNumArgs(interp, 1, objv, "posid x_jitter y_jitter r_jitter");
	 return TCL_ERROR;
  }

  const PosList& plist = PoslistTcl::getPosList();
  Position *p = PoslistTcl::getPosFromArg(interp, objv, plist, 1);
  if ( p == NULL ) return TCL_ERROR;
  Jitter *j = dynamic_cast<Jitter *>(p);
  if ( j == NULL ) {
	 err_message(interp, objv, "position not of type jitter");
	 return TCL_ERROR;
  }

  double xj, yj, rj;
  if (Tcl_GetDoubleFromObj(interp, objv[2], &xj) != TCL_OK) return TCL_ERROR;
  if (Tcl_GetDoubleFromObj(interp, objv[3], &yj) != TCL_OK) return TCL_ERROR;
  if (Tcl_GetDoubleFromObj(interp, objv[4], &rj) != TCL_OK) return TCL_ERROR;

  j->setXJitter(xj);
  j->setYJitter(yj);
  j->setRJitter(rj);
  return TCL_OK;
}

int JitterTcl::Jitter_Init(Tcl_Interp *interp) {
DOTRACE("JitterTcl::Jitter_Init");
  // Add all commands to ::Jitter namespace
  Tcl_CreateObjCommand(interp, "Jitter::jitter", jitterCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "Jitter::setJitter", setJitterCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_Eval(interp,
			  "namespace eval Jitter {\n"
			  "  namespace export jitter\n"
			  "  namespace export setJitter\n"
			  "}");
  Tcl_Eval(interp,
			  "namespace import Jitter::jitter\n"
			  "namespace import Jitter::setJitter\n");

  Tcl_PkgProvide(interp, "Jitter", "2.1");
  return TCL_OK;
}

static const char vcid_jittertcl_cc[] = "$Header$";
#endif // !JITTERTCL_CC_DEFINED
