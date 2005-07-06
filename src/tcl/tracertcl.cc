/** @file tcl/tracertcl.cc define tcl commands for a rutz::tracer object */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Thu Feb 17 13:34:40 2000
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_TCL_TRACERTCL_CC_UTC20050626084018_DEFINED
#define GROOVX_TCL_TRACERTCL_CC_UTC20050626084018_DEFINED

#include "tcl/tracertcl.h"

#include "tcl/pkg.h"

#include "rutz/tracer.h"

void tcl::def_tracing(tcl::pkg* pkg, rutz::tracer& t)
{
  using namespace rutz;

  pkg->def( "traceOn",     "", bind_first(mem_func(&tracer::on), &t), SRC_POS );
  pkg->def( "traceOff",    "", bind_first(mem_func(&tracer::off), &t), SRC_POS );
  pkg->def( "traceToggle", "", bind_first(mem_func(&tracer::toggle), &t), SRC_POS );
  pkg->def( "traceStatus", "", bind_first(mem_func(&tracer::status), &t), SRC_POS );
}

static const char vcid_groovx_tcl_tracertcl_cc_utc20050626084018[] = "$Id$ $HeadURL$";
#endif // !GROOVX_TCL_TRACERTCL_CC_UTC20050626084018_DEFINED
