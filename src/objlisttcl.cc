///////////////////////////////////////////////////////////////////////
//
// objlisttcl.cc
// Rob Peters
// created: Jan-99
// written: Wed Jul  7 14:27:46 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef OBJLISTTCL_CC_DEFINED
#define OBJLISTTCL_CC_DEFINED

#include "objlisttcl.h"

#include "objlist.h"
#include "listpkg.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

namespace ObjlistTcl {
  class ObjListPkg;
}

//---------------------------------------------------------------------
//
// ObjListPkg class definition
//
//---------------------------------------------------------------------

class ObjlistTcl::ObjListPkg : public ListPkg<ObjList> {
public:
  ObjListPkg(Tcl_Interp* interp) :
	 ListPkg<ObjList>(interp, ObjList::theObjList(), "ObjList", "3.0") {}
};

//---------------------------------------------------------------------
//
// ObjlistTcl::Objlist_Init --
//
//---------------------------------------------------------------------

int Objlist_Init(Tcl_Interp* interp) {
DOTRACE("Objlist_Init");

  new ObjlistTcl::ObjListPkg(interp);

  return TCL_OK;
}

static const char vcid_objlisttcl_cc[] = "$Header$";
#endif // !OBJLISTTCL_CC_DEFINED
