///////////////////////////////////////////////////////////////////////
// misctcl.h
// Rob Peters
// created: Nov-98
// written: Fri Mar 12 12:56:03 1999
static const char vcid_misctcl_h[] = "$Id$";
//
// this package provides miscellaneous helper procedures for Tcl
// including rand(), sleep(), and usleep()
///////////////////////////////////////////////////////////////////////

#ifndef MISCTCL_H_DEFINED
#define MISCTCL_H_DEFINED

struct Tcl_Interp;
typedef int (Tcl_PackageInitProc) (Tcl_Interp *interp);

namespace MiscTcl {
  Tcl_PackageInitProc Misctcl_Init;
}

#endif // !MISCTCL_H_DEFINED
