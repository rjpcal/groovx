///////////////////////////////////////////////////////////////////////
//
// gtrace.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jan 21 12:52:51 2002
// written: Mon Jan 21 12:55:57 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GTRACE_CC_DEFINED
#define GTRACE_CC_DEFINED

#include "tcl/tclpkg.h"

#include "util/trace.h"

extern "C"
int Gtrace_Init(Tcl_Interp* interp)
{
DOTRACE("Gtrace_Init");

  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "Gtrace", "$Revision$");
  pkg->def("::gtrace", "on_off", &Util::Trace::setGlobalTrace);
  pkg->def("::gtrace", "", &Util::Trace::getGlobalTrace);
  pkg->def("maxDepth", "level", &Util::Trace::setMaxLevel);
  pkg->def("maxDepth", "", &Util::Trace::getMaxLevel);

  return pkg->initStatus();
}

static const char vcid_gtrace_cc[] = "$Header$";
#endif // !GTRACE_CC_DEFINED
