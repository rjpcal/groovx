///////////////////////////////////////////////////////////////////////
//
// morphyfacetcl.cc
// Rob Peters 
// created: Wed Sep  8 15:42:36 1999
// written: Wed Mar 15 10:17:28 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FACETCL_CC_DEFINED
#define FACETCL_CC_DEFINED

#include "objlist.h"
#include "propitempkg.h"
#include "morphyface.h"

#define NO_TRACE
#include "util/trace.h"

extern "C"
int Morphyface_Init(Tcl_Interp* interp) {
DOTRACE("Morphyface_Init");

  Tcl::TclPkg* pkg = 
	 new Tcl::PropertyListItemPkg<MorphyFace, ObjList>(
              interp, ObjList::theObjList(), "MorphyFace", "1.2");

  return pkg->initStatus();
}

static const char vcid_morphyfacetcl_cc[] = "$Header$";
#endif // !FACETCL_CC_DEFINED
