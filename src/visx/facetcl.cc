///////////////////////////////////////////////////////////////////////
//
// facetcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Jan-99
// written: Wed Jul 18 09:48:26 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FACETCL_CC_DEFINED
#define FACETCL_CC_DEFINED

#include "cloneface.h"
#include "face.h"

#include "tcl/fieldpkg.h"

#define NO_TRACE
#include "util/trace.h"

extern "C"
int Face_Init(Tcl_Interp* interp)
{
DOTRACE("Face_Init");

  Tcl::TclItemPkg* pkg = new Tcl::TclItemPkg(interp, "Face", "$Revision$");
  Tcl::defFieldContainer<Face>(pkg);

  Tcl::TclItemPkg* pkg2 = new Tcl::TclItemPkg(interp, "CloneFace", "$Revision$");
  Tcl::defFieldContainer<CloneFace>(pkg);

  return Tcl::TclPkg::initStatus(pkg, pkg2);
}

static const char vcid_facetcl_cc[] = "$Header$";
#endif // !FACETCL_CC_DEFINED
