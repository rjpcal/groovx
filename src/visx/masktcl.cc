///////////////////////////////////////////////////////////////////////
//
// masktcl.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Sep 23 18:19:05 1999
// written: Fri Jan 18 16:07:00 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MASKTCL_CC_DEFINED
#define MASKTCL_CC_DEFINED

#include "visx/maskhatch.h"

#include "tcl/fieldpkg.h"

#include "util/trace.h"

extern "C"
int Mask_Init(Tcl_Interp* interp)
{
DOTRACE("Mask_Init");

  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "MaskHatch", "$Revision$");
  Tcl::defFieldContainer<MaskHatch>(pkg);
  Tcl::defCreator<MaskHatch>(pkg);

  return pkg->initStatus();
}

static const char vcid_masktcl_cc[] = "$Header$";
#endif // !MASKTCL_CC_DEFINED
