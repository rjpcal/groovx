///////////////////////////////////////////////////////////////////////
// misctcl.h
// Rob Peters
// created: Nov-98
// written: Mon Mar 15 15:54:05 1999
// $Id$
//
// this package provides miscellaneous helper procedures for Tcl
// including rand(), sleep(), and usleep()
///////////////////////////////////////////////////////////////////////

#ifndef MISCTCL_H_DEFINED
#define MISCTCL_H_DEFINED

struct Tcl_Interp;
typedef int (Tcl_PackageInitProc) (Tcl_Interp *interp);

namespace MiscTcl {
  Tcl_PackageInitProc Misc_Init;
}

static const char vcid_misctcl_h[] = "$Header$";
#endif // !MISCTCL_H_DEFINED
