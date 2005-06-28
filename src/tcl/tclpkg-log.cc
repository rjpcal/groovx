///////////////////////////////////////////////////////////////////////
//
// logtcl.cc
//
// Copyright (c) 2004-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Jan 14 15:19:06 2004
// commit: $Id$
// $HeadURL$
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

#ifndef GROOVX_TCL_TCLPKG_LOG_CC_UTC20050628161246_DEFINED
#define GROOVX_TCL_TCLPKG_LOG_CC_UTC20050628161246_DEFINED

#include "tcl/tclpkg-log.h"

#include "nub/log.h"

#include "tcl/tclpkg.h"

#include "rutz/fstring.h"

#include "rutz/debug.h"
GVX_DBG_REGISTER
#include "rutz/trace.h"

extern "C"
int Log_Init(Tcl_Interp* interp)
{
GVX_TRACE("Log_Init");

  GVX_PKG_CREATE(pkg, interp, "Log", "4.$Revision$");
  pkg->def("reset", "", &Nub::Log::reset, SRC_POS);
  pkg->def("addScope", "scopename", &::Nub::Log::addScope, SRC_POS);
  pkg->def("removeScope", "scopename", &::Nub::Log::removeScope, SRC_POS);
  pkg->def("logFilename", "filename", &Nub::Log::setLogFilename, SRC_POS);
  pkg->def("copyToStdout", "shouldcopy", &Nub::Log::setCopyToStdout, SRC_POS);

  pkg->def("log", "msg", (void (*)(const char*)) &Nub::log, SRC_POS);

  GVX_PKG_RETURN(pkg);
}

static const char vcid_groovx_tcl_tclpkg_log_cc_utc20050628161246[] = "$Id$ $HeadURL$";
#endif // !GROOVX_TCL_TCLPKG_LOG_CC_UTC20050628161246_DEFINED
