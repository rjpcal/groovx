///////////////////////////////////////////////////////////////////////
//
// morphyfacetcl.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Sep  8 15:42:36 1999
// written: Wed Mar 19 12:46:29 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MORPHYFACETCL_CC_DEFINED
#define MORPHYFACETCL_CC_DEFINED

#include "visx/morphyface.h"

#include "tcl/fieldpkg.h"

#include "util/trace.h"

extern "C"
int Morphyface_Init(Tcl_Interp* interp)
{
DOTRACE("Morphyface_Init");

  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "MorphyFace", "$Revision$");
  Tcl::defFieldContainer<MorphyFace>(pkg);
  Tcl::defCreator<MorphyFace>(pkg);

  return pkg->initStatus();
}

static const char vcid_morphyfacetcl_cc[] = "$Header$";
#endif // !MORPHYFACETCL_CC_DEFINED
