///////////////////////////////////////////////////////////////////////
//
// housetcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Sep 13 15:14:19 1999
// written: Tue Dec  7 19:15:03 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef HOUSETCL_CC_DEFINED
#define HOUSETCL_CC_DEFINED

#include <tcl.h>

#include "house.h"
#include "objlist.h"
#include "propitempkg.h"

#define NO_TRACE
#include "trace.h"


extern "C" Tcl_PackageInitProc House_Init;

int House_Init(Tcl_Interp* interp) {
DOTRACE("House_Init");

  new Tcl::PropertyListItemPkg<House, ObjList>(interp, ObjList::theObjList(),
															  "House", "1.3");

  return TCL_OK;
}

static const char vcid_housetcl_cc[] = "$Header$";
#endif // !HOUSETCL_CC_DEFINED
