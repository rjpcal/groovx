///////////////////////////////////////////////////////////////////////
// glisttcl.h
// Rob Peters
// created: Jan-99
// written: Fri Mar 12 11:31:25 1999
static const char vcid[] = "$Id$";
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

#endif // !GLISTTCL_H_DEFINED
