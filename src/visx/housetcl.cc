///////////////////////////////////////////////////////////////////////
//
// housetcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Sep 13 15:14:19 1999
// written: Wed Mar  8 16:33:13 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef HOUSETCL_CC_DEFINED
#define HOUSETCL_CC_DEFINED

#include "house.h"
#include "objlist.h"
#include "propitempkg.h"

#define NO_TRACE
#include "trace.h"


extern "C"
int House_Init(Tcl_Interp* interp) {
DOTRACE("House_Init");

  Tcl::TclPkg* pkg = new Tcl::PropertyListItemPkg<House, ObjList>(
											interp, ObjList::theObjList(),
											"House", "1.3");

  return pkg->initStatus();
}

static const char vcid_housetcl_cc[] = "$Header$";
#endif // !HOUSETCL_CC_DEFINED
