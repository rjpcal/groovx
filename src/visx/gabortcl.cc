///////////////////////////////////////////////////////////////////////
//
// gabortcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Oct  6 14:16:30 1999
// written: Tue Dec  7 19:15:03 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GABORTCL_CC_DEFINED
#define GABORTCL_CC_DEFINED

#include <tcl.h>

#include "objlist.h"
#include "propitempkg.h"
#include "gabor.h"

#define NO_TRACE
#include "trace.h"

extern "C" Tcl_PackageInitProc Gabor_Init;

int Gabor_Init(Tcl_Interp* interp) {
DOTRACE("Gabor_Init");

  new Tcl::PropertyListItemPkg<Gabor, ObjList>(interp, ObjList::theObjList(),
															  "Gabor", "1.1");

  return TCL_OK;
}

static const char vcid_gabortcl_cc[] = "$Header$";
#endif // !GABORTCL_CC_DEFINED
