///////////////////////////////////////////////////////////////////////
// expttcl.h
// Rob Peters
// created: Mon Mar  8 03:18:35 1999
// written: Tue Mar 16 19:45:34 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef EXPTTCL_H_DEFINED
#define EXPTTCL_H_DEFINED

struct Tcl_Interp;
typedef int (Tcl_PackageInitProc) (Tcl_Interp *interp);

namespace ExptTcl {
  Tcl_PackageInitProc Expt_Init;
}

static const char vcid_expttcl_h[] = "$Header$";
#endif // !EXPTTCL_H_DEFINED
