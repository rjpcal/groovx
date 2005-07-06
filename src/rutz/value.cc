/** @file rutz/value.cc */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Sep 28 11:21:32 1999
// commit: $Id$
// $HeadURL$
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

rutz::value::value()
{
GVX_TRACE("rutz::value::value");
}

rutz::value::~value()
{
GVX_TRACE("rutz::value::~value");
}

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

static const char vcid_groovx_rutz_value_cc_utc20050626084019[] = "$Id$ $HeadURL$";
#endif // !GROOVX_RUTZ_VALUE_CC_UTC20050626084019_DEFINED
