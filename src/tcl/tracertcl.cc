///////////////////////////////////////////////////////////////////////
//
// tracertcl.cc
//
// Copyright (c) 2000-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Feb 17 13:34:40 2000
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

#ifndef TRACERTCL_CC_DEFINED
#define TRACERTCL_CC_DEFINED

#include "tcl/tracertcl.h"

#include "tcl/tclpkg.h"

#include "util/tracer.h"

void Tcl::defTracing(Tcl::Pkg* pkg, rutz::tracer& t)
{
  using namespace rutz;

  pkg->def( "traceOn",     "", bind_first(mem_func(&tracer::on), &t), SRC_POS );
  pkg->def( "traceOff",    "", bind_first(mem_func(&tracer::off), &t), SRC_POS );
  pkg->def( "traceToggle", "", bind_first(mem_func(&tracer::toggle), &t), SRC_POS );
  pkg->def( "traceStatus", "", bind_first(mem_func(&tracer::status), &t), SRC_POS );
}

static const char vcid_tracertcl_cc[] = "$Header$";
#endif // !TRACERTCL_CC_DEFINED
