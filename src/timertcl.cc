///////////////////////////////////////////////////////////////////////
// timertcl.cc
// Rob Peters
// created: Wed Feb 10 19:45:00 1999
// written: Fri Mar 12 11:21:33 1999
static const char vcid[] = "$Id$";
///////////////////////////////////////////////////////////////////////

#ifndef TIMERTCL_CC_DEFINED
#define TIMERTCL_CC_DEFINED

#include "timertcl.h"

#include <string>

#include "grobjtcl.h"
#include "objlisttcl.h"
#include "errmsg.h"
#include "timer.h"

namespace TimerTcl {
  using namespace GrobjTcl;

  ProcMap theProcMap;			  // maps strings to subcommand procedures
  string theObjName;				  // the string to invoke main command

  GrObj* newTimer();				  // returns a new timer object
  Tcl_ObjCmdProc timerCmd;		  // main command; delegates to subcommands
  GrobjTcl_SubProc timeCmd;	  // subcommand to set time of timer

  int Timer_Init(Tcl_Interp *interp);  
};

GrObj* TimerTcl::newTimer() { return new Timer; }

int TimerTcl::timerCmd(ClientData clientData, Tcl_Interp *interp,
							  int objc, Tcl_Obj *const objv[]) {
  return grobjCmd(clientData, interp, objc, objv, &newTimer, theProcMap);
}

int TimerTcl::Timer_Init(Tcl_Interp *interp) { 
  theProcMap["time"] = timeCmd;

  theObjName = "timer";
  return Grobj_Init(interp, theObjName, timerCmd); 
}

int TimerTcl::timeCmd(ObjList *olist, Tcl_Interp *interp,
								int objc, Tcl_Obj *const objv[]) {
  Timer *p = NULL;
  GrObj *g = NULL;
  int msec;

  if (objc != 4) {
	 Tcl_WrongNumArgs(interp, 2, objv, "objid num_msec");
	 return TCL_ERROR;
  }

  // Get the object
  if ( (g = ObjlistTcl::getObjFromArg(interp, objv, olist, 2)) == NULL )
	 return TCL_ERROR;

  // Make sure it's a timer object
  p = dynamic_cast<Timer *>(g);

  if ( p == NULL ) {				  // dynamic cast failed
	 err_message(interp, objv, ObjlistTcl::wrong_type_msg);
	 return TCL_ERROR;
  }

  if ( Tcl_GetIntFromObj(interp, objv[3], &msec) != TCL_OK ) return TCL_ERROR;

  p->setTime(msec);
  return TCL_OK;
}

#endif // !TIMERTCL_CC_DEFINED
