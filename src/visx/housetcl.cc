///////////////////////////////////////////////////////////////////////
//
// housetcl.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Sep 13 15:14:19 1999
// written: Wed Mar 19 12:45:29 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef HOUSETCL_CC_DEFINED
#define HOUSETCL_CC_DEFINED

#include "visx/house.h"

#include "tcl/fieldpkg.h"

#include "util/trace.h"


extern "C"
int House_Init(Tcl_Interp* interp)
{
DOTRACE("House_Init");

  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "House", "$Revision$");
  Tcl::defFieldContainer<House>(pkg);
  Tcl::defCreator<House>(pkg);

  return pkg->initStatus();
}

static const char vcid_housetcl_cc[] = "$Header$";
#endif // !HOUSETCL_CC_DEFINED
