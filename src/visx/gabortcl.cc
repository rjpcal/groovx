///////////////////////////////////////////////////////////////////////
//
// gabortcl.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Oct  6 14:16:30 1999
// written: Wed Mar 19 12:45:30 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GABORTCL_CC_DEFINED
#define GABORTCL_CC_DEFINED

#include "visx/gabor.h"

#include "tcl/fieldpkg.h"

#include "util/trace.h"

extern "C"
int Gabor_Init(Tcl_Interp* interp)
{
DOTRACE("Gabor_Init");

  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "Gabor", "$Revision$");
  Tcl::defFieldContainer<Gabor>(pkg);
  Tcl::defCreator<Gabor>(pkg);

  return pkg->initStatus();
}

static const char vcid_gabortcl_cc[] = "$Header$";
#endif // !GABORTCL_CC_DEFINED
