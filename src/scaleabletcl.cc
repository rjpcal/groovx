///////////////////////////////////////////////////////////////////////
// scaleabletcl.cc
// Rob Peters
// created: Thu Mar  4 21:29:15 1999
// written: Fri Mar 12 11:25:59 1999
static const char vcid[] = "$Id$";
///////////////////////////////////////////////////////////////////////

#ifndef SCALEABLETCL_CC_DEFINED
#define SCALEABLETCL_CC_DEFINED

#include "scaleabletcl.h"

#include <tcl.h>

#include "objlisttcl.h"
#include "scaleable.h"

#define NO_TRACE
#include "trace.h"
#include "debug.h"

namespace ScaleableTcl {
  Tcl_ObjCmdProc translateObjCmd;
  Tcl_ObjCmdProc scaleObjCmd;
  Tcl_ObjCmdProc rotateObjCmd;
}

// set the coordinates for translating an object before it is drawn
int ScaleableTcl::translateObjCmd(ClientData, Tcl_Interp *interp,
										  int objc, Tcl_Obj *const objv[]) {
DOTRACE("ScaleableTcl::translateObjCmd");
  if (objc > 5 || objc < 2)  {
    Tcl_WrongNumArgs(interp, 1, objv, "objid new_x new_y [new_z]");
    return TCL_ERROR;
  }

  ObjList *olist = ObjlistTcl::getObjList();
  GrObj *g = ObjlistTcl::getObjFromArg(interp, objv, olist, 1);
  Scaleable *s = dynamic_cast<Scaleable *>(g);
  if ( s == NULL ) return TCL_ERROR;

  double new_x=0.0, new_y=0.0, new_z=0.0;
  if (objc >= 5) {
	 if (Tcl_GetDoubleFromObj(interp, objv[4], &new_z) != TCL_OK) 
		return TCL_ERROR;
  }

  if (objc >= 4) {
	 if (Tcl_GetDoubleFromObj(interp, objv[3], &new_y) != TCL_OK)
		return TCL_ERROR;
  }

  if (objc >= 3) {
	 if (Tcl_GetDoubleFromObj(interp, objv[2], &new_x) != TCL_OK)
		return TCL_ERROR;
  }

  s->setTranslate(new_x, new_y, new_z);
  return TCL_OK;
}

// set the parameters for scaling an object before it is drawn
int ScaleableTcl::scaleObjCmd(ClientData, Tcl_Interp *interp,
									 int objc, Tcl_Obj *const objv[]) {
DOTRACE("ScaleableTcl::scaleObjCmd");
  if (objc > 5 || objc < 2)  {
    Tcl_WrongNumArgs(interp, 1, objv, "objid scale_x [scale_y] [scale_z]");
    return TCL_ERROR;
  }

  ObjList *olist = ObjlistTcl::getObjList();
  GrObj *g = ObjlistTcl::getObjFromArg(interp, objv, olist, 1); 
  Scaleable *s = dynamic_cast<Scaleable *>(g);
  if ( s == NULL ) return TCL_ERROR;

  double new_x=1.0, new_y=1.0, new_z=1.0;
  if (objc >= 3) {
	 if (Tcl_GetDoubleFromObj(interp, objv[2], &new_x) != TCL_OK)
		return TCL_ERROR;
  }

  if (objc >= 4) {
	 if (Tcl_GetDoubleFromObj(interp, objv[3], &new_y) != TCL_OK)
		return TCL_ERROR;
  }
  else new_y = new_x;

  if (objc >= 5) {
	 if (Tcl_GetDoubleFromObj(interp, objv[4], &new_z) != TCL_OK)
		return TCL_ERROR;
  }

  s->setScale(new_x, new_y, new_z);
  return TCL_OK;
}

// set the parameters for rotating an object before it is drawn
int ScaleableTcl::rotateObjCmd(ClientData, Tcl_Interp *interp,
										 int objc, Tcl_Obj *const objv[]) {
DOTRACE("ScaleableTcl::rotateObjCmd");
  if (objc != 3 && objc != 6)  {
    Tcl_WrongNumArgs(interp, 1, objv, 
							"objid angle [rot_axis_x rot_axis_y rot_axis_z]");
    return TCL_ERROR;
  }

  ObjList *olist = ObjlistTcl::getObjList();
  GrObj *g = ObjlistTcl::getObjFromArg(interp, objv, olist, 1); 
  Scaleable *s = dynamic_cast<Scaleable *>(g);
  if ( s == NULL ) return TCL_ERROR;

  double new_a=0.0;
  if (Tcl_GetDoubleFromObj(interp, objv[2], &new_a) != TCL_OK)
	 return TCL_ERROR;
  if (objc == 3) {
	 s->setAngle(new_a);
	 return TCL_OK;
  }

  double new_x=0.0, new_y=0.0, new_z=1.0;
  if (Tcl_GetDoubleFromObj(interp, objv[3], &new_x) != TCL_OK)
	 return TCL_ERROR;
  if (Tcl_GetDoubleFromObj(interp, objv[4], &new_y) != TCL_OK)
	 return TCL_ERROR;
  if (Tcl_GetDoubleFromObj(interp, objv[5], &new_z) != TCL_OK)
	 return TCL_ERROR;
  
  s->setRotate(new_a, new_x, new_y, new_z);
  return TCL_OK;
}

int ScaleableTcl::Scaleable_Init(Tcl_Interp *interp) {
DOTRACE("ScaleableTcl::Scaleable_Init");
  Tcl_CreateObjCommand(interp, "translateObj", translateObjCmd,
							  (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "scaleObj", scaleObjCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "rotateObj", rotateObjCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);

  return TCL_OK;
}

#endif // !SCALEABLETCL_CC_DEFINED
