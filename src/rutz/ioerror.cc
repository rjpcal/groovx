///////////////////////////////////////////////////////////////////////
//
// ioerror.cc
//
// Copyright (c) 2001-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Sep 12 21:43:15 2001
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

#ifndef GROOVX_RUTZ_IOERROR_CC_UTC20050626084020_DEFINED
#define GROOVX_RUTZ_IOERROR_CC_UTC20050626084020_DEFINED

#include "ioerror.h"

#include "rutz/trace.h"

rutz::filename_error::filename_error(const char* str,
                                     const rutz::file_pos& pos)
  :
  rutz::error(rutz::fstring("couldn't use file '", str, "'"), pos)
{
GVX_TRACE("rutz::filename_error::filename_error");
}

rutz::filename_error::~filename_error() throw() {}

static const char vcid_groovx_rutz_ioerror_cc_utc20050626084020[] = "$Id$ $HeadURL$";
#endif // !GROOVX_RUTZ_IOERROR_CC_UTC20050626084020_DEFINED