///////////////////////////////////////////////////////////////////////
//
// tlisttcl.h
// Rob Peters
// created: Sat Mar 13 12:37:22 1999
// written: Thu May 27 13:49:50 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TLISTTCL_H_DEFINED
#define TLISTTCL_H_DEFINED

///////////////////////////////////////////////////////////////////////
//
// Tlist Tcl package declarations
//
///////////////////////////////////////////////////////////////////////

struct Tcl_Interp;
typedef int (Tcl_PackageInitProc) (Tcl_Interp* interp);
struct Tcl_Obj;
class Tlist;

namespace TlistTcl {
  Tlist& getTlist();

  // Exported procedures
  int write_responsesProc(Tcl_Interp* interp, Tcl_Obj* const objv[],
								  const char* filename);

  Tcl_PackageInitProc Tlist_Init;
}

static const char vcid_tlisttcl_h[] = "$Header$";
#endif // !TLISTTCL_H_DEFINED
