/** @file tcl/regexp.cc c++ wrapper class for tcl regexp objects */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Jul 16 13:09:12 2001
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

#include "tcl/regexp.h"

#include "rutz/error.h"
#include "rutz/sfmt.h"

#include <tcl.h>

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

namespace
{
  Tcl_RegExp get_checked_regexp(const tcl::obj& patrn)
  {
    const int flags = 0;
    // OK to pass Tcl_Interp*==0
    Tcl_RegExp regexp = Tcl_GetRegExpFromObj(0, patrn.get(), flags);
    if (!regexp)
      {
        throw rutz::error(rutz::sfmt("error getting a regexp from '%s'",
                                     Tcl_GetString(patrn.get())),
                          SRC_POS);
      }
    return regexp;
  }
}

bool tcl::regexp::matches_string(const char* str)
{
  static const int REGEX_ERROR        = -1;
  static const int REGEX_NO_MATCH     =  0;
  static const int REGEX_FOUND_MATCH  =  1;

  Tcl_RegExp regexp = get_checked_regexp(m_pattern);

  // OK to pass Tcl_Interp*==0
  int regex_result = Tcl_RegExpExec(0, regexp, str, str);

  switch (regex_result)
  {
  case REGEX_ERROR:
    throw rutz::error("error executing regular expression", SRC_POS);

  case REGEX_NO_MATCH:
    return false;

  case REGEX_FOUND_MATCH:
    return true;

  default: // "can't happen"
    GVX_ASSERT(false);
  }

  return false;
}
