///////////////////////////////////////////////////////////////////////
//
// gtracetcl.cc
//
// Copyright (c) 2002-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Jan 21 12:52:51 2002
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GTRACE_CC_DEFINED
#define GTRACE_CC_DEFINED

#include "tcl/tclpkg.h"

#include "util/error.h"
#include "util/strings.h"

#include <sstream>

#include "util/debug.h"
DBG_REGISTER
#include "util/trace.h"

namespace
{
  fstring profSummary()
  {
    std::ostringstream oss;
    Util::Prof::printAllProfData(oss);
    return fstring(oss.str().c_str());
  }

  void setOneLevel(int key, int level)
  {
    if (key < 0 || key >= Debug::MAX_KEYS)
      throw Util::Error(fstring("no such debug key '", key, "'"));

    // else...
    Debug::keyLevels[key] = level;
  }

  void setOneLevelc(const char* fname, int level)
  {
    const int key = Debug::lookupKey(fname);
    if (key == -1)
      throw Util::Error(fstring("no debug key for file '", fname, "'"));

    Assert(key >= 0 && key < Debug::MAX_KEYS);

    Debug::keyLevels[key] = level;
  }
}

extern "C"
int Gtrace_Init(Tcl_Interp* interp)
{
DOTRACE("Gtrace_Init");

  PKG_CREATE(interp, "Gtrace", "$Revision$");
  pkg->def("::gtrace", "on_off", &Util::Trace::setGlobalTrace, SRC_POS);
  pkg->def("::gtrace", "", &Util::Trace::getGlobalTrace, SRC_POS);
  pkg->def("maxDepth", "level", &Util::Trace::setMaxLevel, SRC_POS);
  pkg->def("maxDepth", "", &Util::Trace::getMaxLevel, SRC_POS);
  pkg->def("::dbglevelg", "global_level", &Debug::setGlobalLevel, SRC_POS);
  pkg->def("::dbglevel", "key level", &setOneLevel, SRC_POS);
  pkg->def("::dbglevelc", "filename level", &setOneLevelc, SRC_POS);
  pkg->def("::dbgkey", "filename", &Debug::lookupKey, SRC_POS);

  PKG_RETURN;
}

extern "C"
int Prof_Init(Tcl_Interp* interp)
{
DOTRACE("Prof_Init");

  PKG_CREATE(interp, "Prof", "$Revision$");
  pkg->def("summary", "", &profSummary, SRC_POS);
  pkg->def("reset", "", &Util::Prof::resetAllProfData, SRC_POS);

  PKG_RETURN;
}

static const char vcid_gtrace_cc[] = "$Header$";
#endif // !GTRACE_CC_DEFINED
