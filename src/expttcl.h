///////////////////////////////////////////////////////////////////////
// expttcl.h
// Rob Peters
// created: Mon Mar  8 03:18:35 1999
// written: Mon Apr 19 18:45:15 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef EXPTTCL_H_DEFINED
#define EXPTTCL_H_DEFINED

struct Tcl_Interp;
typedef int (Tcl_PackageInitProc) (Tcl_Interp *interp);

class Expt;

namespace ExptTcl {
  Expt& getExpt();

  Tcl_PackageInitProc Expt_Init;
}

static const char vcid_expttcl_h[] = "$Header$";
#endif // !EXPTTCL_H_DEFINED
