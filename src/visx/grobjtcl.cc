///////////////////////////////////////////////////////////////////////
//
// grobjtcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Jul  1 14:01:18 1999
// written: Thu Jul  1 14:12:36 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GROBJTCL_CC_DEFINED
#define GROBJTCL_CC_DEFINED

#include "grobjtcl.h"

#include <typeinfo>

#include "grobj.h"
#include "id.h"

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

GrobjTcl::GrObjPkg::GrObjPkg(Tcl_Interp* interp) :
  CTclIoItemPkg<GrObj>(interp, "GrObj", "2.5")
{
  addCommand( new TypeCmd(this, "GrObj::type") );
  declareAttrib("category", new CAttrib<GrObj, int>(&GrObj::getCategory,
																	 &GrObj::setCategory));
  declareAttrib("usingCompile", 
					 new CAttrib<GrObj, bool>(&GrObj::getUsingCompile,
													  &GrObj::setUsingCompile));
  declareAction("update", new CConstAction<GrObj>(&GrObj::update));
}

GrObj* GrobjTcl::GrObjPkg::getCItemFromId(int id) {
  ObjId objid(id);
  if ( !objid ) {
	 throw TclError("objid out of range");
  }
  return objid.get();
}

IO& GrobjTcl::GrObjPkg::getIoFromId(int id) {
  return dynamic_cast<IO&>( *(getCItemFromId(id)) );
}

extern "C" Tcl_PackageInitProc Grobj_Init;

int Grobj_Init(Tcl_Interp* interp) {

  new GrobjTcl::GrObjPkg(interp);

  return TCL_OK;
}

static const char vcid_grobjtcl_cc[] = "$Header$";
#endif // !GROBJTCL_CC_DEFINED
