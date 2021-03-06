/** @file tcl/tclpkg-log.cc tcl interface package for nub::logging */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Wed Jan 14 15:19:06 2004
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#include "tcl/tclpkg-log.h"

#include "nub/log.h"

#include "tcl/pkg.h"

#include "rutz/fstring.h"

#include "rutz/debug.h"
GVX_DBG_REGISTER
#include "rutz/trace.h"

extern "C"
int Log_Init(Tcl_Interp* interp)
{
GVX_TRACE("Log_Init");

  return tcl::pkg::init
    (interp, "log", "4.0",
     [](tcl::pkg* pkg) {
      pkg->def("reset", "", &nub::logging::reset, SRC_POS);
      pkg->def("add_scope", "scopename", &::nub::logging::add_scope, SRC_POS);
      pkg->def("remove_scope", "scopename", &::nub::logging::remove_scope, SRC_POS);
      pkg->def("filename", "filename", &nub::logging::set_log_filename, SRC_POS);
      pkg->def("copy_to_stdout", "shouldcopy", &nub::logging::copy_to_stdout, SRC_POS);
      pkg->def("log", "msg", (void (*)(const char*)) &nub::log, SRC_POS);
    });
}
