///////////////////////////////////////////////////////////////////////
//
// facetcl.cc
// Rob Peters 
// created: Jan-99
// written: Fri Oct 20 17:01:48 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FACETCL_CC_DEFINED
#define FACETCL_CC_DEFINED

#include "cloneface.h"
#include "objlist.h"
#include "face.h"

#include "tcl/propitempkg.h"

#define NO_TRACE
#include "util/trace.h"

extern "C"
int Face_Init(Tcl_Interp* interp) {
DOTRACE("Face_Init");

  Tcl::TclPkg* pkg = new Tcl::PropertyListItemPkg<Face, ObjList>(
              interp, ObjList::theObjList(), "Face", "$Revision$"); 

  IO::IoFactory::theOne().registerCreatorFunc(&CloneFace::make);

  return pkg->initStatus();
}

static const char vcid_facetcl_cc[] = "$Header$";
#endif // !FACETCL_CC_DEFINED
