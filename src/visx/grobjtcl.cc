///////////////////////////////////////////////////////////////////////
//
// grobjtcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Jul  1 14:01:18 1999
// written: Mon Jul 12 13:05:03 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJTCL_CC_DEFINED
#define GROBJTCL_CC_DEFINED

#include "grobjtcl.h"

#include <typeinfo>

#include "grobj.h"
#include "objlist.h"
#include "listitempkg.h"

///////////////////////////////////////////////////////////////////////
//
// Grobj Tcl package
//
///////////////////////////////////////////////////////////////////////

namespace GrobjTcl {
  class TypeCmd;
  class GrObjPkg;
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

//---------------------------------------------------------------------
//
// GrObjPkg definitions --
//
//---------------------------------------------------------------------

class GrobjTcl::GrObjPkg : public AbstractListItemPkg<GrObj, ObjList> {
public:
  GrObjPkg(Tcl_Interp* interp) :
	 AbstractListItemPkg<GrObj, ObjList>(interp, ObjList::theObjList(),
													 "GrObj", "2.5")
  {
	 addCommand( new TypeCmd(this, "GrObj::type") );
	 declareCAttrib("category", &GrObj::getCategory, &GrObj::setCategory);
	 declareCAttrib("usingCompile",
						 &GrObj::getUsingCompile, &GrObj::setUsingCompile);
	 declareCAction("update", &GrObj::update);
  }
};

//---------------------------------------------------------------------
//
// Grobj_Init --
//
//---------------------------------------------------------------------

int Grobj_Init(Tcl_Interp* interp) {

  new GrobjTcl::GrObjPkg(interp);

  return TCL_OK;
}

static const char vcid_grobjtcl_cc[] = "$Header$";
#endif // !GROBJTCL_CC_DEFINED
