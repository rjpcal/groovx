/** @file tcl/regexp.h c++ wrapper class for tcl regexp objects */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Mon Jul 16 13:05:59 2001
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

#ifndef GROOVX_TCL_REGEXP_H_UTC20050628162421_DEFINED
#define GROOVX_TCL_REGEXP_H_UTC20050628162421_DEFINED

#include "tcl/obj.h"

namespace tcl
{
  class regexp;
}

/// Regular-expression class implemented with Tcl's regexp facilities.
class tcl::regexp
{
public:
  regexp() : m_pattern() {}

  template <class T>
  regexp(T val) : m_pattern(val) {}

  bool matches_string(const char* str);

private:
  tcl::obj m_pattern;
};

#endif // !GROOVX_TCL_REGEXP_H_UTC20050628162421_DEFINED
