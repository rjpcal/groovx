/** @file tcl/channelbuf.h bridge class between tcl channels and c++
    streams */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Jun 30 17:01:08 2004
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

#ifndef GROOVX_TCL_CHANNELBUF_H_UTC20050628162420_DEFINED
#define GROOVX_TCL_CHANNELBUF_H_UTC20050628162420_DEFINED

#include <istream>
#include <ostream>
#include <tcl.h>

namespace tcl
{
  std::unique_ptr<std::ostream>
  ochanopen(Tcl_Interp* interp,
            const char* channame,
            std::ios::openmode flags = std::ios::openmode(0));

  std::unique_ptr<std::istream>
  ichanopen(Tcl_Interp* interp,
            const char* channame,
            std::ios::openmode flags = std::ios::openmode(0));
}

#endif // !GROOVX_TCL_CHANNELBUF_H_UTC20050628162420_DEFINED
