///////////////////////////////////////////////////////////////////////
// glisttcl.h
// Rob Peters
// created: Jan-99
// written: Sat Mar 13 12:36:50 1999
///////////////////////////////////////////////////////////////////////

#ifndef GLISTTCL_H_DEFINED
#define GLISTTCL_H_DEFINED

///////////////////////////////////////////////////////////////////////
// Glist Tcl package declarations
///////////////////////////////////////////////////////////////////////

struct Tcl_Interp;
typedef int (Tcl_PackageInitProc) (Tcl_Interp *interp);
class Glist;

namespace GlistTcl {
  extern Glist* theGlist;
  inline Glist** getGlistHandle() { return &theGlist; }
  Tcl_PackageInitProc Glist_Init;
}

static const char vcid_glisttcl_h[] = "$Id$";
#endif // !GLISTTCL_H_DEFINED
