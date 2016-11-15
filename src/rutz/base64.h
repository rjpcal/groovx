/** @file rutz/base64.h functions for base64 encoding/decoding */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Fri Oct  8 13:44:03 2004
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

#ifndef GROOVX_RUTZ_BASE64_H_UTC20050626084019_DEFINED
#define GROOVX_RUTZ_BASE64_H_UTC20050626084019_DEFINED

#include <cstddef>
#include <string>

namespace rutz
{
  struct byte_array;

  std::string base64_encode(const unsigned char* src,
                            size_t src_len,
                            unsigned int line_width = 0);

  std::string base64_encode_string(const char* str,
                                   unsigned int line_width = 0);

  std::string base64_encode_file(const char* src_filename,
                                 unsigned int line_width = 0);

  rutz::byte_array base64_decode(const char* src,
                                 size_t in_len);

  rutz::byte_array base64_decode(const rutz::byte_array& src);

  rutz::byte_array base64_decode(const std::string& src);
}

#endif // !GROOVX_RUTZ_BASE64_H_UTC20050626084019_DEFINED
