///////////////////////////////////////////////////////////////////////
//
// writer.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jun  7 12:49:50 1999
// written: Wed Aug  8 20:16:39 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef WRITER_CC_DEFINED
#define WRITER_CC_DEFINED

#include "io/writer.h"

#include "util/strings.h"

#define LOCAL_ASSERT
#include "util/debug.h"

IO::WriteError::~WriteError() {}

IO::WriteVersionError::WriteVersionError(const char* classname,
                                         IO::VersionId attempted_id,
                                         IO::VersionId lowest_supported_id,
                                         const char* msg) :
  Util::Error()
{
  append("IO::WriteVersionError: ");
  append("in ", classname, ", serial version ");
  append(attempted_id, " is not supported. The lowest supported version is ");
  append(lowest_supported_id, ". ", msg);
}

IO::WriteVersionError::~WriteVersionError() {}

IO::Writer::~Writer () {}

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

template<>
void IO::Writer::writeValue<char>(const char* name, const char& val)
{
  writeChar(name, val);
}

template<>
void IO::Writer::writeValue<int>(const char* name, const int& val)
{
  writeInt(name, val);
}

template<>
void IO::Writer::writeValue<size_t>(const char* name, const size_t& val)
{
  writeInt(name, val);
}

template<>
void IO::Writer::writeValue<bool>(const char* name, const bool& val)
{
  writeBool(name, val);
}

template<>
void IO::Writer::writeValue<double>(const char* name, const double& val)
{
  writeDouble(name, val);
}


template<>
void IO::Writer::writeValue<fstring>(const char* name,
                                     const fstring& val)
{
  writeCstring(name, val.c_str());
}


template<>
void IO::Writer::writeValue<Value>(const char* name,
                                   const Value& value)
{
  writeValueObj(name, value);
}

static const char vcid_writer_cc[] = "$Header$";
#endif // !WRITER_CC_DEFINED
