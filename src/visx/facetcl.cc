///////////////////////////////////////////////////////////////////////
//
// facetcl.cc
// Rob Peters 
// created: Jan-99
// written: Fri Oct 27 18:44:30 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FACETCL_CC_DEFINED
#define FACETCL_CC_DEFINED

#include "cloneface.h"
#include "ioptrlist.h"
#include "face.h"

#include "tcl/propitempkg.h"

#define NO_TRACE
#include "util/trace.h"

extern "C"
int Face_Init(Tcl_Interp* interp) {
DOTRACE("Face_Init");

  Tcl::TclPkg* pkg = new Tcl::PropertyListItemPkg<Face, IoPtrList>(
              interp, IoPtrList::theList(), "Face", "$Revision$"); 

  IO::IoFactory::theOne().registerCreatorFunc(&CloneFace::make);

  return pkg->initStatus();
}

static const char vcid_facetcl_cc[] = "$Header$";
#endif // !FACETCL_CC_DEFINED
