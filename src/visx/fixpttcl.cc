///////////////////////////////////////////////////////////////////////
//
// fixpttcl.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jan  4 08:00:00 1999
// written: Fri Jan 18 16:07:02 2002
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
