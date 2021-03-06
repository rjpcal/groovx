/** @file rutz/demangle.h get a demangled name from a std::type_info
    object */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Wed Oct 13 10:41:03 1999
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

#ifndef GROOVX_RUTZ_DEMANGLE_H_UTC20050626084020_DEFINED
#define GROOVX_RUTZ_DEMANGLE_H_UTC20050626084020_DEFINED

#include <typeinfo>

namespace rutz
{
  /// Returns a demangled typename for the given type.
  const char* demangled_name(const std::type_info& info);
}

#endif // !GROOVX_RUTZ_DEMANGLE_H_UTC20050626084020_DEFINED
