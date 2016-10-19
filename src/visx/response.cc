/** @file visx/response.cc user responses in a psychophysics experiment */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Mar 13 18:33:17 2000
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

#include "visx/response.h"

#include "rutz/fstring.h"

#include <iostream>

#include "rutz/trace.h"

///////////////////////////////////////////////////////////////////////
//
// Response member definitions
//
///////////////////////////////////////////////////////////////////////

const int Response::INVALID_VALUE;
const int Response::ALWAYS_CORRECT;
const int Response::IGNORE;

rutz::fstring Response::value_typename() const
{
GVX_TRACE("Response::value_typename");
  static rutz::fstring name("Response"); return name;
}

void Response::print_to(std::ostream& os) const
{
GVX_TRACE("Response::printTo");
  os << itsVal << " " << itsMsec;
}

void Response::scan_from(std::istream& is)
{
GVX_TRACE("Response::scan_from");
  is >> itsVal >> itsMsec;
}
