/** @file rutz/value.cc abstract interface for string-convertible types */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Sep 28 11:21:32 1999
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

#ifndef GROOVX_RUTZ_VALUE_CC_UTC20050626084019_DEFINED
#define GROOVX_RUTZ_VALUE_CC_UTC20050626084019_DEFINED

#include "rutz/value.h"

#include "rutz/cstrstream.h"
#include "rutz/fstring.h"

#include <sstream>

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

rutz::fstring rutz::value::get_string() const
{
  std::ostringstream oss;
  print_to(oss);

  return rutz::fstring(oss.str().c_str());
}

void rutz::value::set_string(rutz::fstring val)
{
  rutz::icstrstream ist(val.c_str());
  scan_from(ist);
}

#endif // !GROOVX_RUTZ_VALUE_CC_UTC20050626084019_DEFINED
