///////////////////////////////////////////////////////////////////////
//
// gabortcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Oct  6 14:16:30 1999
// written: Wed Mar 15 10:17:30 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GABORTCL_CC_DEFINED
#define GABORTCL_CC_DEFINED

#include "objlist.h"
#include "propitempkg.h"
#include "gabor.h"

#define NO_TRACE
#include "util/trace.h"

extern "C"
int Gabor_Init(Tcl_Interp* interp) {
DOTRACE("Gabor_Init");

  Tcl::TclPkg* pkg = new Tcl::PropertyListItemPkg<Gabor, ObjList>(
                              interp, ObjList::theObjList(),
										"Gabor", "1.1");

  return pkg->initStatus();
}

static const char vcid_gabortcl_cc[] = "$Header$";
#endif // !GABORTCL_CC_DEFINED
