///////////////////////////////////////////////////////////////////////
// tlisttcl.h
// Rob Peters
// created: Sat Mar 13 12:37:22 1999
// written: Sat Mar 13 13:23:59 1999
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
  extern Tlist* theTlist;
  inline Tlist* getTlist() { return theTlist; }
  inline Tlist** getTlistHandle() { return &theTlist; }
  Tcl_PackageInitProc Tlist_Init;
}

static const char vcid_tlisttcl_h[] = "$Id$";
#endif // !TLISTTCL_H_DEFINED
