///////////////////////////////////////////////////////////////////////
//
// morphyfacetcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Sep  8 15:42:36 1999
// written: Wed Jul 18 11:13:31 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MORPHYFACETCL_CC_DEFINED
#define MORPHYFACETCL_CC_DEFINED

#include "morphyface.h"

#include "tcl/fieldpkg.h"

#define NO_TRACE
#include "util/trace.h"

extern "C"
int Morphyface_Init(Tcl_Interp* interp)
{
DOTRACE("Morphyface_Init");

  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "MorphyFace", "$Revision$");
  Tcl::defFieldContainer<MorphyFace>(pkg);

  return pkg->initStatus();
}

static const char vcid_morphyfacetcl_cc[] = "$Header$";
#endif // !MORPHYFACETCL_CC_DEFINED
