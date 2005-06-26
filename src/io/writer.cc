///////////////////////////////////////////////////////////////////////
//
// writer.cc
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Jun  7 12:49:50 1999
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

#ifndef GROOVX_IO_WRITER_CC_UTC20050626084021_DEFINED
#define GROOVX_IO_WRITER_CC_UTC20050626084021_DEFINED

#include "io/writer.h"

#include "rutz/base64.h"
#include "rutz/bytearray.h" // for use with rutz::base64_encode()
#include "rutz/error.h"
#include "rutz/fstring.h"

#include "rutz/trace.h"
#include "rutz/debug.h"
DBG_REGISTER

namespace IO
{
  class WriteVersionError : public rutz::error
  {
  public:
    /// Construct with information relevant to the problem
    WriteVersionError(const char* classname,
                      IO::VersionId attempted_id,
                      IO::VersionId lowest_supported_id,
                      const char* msg,
                      const rutz::file_pos& pos);

    virtual ~WriteVersionError() throw();
  };
}

IO::WriteVersionError::WriteVersionError(const char* classname,
                                         IO::VersionId attempted_id,
                                         IO::VersionId lowest_supported_id,
                                         const char* info,
                                         const rutz::file_pos& pos) :
  rutz::error(pos)
{
  rutz::fstring m;
  m.append("IO::WriteVersionError: in ", classname,
           ", serial version ", attempted_id, " is not supported. ");
  m.append("The lowest supported version is ", lowest_supported_id,
           ". ", info);
  this->set_msg(m);
}

IO::WriteVersionError::~WriteVersionError() throw() {}

IO::Writer::~Writer () throw() {}

int IO::Writer::ensureWriteVersionId(const char* name,
                                     IO::VersionId actual_version,
                                     IO::VersionId lowest_supported_version,
                                     const char* msg,
                                     const rutz::file_pos& pos)
{
  if (actual_version < lowest_supported_version)
    throw IO::WriteVersionError(name, actual_version,
                                lowest_supported_version, msg, pos);

  ASSERT(actual_version >= lowest_supported_version);

  return actual_version;
}

void IO::Writer::defaultWriteRawData(const char* name,
                                     const unsigned char* data,
                                     unsigned int length)
{
DOTRACE("IO::Writer::defaultWriteRawData");

  rutz::byte_array encoded;

  rutz::base64_encode(data, length, encoded, 60);

  encoded.vec.push_back('\0');

  writeCstring(name, reinterpret_cast<char*>(&encoded.vec[0]));
}

static const char vcid_groovx_io_writer_cc_utc20050626084021[] = "$Id$ $HeadURL$";
#endif // !GROOVX_IO_WRITER_CC_UTC20050626084021_DEFINED
