///////////////////////////////////////////////////////////////////////
//
// reader.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Jun  7 12:47:00 1999
// written: Wed Mar 19 17:55:54 2003
// $Id$
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

#ifndef READER_CC_DEFINED
#define READER_CC_DEFINED

#include "io/reader.h"

#include "util/trace.h"
#include "util/debug.h"

IO::ReadError::~ReadError() {}

IO::ReadVersionError::ReadVersionError(const char* classname,
                                       IO::VersionId attempted_id,
                                       IO::VersionId lowest_supported_id,
                                       const char* info) :
  Util::Error()
{
  msg().append("IO::ReadVersionError: ");
  msg().append("in ", classname, ", serial version ");
  msg().append(attempted_id, " is not supported. The lowest supported version is ");
  msg().append(lowest_supported_id, ". ", info);
}

IO::ReadVersionError::~ReadVersionError() {}

IO::Reader::~Reader() {}

int IO::Reader::ensureReadVersionId(const char* name,
                                    IO::VersionId lowest_supported_version,
                                    const char* msg)
{
DOTRACE("IO::Reader::ensureReadVersionId");

  IO::VersionId actual_version = this->readSerialVersionId();

  if (actual_version < lowest_supported_version)
    throw IO::ReadVersionError(name, actual_version,
                               lowest_supported_version, msg);

  Assert(actual_version >= lowest_supported_version);

  return actual_version;
}

template <>
void IO::Reader::readValue<char>(const fstring& name, char& return_value)
{
  return_value = readChar(name);
}

template <>
void IO::Reader::readValue<int>(const fstring& name, int& return_value)
{
  return_value = readInt(name);
}

template <>
void IO::Reader::readValue<unsigned int>(const fstring& name,
                                         unsigned int& return_value)
{
  return_value = readInt(name);
}

template <>
void IO::Reader::readValue<unsigned long>(const fstring& name,
                                          unsigned long& return_value)
{
  return_value = readInt(name);
}

template <>
void IO::Reader::readValue<bool>(const fstring& name,
                                 bool& return_value)
{
  return_value = readBool(name);
}

template <>
void IO::Reader::readValue<double>(const fstring& name,
                                   double& return_value)
{
  return_value = readDouble(name);
}

template <>
void IO::Reader::readValue<fstring>(const fstring& name,
                                    fstring& return_value)
{
  return_value = readStringImpl(name);
}

template <>
void IO::Reader::readValue<Value>(const fstring& name,
                                  Value& return_value)
{
  readValueObj(name, return_value);
}

static const char vcid_reader_cc[] = "$Header$";
#endif // !READER_CC_DEFINED
