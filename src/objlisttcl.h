///////////////////////////////////////////////////////////////////////
// objlisttcl.h
// Rob Peters
// created: Jan-99
// written: Fri Mar 12 12:55:43 1999
static const char vcid_objlisttcl_h[] = "$Id$";
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
	 (ObjList *olist, Tcl_Interp *interp,
	  int objc, Tcl_Obj *const objv[]);

  extern ObjList *theObjList;
  inline ObjList* getObjList() { return theObjList; }
  inline ObjList** getObjListHandle() { return &theObjList; }
  GrObj* getObjFromArg(Tcl_Interp *interp, Tcl_Obj *const objv[], 
                       ObjList *olist, int argn);

  const char* const cant_make_obj = ": error creating object";
  const char* const bad_objid_msg = ": objid out of range";
  const char* const wrong_type_msg = ": object not of correct type";

  Tcl_PackageInitProc Objlist_Init;
}

#endif // !OBJLISTTCL_H_DEFINED
