///////////////////////////////////////////////////////////////////////
//
// writer.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jun  7 12:49:50 1999
// written: Mon Jan 13 11:04:47 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef WRITER_CC_DEFINED
#define WRITER_CC_DEFINED

#include "io/writer.h"

#include "util/strings.h"

#include "util/debug.h"

IO::WriteError::~WriteError() {}

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
void IO::Writer::writeValue<unsigned int>(const char* name,
                                          const unsigned int& val)
{
  writeInt(name, val);
}

template<>
void IO::Writer::writeValue<unsigned long>(const char* name,
                                           const unsigned long& val)
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
