///////////////////////////////////////////////////////////////////////
//
// objlisttcl.cc
// Rob Peters
// created: Jan-99
// written: Tue Jun 29 17:41:07 1999
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

#include "objlist.h"
#include "grobj.h"
#include "id.h"
#include "tclitempkg.h"
#include "listpkg.h"

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
  class AllObjsCmd;
  class ObjListPkg;
}

namespace GrobjTcl {
  class TypeCmd;
  class CategoryCmd;
  class GrObjPkg;
};

///////////////////////////////////////////////////////////////////////
//
// Objlist Tcl package definitions
//
///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------
//
// ObjlistTcl::AllObjsCmd --
//
//---------------------------------------------------------------------

class ObjlistTcl::AllObjsCmd : public TclCmd {
public:
  AllObjsCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name, NULL, 1, 1) {}
protected:
  virtual void invoke() {
	 vector<int> objids;
	 ObjList::theObjList().getValidObjids(objids);

	 returnSequence(objids.begin(), objids.end());
  }
};

//---------------------------------------------------------------------
//
// TypeCmd --
//
//---------------------------------------------------------------------

class GrobjTcl::TypeCmd : public TclItemCmd<GrObj> {
public:
  TypeCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<GrObj>(pkg, cmd_name, "objid", 2, 2) {}
protected:
  virtual void invoke() {
	 GrObj* p = getItem();
	 returnCstring(typeid(*p).name());
  }
};

///////////////////////////////////////////////////////////////////////
//
// ObjListPkg class definition
//
///////////////////////////////////////////////////////////////////////

class ObjlistTcl::ObjListPkg : public ListPkg<ObjList> {
public:
  ObjListPkg(Tcl_Interp* interp) :
	 ListPkg<ObjList>(interp, "ObjList", "3.0") {}

  virtual IO& getIoFromId(int) { 
	 return dynamic_cast<IO&>(ObjList::theObjList());
  }
  virtual ObjList* getCItemFromId(int) { return &(ObjList::theObjList()); }
};

///////////////////////////////////////////////////////////////////////
//
// GrObjPkg class definition
//
///////////////////////////////////////////////////////////////////////

class GrobjTcl::GrObjPkg : public CTclIoItemPkg<GrObj> {
public:
  GrObjPkg(Tcl_Interp* interp) :
	 CTclIoItemPkg<GrObj>(interp, "Grobj", "2.5")
  {
	 addCommand( new TypeCmd(this, "GrObj::type") );
	 declareAttrib("category", new CAttrib<GrObj, int>(&GrObj::getCategory,
																		&GrObj::setCategory));

	 Tcl_Eval(interp,
				 "proc objType {id} { return [GrObj::type $id] }\n");
  }

  virtual GrObj* getCItemFromId(int id) {
	 ObjId objid(id);
	 if ( !objid ) {
		throw TclError("objid out of range");
	 }
	 return objid.get();
  }

  virtual IO& getIoFromId(int id) {
	 return dynamic_cast<IO&>( *(getCItemFromId(id)) );
  }
};

//---------------------------------------------------------------------
//
// ObjlistTcl::Objlist_Init --
//
//---------------------------------------------------------------------

int Objlist_Init(Tcl_Interp* interp) {
DOTRACE("Objlist_Init");

  new ObjlistTcl::ObjListPkg(interp);
  new GrobjTcl::GrObjPkg(interp);

  return TCL_OK;
}

static const char vcid_objlisttcl_cc[] = "$Header$";
#endif // !OBJLISTTCL_CC_DEFINED
