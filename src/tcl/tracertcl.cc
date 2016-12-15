/** @file tcl/tracertcl.cc define tcl commands for a rutz::tracer object */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Thu Feb 17 13:34:40 2000
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

#include "tcl/tracertcl.h"

#include "tcl/pkg.h"

#include "rutz/tracer.h"

void tcl::def_tracing(tcl::pkg* pkg, rutz::tracer& t)
{
  pkg->def( "traceOn",     "", [&t](){t.on();}, SRC_POS );
  pkg->def( "traceOff",    "", [&t](){t.off();}, SRC_POS );
  pkg->def( "traceToggle", "", [&t](){t.toggle();}, SRC_POS );
  pkg->def( "traceStatus", "", [&t](){t.status();}, SRC_POS );
}
