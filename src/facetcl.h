///////////////////////////////////////////////////////////////////////
// facetcl.h
// Rob Peters
// created: Jan-99
// written: Sat Mar 13 12:52:37 1999
///////////////////////////////////////////////////////////////////////

#ifndef FACETCL_H_DEFINED
#define FACETCL_H_DEFINED

struct Tcl_Interp;
typedef int (Tcl_PackageInitProc) (Tcl_Interp *interp);

namespace FaceTcl {
  Tcl_PackageInitProc Face_Init;
}

static const char vcid_facetcl_h[] = "$Id$";
#endif // !FACETCL_H_DEFINED
