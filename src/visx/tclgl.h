///////////////////////////////////////////////////////////////////////
// tclgl.h
// Rob Peters
// created: Nov-98
// written: Fri Mar 12 12:53:43 1999
static const char vcid_tclgl_h[] = "$Id$";
//
// This package provides some simple Tcl functions that are wrappers
// for C OpenGL functions. The function names, argument lists, and
// symbolic constants for the Tcl functions are identical to those in
// the analagous C functions.
///////////////////////////////////////////////////////////////////////

#ifndef TCLGL_H_DEFINED
#define TCLGL_H_DEFINED

struct Tcl_Interp;
typedef int (Tcl_PackageInitProc) (Tcl_Interp *interp);

namespace TclGL {
  // initializes the Tclgl package when called from Tcl_AppInit
  Tcl_PackageInitProc Tclgl_Init;
}

#endif // !TCLGL_H_DEFINED
