///////////////////////////////////////////////////////////////////////
// grobjtcl.cc
// Rob Peters
// created: Wed Feb 10 19:30:37 1999
// written: Fri Mar 12 17:10:45 1999
static const char vcid_grobjtcl_cc[] = "$Id$";
///////////////////////////////////////////////////////////////////////

#ifndef GROBJTCL_CC_DEFINED
#define GROBJTCL_CC_DEFINED

#include "grobjtcl.h"

#include <tcl.h>

#include "errmsg.h"
#include "objlist.h"
#include "objlisttcl.h"

#define NO_TRACE
#include "trace.h"
#include "debug.h"

int GrobjTcl::grobjCmd(ClientData clientData, Tcl_Interp *interp,
							  int objc, Tcl_Obj *const objv[], 
							  newGrobjProc theGrobjProc, ProcMap &theProcMap) {
  int id;
  
  ObjList *olist = (ObjList *) clientData;

  if (objc == 1) {
	 GrObj *theObj = theGrobjProc();
	 if ( (id = olist->addObj(theObj)) < 0) {
		err_message(interp, objv, ObjlistTcl::cant_make_obj);
		delete theObj;
		return TCL_ERROR;
	 }
	 Tcl_SetObjResult(interp, Tcl_NewIntObj(id));
	 return TCL_OK;
  }
  else {								  // subcommand
	 string cmd = Tcl_GetString(objv[1]);
  
	 if ( theProcMap[cmd] == 0 ) {
		err_message(interp, objv, bad_command);
		return TCL_ERROR;
	 }
	 else {
		return ( theProcMap[cmd](olist, interp, objc, objv) );
	 }
  }
}

int GrobjTcl::Grobj_Init(Tcl_Interp *interp, 
								 string theObjName, Tcl_ObjCmdProc theObjCmd) {
  ObjList *olist = ObjlistTcl::getObjList();

  Tcl_CreateObjCommand(interp, const_cast<char *>(theObjName.c_str()), theObjCmd,
							  (ClientData) olist, (Tcl_CmdDeleteProc *) NULL);
  return TCL_OK;
}

#endif // !GROBJTCL_CC_DEFINED
