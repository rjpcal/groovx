///////////////////////////////////////////////////////////////////////
// objlisttcl.h
// Rob Peters
// created: Jan-99
// written: Mon Apr 19 17:05:35 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef OBJLISTTCL_H_DEFINED
#define OBJLISTTCL_H_DEFINED

///////////////////////////////////////////////////////////////////////
// ObjList Tcl package declarations
///////////////////////////////////////////////////////////////////////

struct Tcl_Interp;
typedef int (Tcl_PackageInitProc) (Tcl_Interp *interp);
struct Tcl_Obj;
class ObjList;
class GrObj;

namespace ObjlistTcl {
  typedef int (ObjlistTcl_SubcmdProc) 
    (const ObjList& olist, Tcl_Interp *interp,
     int objc, Tcl_Obj *const objv[]);

  ObjList& getObjList();
  GrObj* getObjFromArg(Tcl_Interp *interp, Tcl_Obj *const objv[], 
                       const ObjList& olist, int argn);

  const char* const bad_objid_msg = "objid out of range";
  const char* const wrong_type_msg = "object not of correct type";

  Tcl_PackageInitProc Objlist_Init;
}

static const char vcid_objlisttcl_h[] = "$Header$";
#endif // !OBJLISTTCL_H_DEFINED
