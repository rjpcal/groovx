///////////////////////////////////////////////////////////////////////
// positiontcl.cc
// Rob Peters
// created: Sat Mar 13 12:53:34 1999
// written: Mon Mar 15 16:03:21 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef POSITIONTCL_CC_DEFINED
#define POSITIONTCL_CC_DEFINED

#include "positiontcl.h"

#include <tcl.h>
#include <strstream.h>

#include "errmsg.h"
#include "position.h"
#include "poslist.h"
#include "poslisttcl.h"

#define NO_TRACE
#include "trace.h"
#include "debug.h"

namespace PositionTcl {
  const int BUF_SIZE = 200;
  
  Tcl_ObjCmdProc positionCmd;
  Tcl_ObjCmdProc translateCmd;
  Tcl_ObjCmdProc scaleCmd;
  Tcl_ObjCmdProc rotateCmd;
  Tcl_ObjCmdProc stringify_positionCmd;
}

// create a new Position object, insert it into the PosList, and
// return its id#
int PositionTcl::positionCmd(ClientData, Tcl_Interp *interp,
									  int objc, Tcl_Obj *const objv[]) {
DOTRACE("PositionTcl::positionCmd");
  if (objc > 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }
  
  Position *p = new Position();
  
  PosList& plist = PoslistTcl::getPosList();
  int id = plist.addPos(p);
  if (id < 0) {
    err_message(interp, objv, PoslistTcl::cant_make_pos);
    delete p;
    return TCL_ERROR;
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(id));
  return TCL_OK;
}

// set the coordinates for translating an object before it is drawn
int PositionTcl::translateCmd(ClientData, Tcl_Interp *interp,
										  int objc, Tcl_Obj *const objv[]) {
DOTRACE("PositionTcl::translateCmd");
  if (objc > 5 || objc < 2)  {
    Tcl_WrongNumArgs(interp, 1, objv, "posid new_x new_y [new_z]");
    return TCL_ERROR;
  }

  const PosList& plist = PoslistTcl::getPosList();
  Position *p = PoslistTcl::getPosFromArg(interp, objv, plist, 1);
  if ( p == NULL ) return TCL_ERROR;

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

  p->setTranslate(new_x, new_y, new_z);
  return TCL_OK;
}

// set the parameters for scaling an object before it is drawn
int PositionTcl::scaleCmd(ClientData, Tcl_Interp *interp,
									 int objc, Tcl_Obj *const objv[]) {
DOTRACE("PositionTcl::scaleCmd");
  if (objc > 5 || objc < 2)  {
    Tcl_WrongNumArgs(interp, 1, objv, "posid scale_x [scale_y] [scale_z]");
    return TCL_ERROR;
  }

  const PosList& plist = PoslistTcl::getPosList();
  Position *p = PoslistTcl::getPosFromArg(interp, objv, plist, 1); 
  if ( p == NULL ) return TCL_ERROR;

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

  p->setScale(new_x, new_y, new_z);
  return TCL_OK;
}

// set the parameters for rotating an object before it is drawn
int PositionTcl::rotateCmd(ClientData, Tcl_Interp *interp,
										 int objc, Tcl_Obj *const objv[]) {
DOTRACE("PositionTcl::rotateCmd");
  if (objc != 3 && objc != 6)  {
    Tcl_WrongNumArgs(interp, 1, objv, 
							"posid angle [rot_axis_x rot_axis_y rot_axis_z]");
    return TCL_ERROR;
  }

  const PosList& plist = PoslistTcl::getPosList();
  Position *p = PoslistTcl::getPosFromArg(interp, objv, plist, 1); 
  if ( p == NULL ) return TCL_ERROR;

  double new_a=0.0;
  if (Tcl_GetDoubleFromObj(interp, objv[2], &new_a) != TCL_OK)
	 return TCL_ERROR;
  if (objc == 3) {
	 p->setAngle(new_a);
	 return TCL_OK;
  }

  double new_x=0.0, new_y=0.0, new_z=1.0;
  if (Tcl_GetDoubleFromObj(interp, objv[3], &new_x) != TCL_OK)
	 return TCL_ERROR;
  if (Tcl_GetDoubleFromObj(interp, objv[4], &new_y) != TCL_OK)
	 return TCL_ERROR;
  if (Tcl_GetDoubleFromObj(interp, objv[5], &new_z) != TCL_OK)
	 return TCL_ERROR;
  
  p->setRotate(new_a, new_x, new_y, new_z);
  return TCL_OK;
}

int PositionTcl::stringify_positionCmd(ClientData, Tcl_Interp *interp,
										 int objc, Tcl_Obj *const objv[]) {
DOTRACE("PositionTcl::stringify_positionCmd");
  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "posid");
    return TCL_ERROR;
  }
  
  const PosList& plist = PoslistTcl::getPosList();
  Position *p = PoslistTcl::getPosFromArg(interp, objv, plist, 1);
  if ( p == NULL ) return TCL_ERROR;

  char buf[BUF_SIZE];
  ostrstream ost(buf, BUF_SIZE);
  p->serialize(ost, IO::IOFlag(IO::TYPENAME|IO::BASES));

  Tcl_SetObjResult(interp, Tcl_NewStringObj(buf, -1));

  return TCL_OK;
}

int PositionTcl::Position_Init(Tcl_Interp *interp) {
DOTRACE("PositionTcl::Position_Init");
  Tcl_CreateObjCommand(interp, "position", positionCmd,
							  (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "translate", translateCmd,
							  (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "scale", scaleCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "rotate", rotateCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "stringify_position", stringify_positionCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_PkgProvide(interp, "Position", "2.1");
  return TCL_OK;
}

static const char vcid_positiontcl_cc[] = "$Header$";
#endif // !POSITIONTCL_CC_DEFINED
