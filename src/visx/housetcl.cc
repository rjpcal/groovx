///////////////////////////////////////////////////////////////////////
//
// housetcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Sep 13 15:14:19 1999
// written: Fri Oct 27 18:44:39 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef HOUSETCL_CC_DEFINED
#define HOUSETCL_CC_DEFINED

#include "house.h"
#include "ioptrlist.h"

#include "tcl/propitempkg.h"

#define NO_TRACE
#include "util/trace.h"


extern "C"
int House_Init(Tcl_Interp* interp) {
DOTRACE("House_Init");

  Tcl::TclPkg* pkg = new Tcl::PropertyListItemPkg<House, IoPtrList>(
											interp, IoPtrList::theList(),
											"House", "1.3");

  return pkg->initStatus();
}

static const char vcid_housetcl_cc[] = "$Header$";
#endif // !HOUSETCL_CC_DEFINED
