///////////////////////////////////////////////////////////////////////
// expttcl.h
// Rob Peters
// created: Mon Mar  8 03:18:35 1999
// written: Fri Mar 12 11:17:54 1999
static const char vcid[] = "$Id$";
///////////////////////////////////////////////////////////////////////

#ifndef EXPTTCL_H_DEFINED
#define EXPTTCL_H_DEFINED

struct Tcl_Interp;
typedef int (Tcl_PackageInitProc) (Tcl_Interp *interp);

namespace ExptTcl {
  Tcl_PackageInitProc Expt_Init;
}

#endif // !EXPTTCL_H_DEFINED
