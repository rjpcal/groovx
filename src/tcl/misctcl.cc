///////////////////////////////////////////////////////////////////////
//
// misctcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Nov-98
// written: Fri Jun  1 17:36:26 2001
// $Id$
//
// this file contains the implementations for some simple Tcl functions
// that are basically wrappers for standard C library functions, such
// as rand(), sleep(), and usleep()
//
///////////////////////////////////////////////////////////////////////

#ifndef MISCTCL_C_DEFINED
#define MISCTCL_C_DEFINED

#include "util/rand.h"
#include "util/randutils.h"

#include <tcl.h>
#include <cstdlib>
#include <unistd.h>

///////////////////////////////////////////////////////////////////////
// MiscTcl Tcl package declarations
///////////////////////////////////////////////////////////////////////

namespace MiscTcl {
  Tcl_ObjCmdProc randCmd;
  Tcl_ObjCmdProc srandCmd;
  Tcl_ObjCmdProc sleepCmd;
  Tcl_ObjCmdProc usleepCmd;
  Tcl_ObjCmdProc usleeprCmd;

  Util::Randint generator;
}

///////////////////////////////////////////////////////////////////////
// MiscTcl Tcl package definitions
///////////////////////////////////////////////////////////////////////

int MiscTcl::randCmd(ClientData, Tcl_Interp* interp,
                     int objc, Tcl_Obj* const objv[]) {
  if (objc != 3) {
    Tcl_WrongNumArgs(interp, 1, objv, "min max");
    return TCL_ERROR;
  }

  double min, max;
  if (Tcl_GetDoubleFromObj(interp, objv[1], &min) != TCL_OK) return TCL_ERROR;
  if (Tcl_GetDoubleFromObj(interp, objv[2], &max) != TCL_OK) return TCL_ERROR;

  double result = min + generator.fdraw() * (max-min);
  Tcl_SetObjResult(interp, Tcl_NewDoubleObj(result));
  return TCL_OK;
}

int MiscTcl::srandCmd(ClientData, Tcl_Interp* interp,
                      int objc, Tcl_Obj* const objv[]) {
  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "seed");
    return TCL_ERROR;
  }

  int seed;
  if (Tcl_GetIntFromObj(interp, objv[1], &seed) != TCL_OK) return TCL_ERROR;

  generator.seed(seed);
  return TCL_OK;
}

// use the standard library sleep() to sleep a specified # of seconds
//
// performance: performance is pretty good, considering that we're on
// a seconds timescale with this command. It seems to use an extra
// 9msec more than the specified delay
int MiscTcl::sleepCmd(ClientData, Tcl_Interp* interp,
                      int objc, Tcl_Obj* const objv[]) {
  if (objc != 2) return TCL_ERROR;
  int secs=0;
  Tcl_GetIntFromObj(interp, objv[1], &secs);
  if (secs < 0) return TCL_ERROR;
  sleep(secs);
  return TCL_OK;
}

// use the standard library usleep() to sleep a specified # of microseconds 
// 
// performance: in a real Tcl script, this command chews up an
// additional 9000usec more than the specified delay, unless the
// specified number is < 10000, in which case this command invariably
// takes ~19000 us (ugh)
int MiscTcl::usleepCmd(ClientData, Tcl_Interp* interp,
                       int objc, Tcl_Obj* const objv[]) {
  if (objc != 2) return TCL_ERROR;
  int usecs=0;
  Tcl_GetIntFromObj(interp, objv[1], &usecs);
  if (usecs < 0) return TCL_ERROR;
  usleep(usecs);
  return TCL_OK;  
}

// use the standard library usleep() to repeatedly sleep a specified #
// of microseconds
//
// performance: as with usleepCmd, there is some significant overhead
// here. It is typically an extra 10000usec per loop iteration, but
// again, as in usleepCmd, there seemse to be a minimum of ~20000usec
// per iteration, even if the specified delay is 1.
int MiscTcl::usleeprCmd(ClientData, Tcl_Interp* interp,
                        int objc, Tcl_Obj* const objv[]) {
  if (objc != 3) return TCL_ERROR;
  int usecs=0, reps=0;
  Tcl_GetIntFromObj(interp, objv[1], &usecs);
  Tcl_GetIntFromObj(interp, objv[2], &reps);
  if (usecs < 0 || reps < 0) return TCL_ERROR;
  for ( ; reps > 0; --reps) usleep(usecs);
  return TCL_OK;  
}

extern "C" Tcl_PackageInitProc Misc_Init;

int Misc_Init(Tcl_Interp* interp) {
  using namespace MiscTcl;

  Tcl_CreateObjCommand(interp, "rand", randCmd,
                    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "srand", srandCmd,
                    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "sleep", sleepCmd,
                    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "usleep", usleepCmd,
                    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "usleepr", usleeprCmd,
                    (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_PkgProvide(interp, "Misc", "1.6");
  return TCL_OK;
}

static const char vcid_misctcl_cc[] = "$Header$";
#endif // !MISCTCL_C_DEFINED
