///////////////////////////////////////////////////////////////////////
// scaleabletcl.h
// Rob Peters
// created: Thu Mar  4 21:28:39 1999
// written: Fri Mar 12 11:25:44 1999
static const char vcid[] = "$Id$";
///////////////////////////////////////////////////////////////////////

#ifndef SCALEABLETCL_H_DEFINED
#define SCALEABLETCL_H_DEFINED

struct Tcl_Interp;
typedef int (Tcl_PackageInitProc) (Tcl_Interp *interp);

namespace ScaleableTcl {
  Tcl_PackageInitProc Scaleable_Init;
}

#endif // !SCALEABLETCL_H_DEFINED
