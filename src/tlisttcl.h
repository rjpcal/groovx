///////////////////////////////////////////////////////////////////////
// tlisttcl.h
// Rob Peters
// created: Sat Mar 13 12:37:22 1999
// written: Tue Mar 16 19:22:53 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef TLISTTCL_H_DEFINED
#define TLISTTCL_H_DEFINED

///////////////////////////////////////////////////////////////////////
// Tlist Tcl package declarations
///////////////////////////////////////////////////////////////////////

struct Tcl_Interp;
typedef int (Tcl_PackageInitProc) (Tcl_Interp *interp);
class Tlist;

namespace TlistTcl {
  Tlist& getTlist();
  Tcl_PackageInitProc Tlist_Init;
}

static const char vcid_tlisttcl_h[] = "$Header$";
#endif // !TLISTTCL_H_DEFINED
