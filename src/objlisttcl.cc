///////////////////////////////////////////////////////////////////////
//
// objlisttcl.cc
// Rob Peters
// created: Jan-99
// written: Mon Jun 14 14:02:31 1999
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

#define USE_OLD 0

///////////////////////////////////////////////////////////////////////
//
// Objlist Tcl package
//
///////////////////////////////////////////////////////////////////////

namespace ObjlistTcl {
  Tcl_ObjCmdProc allObjsCmd;
  Tcl_ObjCmdProc objTypeCmd;

#if USE_OLD
  Tcl_ObjCmdProc resetObjListCmd;
  Tcl_ObjCmdProc objCountCmd;
  Tcl_ObjCmdProc stringifyCmd;
  Tcl_ObjCmdProc destringifyCmd;
#endif
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

#if USE_OLD

// clear the ObjList of all GrObj's by calling ObjList::clearObjs()
int ObjlistTcl::resetObjListCmd(ClientData, Tcl_Interp* interp,
                                int objc, Tcl_Obj* const objv[]) {
DOTRACE("ObjlistTcl::resetObjListCmd");
  if (objc > 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  ObjList& olist = ObjList::theObjList();
  olist.clearObjs();

  return TCL_OK;
}

// return the number of objects in theObjList
int ObjlistTcl::objCountCmd(ClientData, Tcl_Interp* interp,
                           int objc, Tcl_Obj* const objv[]) {
DOTRACE("ObjlistTcl::objCountCmd");
  if (objc > 1) {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  const ObjList& olist = ObjList::theObjList();
  Tcl_SetObjResult(interp, Tcl_NewIntObj(olist.objCount()));
  return TCL_OK;
}

#endif

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

#if USE_OLD

int ObjlistTcl::stringifyCmd(ClientData, Tcl_Interp* interp,
                                     int objc, Tcl_Obj* const objv[]) {
DOTRACE("ObjlistTcl::stringifyCmd");
  if (objc != 1)  {
    Tcl_WrongNumArgs(interp, 1, objv, NULL);
    return TCL_ERROR;
  }

  const ObjList& olist = ObjList::theObjList();

  // Try to guess (conservatively) how much space we'll need to avoid
  // overflows when possible
  //
  //   20 chars for first line with ObjList ...
  //   40 chars for each object in list
  //   10 chars for last line
  int BUF_SIZE = 20 + 40*olist.objCount() + 10;

  auto_ptr<char> buf(new char[BUF_SIZE]);
  ostrstream ost(buf.get(), BUF_SIZE);

  try {
	 olist.serialize(ost, IO::BASES|IO::TYPENAME);
	 ost << '\0';
  }
  catch (IoError& err) {
	 string msg = ": " + err.info();
	 err_message(interp, objv, msg.c_str());
	 return TCL_ERROR;
  }	 
  Tcl_SetObjResult(interp, Tcl_NewStringObj(buf.get(), -1));
  return TCL_OK;
}

int ObjlistTcl::destringifyCmd(ClientData, Tcl_Interp* interp,
                                     int objc, Tcl_Obj* const objv[]) {
DOTRACE("ObjlistTcl::destringifyCmd");
  if (objc != 2)  {
    Tcl_WrongNumArgs(interp, 1, objv, "string");
    return TCL_ERROR;
  }

  ObjList& olist = ObjList::theObjList();

  const char* buf = Tcl_GetString(objv[1]);
  Assert(buf);

  istrstream ist(buf);

  try {
	 olist.deserialize(ist, IO::BASES|IO::TYPENAME);
  }
  catch (IoError& err) {
	 err_message(interp, objv, err.info().c_str());
	 return TCL_ERROR;
  }
  return TCL_OK;
}

#endif

///////////////////////////////////////////////////////////////////////
//
// ObjListPkg class definition
//
///////////////////////////////////////////////////////////////////////

class ObjListPkg : public TclListPkg {
public:
  ObjListPkg(Tcl_Interp* interp) :
	 TclListPkg(interp, "ObjList", "2.1")
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
	 // Try to guess (conservatively) how much space we'll need to avoid
	 // overflows when possible
	 //
	 //   20 chars for first line with ObjList ...
	 //   40 chars for each object in list
	 //   10 chars for last line
	 return (20 + 40*ObjList::theObjList().objCount() + 10);
  }

  virtual void reset() { ObjList::theObjList().clearObjs(); }
  virtual int count() { return ObjList::theObjList().objCount(); }

};

int ObjlistTcl::Objlist_Init(Tcl_Interp* interp) {
DOTRACE("ObjlistTcl::Objlist_Init");

  new ObjListPkg(interp);

#if USE_OLD
  // Add all commands to the ::ObjList namespace
  Tcl_CreateObjCommand(interp, "ObjList::resetObjList", resetObjListCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "ObjList::allObjs", allObjsCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "ObjList::objCount", objCountCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "ObjList::objType", objTypeCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "ObjList::stringify", stringifyCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateObjCommand(interp, "ObjList::destringify", destringifyCmd,
                       (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);

  Tcl_Eval(interp, 
			  "namespace eval ObjList {\n"
			  "  namespace export objType\n"
			  "  namespace export resetObjList\n"
			  "  namespace export objCount\n"
			  "}");
  Tcl_Eval(interp,
			  "namespace import ObjList::objType\n"
			  "namespace import ObjList::resetObjList\n"
			  "namespace import ObjList::objCount\n");

  Tcl_PkgProvide(interp, "Objlist", "2.1");
#endif
  return TCL_OK;
}

static const char vcid_objlisttcl_cc[] = "$Header$";
#endif // !OBJLISTTCL_CC_DEFINED
