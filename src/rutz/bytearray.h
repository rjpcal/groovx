/** @file rutz/bytearray.h thin wrapper around std::vector<byte> */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Fri Oct  8 16:43:32 2004
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

#ifndef GROOVX_RUTZ_BYTEARRAY_H_UTC20050626084020_DEFINED
#define GROOVX_RUTZ_BYTEARRAY_H_UTC20050626084020_DEFINED

#include <vector>

namespace rutz
{
  struct byte_array
  {
    std::vector<unsigned char> vec;
  };
}

static const char vcid_groovx_rutz_bytearray_h_utc20050626084020[] = "$Id$ $HeadURL$";
#endif // !GROOVX_RUTZ_BYTEARRAY_H_UTC20050626084020_DEFINED
