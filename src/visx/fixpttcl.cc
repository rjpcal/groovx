///////////////////////////////////////////////////////////////////////
//
// fixpttcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Jan-99
// written: Wed Jul 18 11:13:31 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FIXPTTCL_CC_DEFINED
#define FIXPTTCL_CC_DEFINED

#include "fixpt.h"

#include "tcl/fieldpkg.h"

#define NO_TRACE
#include "util/trace.h"

extern "C"
int Fixpt_Init(Tcl_Interp* interp)
{
DOTRACE("Fixpt_Init");

  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "FixPt", "$Revision$");
  Tcl::defFieldContainer<FixPt>(pkg);

  return pkg->initStatus();
}

static const char vcid_fixpttcl_cc[] = "$Header$";
#endif // !FIXPTTCL_CC_DEFINED
