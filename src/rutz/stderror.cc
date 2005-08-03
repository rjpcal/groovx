/** @file rutz/stderror.cc throw common exceptions with standardized
    messages */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2003-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Sat May 24 13:02:28 2003
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

#ifndef GROOVX_RUTZ_STDERROR_CC_UTC20050626084019_DEFINED
#define GROOVX_RUTZ_STDERROR_CC_UTC20050626084019_DEFINED

#include "rutz/stderror.h"

#include "rutz/demangle.h"
#include "rutz/error.h"

void rutz::throw_bad_cast(const std::type_info& to,
                          const std::type_info& from,
                          const rutz::file_pos& pos)
{
  throw rutz::error(rutz::fstring("failed cast: expected '",
                                  rutz::demangled_name(to),
                                  "', got '",
                                  rutz::demangled_name(from),
                                  "'"), pos);
}

static const char vcid_groovx_rutz_stderror_cc_utc20050626084019[] = "$Id$ $HeadURL$";
#endif // !GROOVX_RUTZ_STDERROR_CC_UTC20050626084019_DEFINED
