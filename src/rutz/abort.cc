/** @file rutz/abort.cc low-level GVX_ABORT macro */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005-2007 University of Southern California
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Jun 30 15:26:51 2005
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

#include "rutz/abort.h"

#include <cstdlib>
#include <cstdio>

void rutz::debug::abort_aux (const char* what, const char* where,
                             int line_no) noexcept
{
  fprintf(stderr, "Abort (%s:%d):\n\tgot '%s'\n\n",
          where, line_no, what);
  abort();
}
