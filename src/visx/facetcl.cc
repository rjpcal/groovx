///////////////////////////////////////////////////////////////////////
// facetcl.cc
// Rob Peters 
// created: Jan-99
// written: Sat Mar 13 13:40:27 1999
static const char vcid_facetcl_cc[] = "$Id$";
///////////////////////////////////////////////////////////////////////

#ifndef FACETCL_CC_DEFINED
#define FACETCL_CC_DEFINED

#include "facetcl.h"

#include <tcl.h>

#include "errmsg.h"
#include "objlist.h"
#include "objlisttcl.h"
#include "face.h"

#define NO_TRACE
#include "trace.h"
#include "debug.h"

///////////////////////////////////////////////////////////////////////
// Tcl package for Face's
///////////////////////////////////////////////////////////////////////

namespace FaceTcl {
  Face* getFaceFromArg(Tcl_Interp *interp, Tcl_Obj *const objv[],
							  ObjList *olist, int argn);

  Tcl_ObjCmdProc faceCmd;
  Tcl_ObjCmdProc set_noseCmd;
  Tcl_ObjCmdProc set_mouthCmd;
  Tcl_ObjCmdProc set_eyeheightCmd;
  Tcl_ObjCmdProc set_eyedistCmd;

  // these constants are passed to err_message() in case of error
  using ObjlistTcl::cant_make_obj;
  using ObjlistTcl::wrong_type_msg;
}

Face* FaceTcl::getFaceFromArg(Tcl_Interp *interp, Tcl_Obj *const objv[],
										ObjList *olist, int argn) {
DOTRACE("FaceTcl::getFaceFromArg");
  // Get the object
  GrObj *g = ObjlistTcl::getObjFromArg(interp, objv, olist, argn);
  
  // Make sure it's a face object
  Face *p = dynamic_cast<Face *>(g);

  if ( p == NULL ) {
	 err_message(interp, objv, wrong_type_msg);
  }
  return p;
}

int FaceTcl::faceCmd(ClientData, Tcl_Interp *interp,
                     int objc, Tcl_Obj *const objv[]) {
DOTRACE("faceCmd");
  if (objc > 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }
  
  float eh=0.6, es=0.4, nl=0.4, mh=-0.8;
  Face *p = new Face(eh, es, nl, mh);
  
  ObjList *olist = ObjlistTcl::getObjList();
  int id = olist->addObj(p);
  if (id < 0) {
    err_message(interp, objv, cant_make_obj);
    delete p;
    return TCL_ERROR;
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(id));
  return TCL_OK;
}

int FaceTcl::set_noseCmd(ClientData, Tcl_Interp *interp,
                           int objc, Tcl_Obj *const objv[]) {  
DOTRACE("set_noseCmd");
  if (objc < 3) {
    Tcl_WrongNumArgs(interp, 1, objv, "faceid length");
    return TCL_ERROR;
  }

  // Get the face
  ObjList *olist = ObjlistTcl::getObjList();
  Face *p = getFaceFromArg(interp, objv, olist, 1);
  if ( p == NULL ) return TCL_ERROR;

  double nl;
  if (Tcl_GetDoubleFromObj(interp, objv[2], &nl) != TCL_OK) return TCL_ERROR;

  p->setNoseLen(nl);

  return TCL_OK;
}

int FaceTcl::set_mouthCmd(ClientData, Tcl_Interp *interp,
                            int objc, Tcl_Obj *const objv[]) {  
DOTRACE("set_mouthCmd");
  if (objc < 3) {
    Tcl_WrongNumArgs(interp, 1, objv, "faceid height");
    return TCL_ERROR;
  }

  // Get the face
  ObjList *olist = ObjlistTcl::getObjList();
  Face *p = getFaceFromArg(interp, objv, olist, 1);
  if ( p == NULL ) return TCL_ERROR;

  double mh;
  if (Tcl_GetDoubleFromObj(interp, objv[2], &mh) != TCL_OK) return TCL_ERROR;

  p->setMouthHgt(mh);

  return TCL_OK;
}

int FaceTcl::set_eyeheightCmd(ClientData, Tcl_Interp *interp,
                          int objc, Tcl_Obj *const objv[]) {  
DOTRACE("set_eyeheightCmd");
    if (objc < 3) {
    Tcl_WrongNumArgs(interp, 1, objv, "faceid height");
    return TCL_ERROR;
  }
  
  // Get the face
  ObjList *olist = ObjlistTcl::getObjList();
  Face *p = getFaceFromArg(interp, objv, olist, 1);
  if ( p == NULL ) return TCL_ERROR;

  double eh;
  if (Tcl_GetDoubleFromObj(interp, objv[2], &eh) != TCL_OK) return TCL_ERROR;

  p->setEyeHgt(eh);

  return TCL_OK;
}

int FaceTcl::set_eyedistCmd(ClientData, Tcl_Interp *interp,
                        int objc, Tcl_Obj *const objv[]) {  
DOTRACE("set_eyedistCmd");
  if (objc < 3) {
    Tcl_WrongNumArgs(interp, 1, objv, "faceid distance");
    return TCL_ERROR;
  }
  
  // Get the face
  ObjList *olist = ObjlistTcl::getObjList();
  Face *p = getFaceFromArg(interp, objv, olist, 1);
  if ( p == NULL ) return TCL_ERROR;

  double ed;
  if (Tcl_GetDoubleFromObj(interp, objv[2], &ed) != TCL_OK) return TCL_ERROR;

  p->setEyeDist(ed);

  return TCL_OK;
}

int FaceTcl::Face_Init(Tcl_Interp *interp) {
DOTRACE("FaceTcl::Face_Init");
  Tcl_CreateObjCommand(interp, "face", faceCmd,
							  (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "set_nose", set_noseCmd,
							  (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "set_mouth", set_mouthCmd,
							  (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "set_eyeheight", set_eyeheightCmd,
							  (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "set_eyedist", set_eyedistCmd,
							  (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  return TCL_OK;
}

#endif // !FACETCL_CC_DEFINED
