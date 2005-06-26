///////////////////////////////////////////////////////////////////////
//
// tracertcl.h
//
// Copyright (c) 2000-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Feb 17 13:32:11 2000
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

#ifndef TRACERTCL_H_DEFINED
#define TRACERTCL_H_DEFINED

namespace rutz
{
  class tracer;
}

namespace Tcl
{
  class Pkg;

  void defTracing(Tcl::Pkg* pkg, rutz::tracer& tracer);
}

static const char vcid_tracertcl_h[] = "$Id$ $URL$";
#endif // !TRACERTCL_H_DEFINED
