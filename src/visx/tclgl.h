///////////////////////////////////////////////////////////////////////
// tclgl.h
// Rob Peters
// created: Nov-98
// written: Tue Mar 16 19:26:21 1999
// $Id$
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

static const char vcid_tclgl_h[] = "$Header$";
#endif // !TCLGL_H_DEFINED
