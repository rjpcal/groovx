///////////////////////////////////////////////////////////////////////
//
// facetcl.cc
// Rob Peters 
// created: Jan-99
// written: Fri Oct 20 14:56:29 2000
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

  FactoryRegistrar<IO::IoObject, CloneFace> :: registerWith(IO::IoFactory::theOne());

  return pkg->initStatus();
}

static const char vcid_facetcl_cc[] = "$Header$";
#endif // !FACETCL_CC_DEFINED
