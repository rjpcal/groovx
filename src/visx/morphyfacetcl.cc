///////////////////////////////////////////////////////////////////////
//
// morphyfacetcl.cc
// Rob Peters 
// created: Wed Sep  8 15:42:36 1999
// written: Fri Oct 27 18:44:43 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MORPHYFACETCL_CC_DEFINED
#define MORPHYFACETCL_CC_DEFINED

#include "ioptrlist.h"
#include "morphyface.h"

#include "tcl/propitempkg.h"

#define NO_TRACE
#include "util/trace.h"

extern "C"
int Morphyface_Init(Tcl_Interp* interp) {
DOTRACE("Morphyface_Init");

  Tcl::TclPkg* pkg = 
	 new Tcl::PropertyListItemPkg<MorphyFace, IoPtrList>(
              interp, IoPtrList::theList(), "MorphyFace", "1.2");

  return pkg->initStatus();
}

static const char vcid_morphyfacetcl_cc[] = "$Header$";
#endif // !MORPHYFACETCL_CC_DEFINED
