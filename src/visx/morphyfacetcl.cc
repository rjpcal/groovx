///////////////////////////////////////////////////////////////////////
//
// morphyfacetcl.cc
// Rob Peters 
// created: Wed Sep  8 15:42:36 1999
// written: Mon Oct  4 15:55:47 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FACETCL_CC_DEFINED
#define FACETCL_CC_DEFINED

#include <tcl.h>

#include "objlist.h"
#include "propitempkg.h"
#include "morphyface.h"

#define NO_TRACE
#include "trace.h"


extern "C" Tcl_PackageInitProc Morphyface_Init;

int Morphyface_Init(Tcl_Interp* interp) {
DOTRACE("Morphyface_Init");

  new PropertyListItemPkg<MorphyFace, ObjList>(interp, ObjList::theObjList(),
															  "MorphyFace", "1.2");

  return TCL_OK;
}

static const char vcid_morphyfacetcl_cc[] = "$Header$";
#endif // !FACETCL_CC_DEFINED
