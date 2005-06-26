///////////////////////////////////////////////////////////////////////
//
// tclregexp.cc
//
// Copyright (c) 2001-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Jul 16 13:09:12 2001
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

#ifndef GROOVX_TCL_TCLREGEXP_CC_UTC20050626084017_DEFINED
#define GROOVX_TCL_TCLREGEXP_CC_UTC20050626084017_DEFINED

#include "tcl/tclregexp.h"

#include "rutz/error.h"

#include <tcl.h>

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

namespace
{
  Tcl_RegExp getCheckedRegexp(const Tcl::ObjPtr& patrn)
  {
    const int flags = 0;
    // OK to pass Tcl_Interp*==0
    Tcl_RegExp regexp = Tcl_GetRegExpFromObj(0, patrn.obj(), flags);
    if (!regexp)
      {
        throw rutz::error(rutz::fstring("error getting a regexp from '",
                                        Tcl_GetString(patrn.obj()), "'"),
                          SRC_POS);
      }
    return regexp;
  }
}

bool Tcl::RegExp::matchesString(const char* str)
{
  static const int REGEX_ERROR        = -1;
  static const int REGEX_NO_MATCH     =  0;
  static const int REGEX_FOUND_MATCH  =  1;

  Tcl_RegExp regexp = getCheckedRegexp(itsPatternObj);

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

static const char vcid_groovx_tcl_tclregexp_cc_utc20050626084017[] = "$Id$ $HeadURL$";
#endif // !GROOVX_TCL_TCLREGEXP_CC_UTC20050626084017_DEFINED
