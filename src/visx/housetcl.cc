///////////////////////////////////////////////////////////////////////
//
// housetcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Sep 13 15:14:19 1999
// written: Thu Sep 30 11:04:53 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef HOUSETCL_CC_DEFINED
#define HOUSETCL_CC_DEFINED

#include <tcl.h>

#include "house.h"
#include "iomgr.h"
#include "objlist.h"
#include "listitempkg.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

///////////////////////////////////////////////////////////////////////
//
// HousePkg class definition
//
///////////////////////////////////////////////////////////////////////

namespace HouseTcl {
  class HousePkg;
}

class HouseTcl::HousePkg : public ListItemPkg<House, ObjList> {
public:
  HousePkg(Tcl_Interp* interp) :
	 ListItemPkg<House, ObjList>(interp, ObjList::theObjList(), "House", "1.1")
  {
	 declareAllProperties();
  }
};

extern "C" Tcl_PackageInitProc House_Init;

int House_Init(Tcl_Interp* interp) {
DOTRACE("House_Init");

  new HouseTcl::HousePkg(interp);

  FactoryRegistrar<IO, House> registrar(IoFactory::theOne());

  return TCL_OK;
}

static const char vcid_housetcl_cc[] = "$Header$";
#endif // !HOUSETCL_CC_DEFINED
