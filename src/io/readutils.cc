/** @file io/readutils.cc helper functions for reading counted
    sequences of objects from an io::reader */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Tue Nov 16 14:25:40 1999
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#include "io/readutils.h"

#include "rutz/fstring.h"
#include "rutz/sfmt.h"

using rutz::fstring;

fstring io::read_utils::make_element_name(const fstring& seq_name,
                                          int element_num)
{
  return rutz::sfmt("%s%d", seq_name.c_str(), element_num);
}

fstring io::read_utils::make_seq_length_name(const fstring& seq_name)
{
  return rutz::sfmt("%sCount", seq_name.c_str());
}
