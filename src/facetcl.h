///////////////////////////////////////////////////////////////////////
// facetcl.h
// Rob Peters
// created: Jan-99
// written: Tue Mar 16 19:44:07 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef FACETCL_H_DEFINED
#define FACETCL_H_DEFINED

struct Tcl_Interp;
typedef int (Tcl_PackageInitProc) (Tcl_Interp *interp);

namespace FaceTcl {
  Tcl_PackageInitProc Face_Init;
}

static const char vcid_facetcl_h[] = "$Header$";
#endif // !FACETCL_H_DEFINED
