///////////////////////////////////////////////////////////////////////
//
// masktcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Sep 23 18:19:05 1999
// written: Wed Mar 15 10:17:28 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MASKTCL_CC_DEFINED
#define MASKTCL_CC_DEFINED

#include "maskhatch.h"
#include "objlist.h"
#include "propitempkg.h"

#define NO_TRACE
#include "util/trace.h"


extern "C"
int Mask_Init(Tcl_Interp* interp) {
DOTRACE("Mask_Init");

  Tcl::TclPkg* pkg = new Tcl::PropertyListItemPkg<MaskHatch, ObjList>(
            interp, ObjList::theObjList(), "MaskHatch", "1.1");

  return pkg->initStatus();
}

static const char vcid_masktcl_cc[] = "$Header$";
#endif // !MASKTCL_CC_DEFINED
