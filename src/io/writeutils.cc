/** @file io/writeutils.cc helper functions for writing counted
    sequences of objects to an io::writer */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Nov 16 14:21:32 1999
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

#ifndef GROOVX_IO_WRITEUTILS_CC_UTC20050626084021_DEFINED
#define GROOVX_IO_WRITEUTILS_CC_UTC20050626084021_DEFINED

#include "io/writeutils.h"

#include "io/readutils.h"

using rutz::fstring;

fstring io::write_utils::make_element_name(const fstring& seq_name,
                                           int element_num)
{
  return read_utils::make_element_name(seq_name, element_num);
}

fstring io::write_utils::make_seq_length_name(const fstring& seq_name)
{
  return read_utils::make_seq_length_name(seq_name);
}

#endif // !GROOVX_IO_WRITEUTILS_CC_UTC20050626084021_DEFINED
