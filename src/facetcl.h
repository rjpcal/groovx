///////////////////////////////////////////////////////////////////////
// facetcl.h
// Rob Peters Jan-99
///////////////////////////////////////////////////////////////////////

#ifndef FACETCL_H_DEFINED
#define FACETCL_H_DEFINED

struct Tcl_Interp;
typedef int (Tcl_PackageInitProc) (Tcl_Interp *interp);

namespace FaceTcl {
  Tcl_PackageInitProc Face_Init;
}

#endif // !FACETCL_H_DEFINED
