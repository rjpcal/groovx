///////////////////////////////////////////////////////////////////////
//
// writer.cc
//
// Copyright (c) 1999-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Jun  7 12:49:50 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef WRITER_CC_DEFINED
#define WRITER_CC_DEFINED

#include "io/writer.h"

#include "util/error.h"
#include "util/strings.h"

#include "util/debug.h"
DBG_REGISTER

namespace IO
{
  class WriteVersionError : public Util::Error
  {
  public:
    /// Construct with information relevant to the problem
    WriteVersionError(const char* classname,
                      IO::VersionId attempted_id,
                      IO::VersionId lowest_supported_id,
                      const char* msg);

    virtual ~WriteVersionError() throw();
  };
}

IO::WriteVersionError::WriteVersionError(const char* classname,
                                         IO::VersionId attempted_id,
                                         IO::VersionId lowest_supported_id,
                                         const char* info) :
  Util::Error()
{
  msg().append("IO::WriteVersionError: ");
  msg().append("in ", classname, ", serial version ");
  msg().append(attempted_id, " is not supported. The lowest supported version is ");
  msg().append(lowest_supported_id, ". ", info);
}

IO::WriteVersionError::~WriteVersionError() throw() {}

IO::Writer::~Writer () throw() {}

int IO::Writer::ensureWriteVersionId(const char* name,
                                     IO::VersionId actual_version,
                                     IO::VersionId lowest_supported_version,
                                     const char* msg)
{
  if (actual_version < lowest_supported_version)
    throw IO::WriteVersionError(name, actual_version,
                                lowest_supported_version, msg);

  Assert(actual_version >= lowest_supported_version);

  return actual_version;
}

static const char vcid_writer_cc[] = "$Header$";
#endif // !WRITER_CC_DEFINED
