///////////////////////////////////////////////////////////////////////
//
// objlisttcl.cc
// Rob Peters
// created: Jan-99
// written: Mon Jun 14 14:24:38 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef OBJLISTTCL_CC_DEFINED
#define OBJLISTTCL_CC_DEFINED

#include "objlisttcl.h"

#include <tcl.h>
#include <string>
#include <strstream.h>
#include <typeinfo>
#include <vector>

#include "errmsg.h"
#include "objlist.h"
#include "grobj.h"
#include "id.h"
#include "tcllistpkg.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

///////////////////////////////////////////////////////////////////////
//
// Objlist Tcl package
//
///////////////////////////////////////////////////////////////////////

namespace ObjlistTcl {
  Tcl_ObjCmdProc allObjsCmd;
  Tcl_ObjCmdProc objTypeCmd;
}

///////////////////////////////////////////////////////////////////////
//
// Objlist Tcl package definitions
//
///////////////////////////////////////////////////////////////////////

GrObj* ObjlistTcl::getObjFromArg(Tcl_Interp* interp, Tcl_Obj* const objv[], 
                                 int argn) {
DOTRACE("ObjlistTcl::getObjFromArg");
// XXX remove ObjList argument
  int id;
  if ( Tcl_GetIntFromObj(interp, objv[argn], &id) != TCL_OK ) return NULL;

  ObjId objid(id);

  DebugEvalNL(objid);

  // Make sure we have a valid objid ...
  if ( !objid ) {
    err_message(interp, objv, bad_objid_msg);
    return NULL;
  }

  // ... and get the object to which it refers
  return objid.get();
}

int ObjlistTcl::allObjsCmd(ClientData, Tcl_Interp* interp,
									int objc, Tcl_Obj* const objv[]) {
DOTRACE("ObjlistTcl::allObjsCmd");
  if (objc != 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  ObjList& olist = ObjList::theObjList();
  vector<int> objids;
  olist.getValidObjids(objids);

  Tcl_Obj* list_out = Tcl_NewListObj(0, (Tcl_Obj **) NULL);

  for (int i = 0; i < objids.size(); ++i) {
	 Tcl_Obj* id_obj = Tcl_NewIntObj(objids[i]);
	 if (Tcl_ListObjAppendElement(interp, list_out, id_obj) != TCL_OK)
		return TCL_ERROR;
  }
  Tcl_SetObjResult(interp, list_out);
  return TCL_OK;
}

// return an object's name
int ObjlistTcl::objTypeCmd(ClientData, Tcl_Interp* interp,
                           int objc, Tcl_Obj* const objv[]) {
DOTRACE("ObjlistTcl::objTypeCmd");
  if (objc != 2)  {
    Tcl_WrongNumArgs(interp, 1, objv, "objid");
    return TCL_ERROR;
  }

  GrObj* g = getObjFromArg(interp, objv, 1);
  if ( g == NULL ) return TCL_ERROR;

  const char* name = typeid(*g).name();
  if ( name != NULL ) {
    Tcl_SetObjResult(interp, Tcl_NewStringObj(name,-1));
  }
  return TCL_OK;
}

///////////////////////////////////////////////////////////////////////
//
// ObjListPkg class definition
//
///////////////////////////////////////////////////////////////////////

class ObjListPkg : public TclListPkg {
public:
  ObjListPkg(Tcl_Interp* interp) :
	 TclListPkg(interp, "ObjList", "2.5")
  {
	 Tcl_CreateObjCommand(interp, "ObjList::objType", ObjlistTcl::objTypeCmd,
								 (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
	 Tcl_CreateObjCommand(interp, "ObjList::allObjs", ObjlistTcl::allObjsCmd,
								 (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);

	 Tcl_Eval(interp, 
				 "namespace eval ObjList {\n"
				 "  proc resetObjList {} { ObjList::reset }\n"
				 "  proc objCount {} { return [ObjList::count] }\n"
				 "  namespace export objType\n"
				 "  namespace export resetObjList\n"
				 "  namespace export objCount\n"
				 "}");
	 Tcl_Eval(interp,
				 "namespace import ObjList::objType\n"
				 "namespace import ObjList::resetObjList\n"
				 "namespace import ObjList::objCount\n");
  }

  virtual IO& getList() { return ObjList::theObjList(); }
  virtual int getBufSize() { 
	 //   20 chars for first line with ObjList ...
	 //   40 chars for each GrObj in list
	 //   10 chars for last line
	 return (20 + 40*ObjList::theObjList().objCount() + 10);
  }

  virtual void reset() { ObjList::theObjList().clearObjs(); }
  virtual int count() { return ObjList::theObjList().objCount(); }
};

//---------------------------------------------------------------------
//
// ObjlistTcl::Objlist_Init --
//
//---------------------------------------------------------------------

int ObjlistTcl::Objlist_Init(Tcl_Interp* interp) {
DOTRACE("ObjlistTcl::Objlist_Init");

  new ObjListPkg(interp);

  return TCL_OK;
}

static const char vcid_objlisttcl_cc[] = "$Header$";
#endif // !OBJLISTTCL_CC_DEFINED
