/** @file io/writer.cc abstract base class for writing io::serializable
    objects */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Jun  7 12:49:50 1999
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

#ifndef GROOVX_IO_WRITER_CC_UTC20050626084021_DEFINED
#define GROOVX_IO_WRITER_CC_UTC20050626084021_DEFINED

#include "io/writer.h"

#include "rutz/base64.h"
#include "rutz/bytearray.h" // for use with rutz::base64_encode()
#include "rutz/error.h"
#include "rutz/fstring.h"
#include "rutz/sfmt.h"

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

namespace io
{
  class write_version_error : public rutz::error
  {
  public:
    /// Construct with information relevant to the problem
    write_version_error(const char* classname,
                      io::version_id attempted_id,
                      io::version_id lowest_supported_id,
                      const char* msg,
                      const rutz::file_pos& pos);

    virtual ~write_version_error() throw();
  };
}

io::write_version_error::write_version_error(const char* classname,
                                         io::version_id attempted_id,
                                         io::version_id lowest_supported_id,
                                         const char* info,
                                         const rutz::file_pos& pos) :
  rutz::error(pos)
{
  const rutz::fstring m =
    rutz::sfmt("io::write_version_error: in %s, serial version %ld "
               "is not supported. The lowest supported version "
               "is %ld. %s",
               classname, attempted_id, lowest_supported_id, info);
  this->set_msg(m);
}

io::write_version_error::~write_version_error() throw() {}

io::writer::~writer () throw() {}

int io::writer::ensure_output_version_id(const char* name,
                                     io::version_id actual_version,
                                     io::version_id lowest_supported_version,
                                     const char* msg,
                                     const rutz::file_pos& pos)
{
  if (actual_version < lowest_supported_version)
    throw io::write_version_error(name, actual_version,
                                lowest_supported_version, msg, pos);

  GVX_ASSERT(actual_version >= lowest_supported_version);

  return actual_version;
}

void io::writer::default_write_byte_array(const char* name,
                                          const unsigned char* data,
                                          unsigned int length)
{
GVX_TRACE("io::writer::default_write_byte_array");

  rutz::byte_array encoded;

  rutz::base64_encode(data, length, encoded, 60);

  encoded.vec.push_back('\0');

  write_cstring(name, reinterpret_cast<char*>(&encoded.vec[0]));
}

#endif // !GROOVX_IO_WRITER_CC_UTC20050626084021_DEFINED
