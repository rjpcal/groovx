///////////////////////////////////////////////////////////////////////
// facetcl.h
// Rob Peters
// created: Jan-99
// written: Fri Mar 12 11:33:20 1999
static const char vcid[] = "$Id$";
///////////////////////////////////////////////////////////////////////

#ifndef FACETCL_H_DEFINED
#define FACETCL_H_DEFINED

struct Tcl_Interp;
typedef int (Tcl_PackageInitProc) (Tcl_Interp *interp);

namespace FaceTcl {
  Tcl_PackageInitProc Face_Init;
}

#endif // !FACETCL_H_DEFINED
