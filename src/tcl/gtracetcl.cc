///////////////////////////////////////////////////////////////////////
//
// gtracetcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jan 21 12:52:51 2002
// written: Fri Nov 22 17:46:07 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GTRACE_CC_DEFINED
#define GTRACE_CC_DEFINED

#include "tcl/tclpkg.h"

#include "util/debug.h"
#include "util/trace.h"

namespace
{
  int getLevel() { return Debug::level; }
  void setLevel(int v) { Debug::level = v; }
}

extern "C"
int Gtrace_Init(Tcl_Interp* interp)
{
DOTRACE("Gtrace_Init");

  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "Gtrace", "$Revision$");
  pkg->def("::gtrace", "on_off", &Util::Trace::setGlobalTrace);
  pkg->def("::gtrace", "", &Util::Trace::getGlobalTrace);
  pkg->def("maxDepth", "level", &Util::Trace::setMaxLevel);
  pkg->def("maxDepth", "", &Util::Trace::getMaxLevel);
  pkg->def("::dbglevel", "", &getLevel);
  pkg->def("::dbglevel", "level", &setLevel);

  return pkg->initStatus();
}

static const char vcid_gtrace_cc[] = "$Header$";
#endif // !GTRACE_CC_DEFINED