///////////////////////////////////////////////////////////////////////
//
// gtracetcl.cc
//
// Copyright (c) 2002-2001 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Jan 21 12:52:51 2002
// written: Wed Mar 19 17:58:07 2003
// $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
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
