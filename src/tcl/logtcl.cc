///////////////////////////////////////////////////////////////////////
//
// logtcl.cc
//
// Copyright (c) 2004-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Jan 14 15:19:06 2004
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

#ifndef LOGTCL_CC_DEFINED
#define LOGTCL_CC_DEFINED

#include "nub/log.h"

#include "tcl/tclpkg.h"

#include "util/fstring.h"

#include "util/debug.h"
DBG_REGISTER
#include "util/trace.h"

extern "C"
int Log_Init(Tcl_Interp* interp)
{
DOTRACE("Log_Init");

  PKG_CREATE(interp, "Log", "4.$Revision$");
  pkg->def("reset", "", &Nub::Log::reset, SRC_POS);
  pkg->def("addScope", "scopename", &::Nub::Log::addScope, SRC_POS);
  pkg->def("removeScope", "scopename", &::Nub::Log::removeScope, SRC_POS);
  pkg->def("logFilename", "filename", &Nub::Log::setLogFilename, SRC_POS);
  pkg->def("copyToStdout", "shouldcopy", &Nub::Log::setCopyToStdout, SRC_POS);

  pkg->def("log", "msg", (void (*)(const char*)) &Nub::log, SRC_POS);

  PKG_RETURN;
}

static const char vcid_logtcl_cc[] = "$Id$ $URL$";
#endif // !LOGTCL_CC_DEFINED
