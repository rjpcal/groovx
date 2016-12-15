/** @file rutz/stderror.cc throw common exceptions with standardized
    messages */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2003-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Sat May 24 13:02:28 2003
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

#include "rutz/stderror.h"

#include "rutz/demangle.h"
#include "rutz/error.h"
#include "rutz/sfmt.h"

void rutz::throw_bad_cast(const std::type_info& to,
                          const std::type_info& from,
                          const rutz::file_pos& pos)
{
  throw rutz::error(rutz::sfmt("failed cast: expected '%s', got '%s'",
                               rutz::demangled_name(to),
                               rutz::demangled_name(from)), pos);
}
