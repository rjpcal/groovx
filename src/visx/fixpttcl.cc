///////////////////////////////////////////////////////////////////////
//
// fixpttcl.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Jan  4 08:00:00 1999
// written: Wed Mar 19 12:45:30 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FIXPTTCL_CC_DEFINED
#define FIXPTTCL_CC_DEFINED

#include "visx/fixpt.h"

#include "tcl/fieldpkg.h"

#include "util/trace.h"

extern "C"
int Fixpt_Init(Tcl_Interp* interp)
{
DOTRACE("Fixpt_Init");

  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "FixPt", "$Revision$");
  Tcl::defFieldContainer<FixPt>(pkg);
  Tcl::defCreator<FixPt>(pkg);

  return pkg->initStatus();
}

static const char vcid_fixpttcl_cc[] = "$Header$";
#endif // !FIXPTTCL_CC_DEFINED
