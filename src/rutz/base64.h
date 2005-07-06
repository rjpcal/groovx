/** @file rutz/base64.h */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Fri Oct  8 13:44:03 2004
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

#ifndef GROOVX_RUTZ_BASE64_H_UTC20050626084019_DEFINED
#define GROOVX_RUTZ_BASE64_H_UTC20050626084019_DEFINED

namespace rutz
{
  class byte_array;

  void base64_encode(const unsigned char* src,
                     unsigned int src_len,
                     rutz::byte_array& dst,
                     unsigned int line_width = 0);

  void base64_encode_string(const char* str,
                            rutz::byte_array& dst,
                            unsigned int line_width = 0);

  void base64_encode_file(const char* src_filename,
                          rutz::byte_array& dst,
                          unsigned int line_width = 0);

  void base64_decode(const char* src,
                     unsigned int in_len,
                     rutz::byte_array& dst);

  void base64_decode(const rutz::byte_array& src,
                     rutz::byte_array& dst);
}

static const char vcid_groovx_rutz_base64_h_utc20050626084019[] = "$Id$ $HeadURL$";
#endif // !GROOVX_RUTZ_BASE64_H_UTC20050626084019_DEFINED
