///////////////////////////////////////////////////////////////////////
// grobjtcl.h
// Rob Peters
// created: Wed Feb 10 19:30:37 1999
// written: Mon Mar  8 17:13:13 1999
///////////////////////////////////////////////////////////////////////

#ifndef GROBJTCL_H_DEFINED
#define GROBJTCL_H_DEFINED

#include <tcl.h>
#include <string>
#include <map>

class GrObj;
class ObjList;

namespace GrobjTcl {
  typedef int (GrobjTcl_SubProc) (ObjList *olist, Tcl_Interp *interp,
											 int objc, Tcl_Obj *const objv[]);

  typedef GrObj * (newGrobjProc) ();

  typedef map<string, GrobjTcl_SubProc *> ProcMap;

  int grobjCmd(ClientData clientData, Tcl_Interp *interp,
					int objc, Tcl_Obj *const objv[], 
					newGrobjProc theGrobjProc, ProcMap &theProcMap);
  int Grobj_Init(Tcl_Interp *interp, 
					  string theObjName, Tcl_ObjCmdProc theObjCmd);

  const char* const bad_command = ": invalid subcommand";
};

#endif // !GROBJTCL_H_DEFINED
