///////////////////////////////////////////////////////////////////////
//
// poslisttcl.h
// Rob Peters
// created: Sat Mar 13 12:43:01 1999
// written: Mon Jun 21 11:42:07 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef POSLISTTCL_H_DEFINED
#define POSLISTTCL_H_DEFINED

namespace PoslistTcl {
  const char* const bad_posid_msg = "posid out of range";
}

struct Tcl_Interp;
typedef int (Tcl_PackageInitProc) (Tcl_Interp* interp);

extern "C" Tcl_PackageInitProc Poslist_Init;

static const char vcid_poslisttcl_h[] = "$Header$";
#endif // !POSLISTTCL_H_DEFINED
