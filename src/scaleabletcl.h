///////////////////////////////////////////////////////////////////////
// scaleabletcl.h
// Rob Peters
// created: Thu Mar  4 21:28:39 1999
// written: Thu Mar  4 21:29:09 1999
///////////////////////////////////////////////////////////////////////

#ifndef SCALEABLETCL_H_DEFINED
#define SCALEABLETCL_H_DEFINED

struct Tcl_Interp;
typedef int (Tcl_PackageInitProc) (Tcl_Interp *interp);

namespace ScaleableTcl {
  Tcl_PackageInitProc Scaleable_Init;
}

#endif // !SCALEABLETCL_H_DEFINED
