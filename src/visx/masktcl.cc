///////////////////////////////////////////////////////////////////////
//
// masktcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Sep 23 18:19:05 1999
// written: Tue Dec  7 19:15:03 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MASKTCL_CC_DEFINED
#define MASKTCL_CC_DEFINED

#include <tcl.h>

#include "maskhatch.h"
#include "objlist.h"
#include "propitempkg.h"

#define NO_TRACE
#include "trace.h"


extern "C" Tcl_PackageInitProc Mask_Init;

int Mask_Init(Tcl_Interp* interp) {
DOTRACE("Mask_Init");

  new Tcl::PropertyListItemPkg<MaskHatch, ObjList>(
            interp, ObjList::theObjList(), "MaskHatch", "1.1");

  return TCL_OK;
}

static const char vcid_masktcl_cc[] = "$Header$";
#endif // !MASKTCL_CC_DEFINED
