/** @file io/io.cc abstract base class for serializable objects */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Mar  9 20:25:02 1999
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

#ifndef GROOVX_IO_IO_CC_UTC20050626084021_DEFINED
#define GROOVX_IO_IO_CC_UTC20050626084021_DEFINED

#include "io/io.h"

#include "rutz/trace.h"

io::serializable::serializable() noexcept
{
GVX_TRACE("io::serializable::serializable");
}

// Must be defined out of line to avoid duplication of io's vtable
io::serializable::~serializable() noexcept
{
GVX_TRACE("io::serializable::~serializable");
}

io::version_id io::serializable::class_version_id() const
{
GVX_TRACE("io::serializable::class_version_id");
  return 0;
}

#endif // !GROOVX_IO_IO_CC_UTC20050626084021_DEFINED
