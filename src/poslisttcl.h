///////////////////////////////////////////////////////////////////////
// poslisttcl.h
// Rob Peters
// created: Sat Mar 13 12:43:01 1999
// written: Tue Mar 16 19:33:39 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef POSLISTTCL_H_DEFINED
#define POSLISTTCL_H_DEFINED

///////////////////////////////////////////////////////////////////////
// PosList Tcl package declarations
///////////////////////////////////////////////////////////////////////

struct Tcl_Interp;
typedef int (Tcl_PackageInitProc) (Tcl_Interp *interp);
struct Tcl_Obj;
class PosList;
class Position;

namespace PoslistTcl {
  typedef int (PoslistTcl_SubcmdProc) 
    (PosList *plist, Tcl_Interp *interp,
     int objc, Tcl_Obj *const objv[]);

  PosList& getPosList();
  Position* getPosFromArg(Tcl_Interp *interp, Tcl_Obj *const objv[], 
                          const PosList& plist, int argn);

  const char* const cant_make_pos = ": error creating position";
  const char* const bad_posid_msg = ": posid out of range";

  Tcl_PackageInitProc Poslist_Init;
}

static const char vcid_poslisttcl_h[] = "$Header$";
#endif // !POSLISTTCL_H_DEFINED
