///////////////////////////////////////////////////////////////////////
//
// reader.cc
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Jun  7 12:47:00 1999
// commit: $Id$
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

#ifndef READER_CC_DEFINED
#define READER_CC_DEFINED

#include "io/reader.h"

#include "util/base64.h"
#include "util/bytearray.h"
#include "util/error.h"

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

namespace IO
{
  class ReadVersionError : public rutz::error
  {
  public:
    /// Construct with information relevant to the problem
    ReadVersionError(const char* classname,
                     IO::VersionId attempted_id,
                     IO::VersionId lowest_supported_id,
                     const char* msg,
                     const rutz::file_pos& pos);

    virtual ~ReadVersionError() throw();
  };
}

IO::ReadVersionError::ReadVersionError(const char* classname,
                                       IO::VersionId attempted_id,
                                       IO::VersionId lowest_supported_id,
                                       const char* info,
                                       const rutz::file_pos& pos) :
  rutz::error(pos)
{
  rutz::fstring m;
  m.append("IO::ReadVersionError: in ", classname,
           ", serial version ", attempted_id, " is not supported. ");
  m.append("The lowest supported version is ", lowest_supported_id,
           ". ", info);
  this->set_msg(m);
}

IO::ReadVersionError::~ReadVersionError() throw() {}

IO::Reader::~Reader() throw() {}

int IO::Reader::ensureReadVersionId(const char* name,
                                    IO::VersionId lowest_supported_version,
                                    const char* msg,
                                    const rutz::file_pos& pos)
{
DOTRACE("IO::Reader::ensureReadVersionId");

  IO::VersionId actual_version = this->readSerialVersionId();

  if (actual_version < lowest_supported_version)
    throw IO::ReadVersionError(name, actual_version,
                               lowest_supported_version, msg,
                               pos);

  ASSERT(actual_version >= lowest_supported_version);

  return actual_version;
}

void IO::Reader::defaultReadRawData(const rutz::fstring& name,
                                    rutz::byte_array& data)
{
DOTRACE("IO::Reader::defaultReadRawData");

  rutz::fstring encoded = readStringImpl(name);
  rutz::base64_decode(encoded.c_str(), encoded.length(),
                      data);
}

static const char vcid_reader_cc[] = "$Header$";
#endif // !READER_CC_DEFINED
