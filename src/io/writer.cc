///////////////////////////////////////////////////////////////////////
//
// writer.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jun  7 12:49:50 1999
// written: Tue Nov 14 14:50:14 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef WRITER_CC_DEFINED
#define WRITER_CC_DEFINED

#include "io/writer.h"

#include "util/strings.h"

#include <strstream.h>

#define LOCAL_ASSERT
#include "util/debug.h"

IO::WriteError::WriteError(const char* msg) :
  ErrorWithMsg("IO::WriteError: ")
{
  appendMsg(msg);
}

IO::WriteError::~WriteError() {}

IO::WriteVersionError::WriteVersionError(const char* classname,
													  IO::VersionId attempted_id,
													  IO::VersionId lowest_supported_id,
													  const char* msg) :
  ErrorWithMsg("IO::WriteVersionError: ")
{
  appendMsg("in ", classname, ", serial version ");
  appendNumber(int(attempted_id));
  appendMsg(" is not supported. The lowest supported version is ");
  appendNumber(int(lowest_supported_id));
  appendMsg(". ", msg);
}

IO::WriteVersionError::~WriteVersionError() {}

IO::Writer::~Writer () {}

void IO::Writer::ensureWriteVersionId(const char* name,
												  IO::VersionId actual_version,
												  IO::VersionId lowest_supported_version,
												  const char* msg) {

  if (actual_version < lowest_supported_version)
	 throw IO::WriteVersionError(name, actual_version,
										  lowest_supported_version, msg);

  Assert(actual_version >= lowest_supported_version);
}

template<>
void IO::Writer::writeValue<char>(const char* name, const char& val) {
  writeChar(name, val);
}

template<>
void IO::Writer::writeValue<int>(const char* name, const int& val) {
  writeInt(name, val);
}

template<>
void IO::Writer::writeValue<size_t>(const char* name, const size_t& val) {
  writeInt(name, val);
}

template<>
void IO::Writer::writeValue<bool>(const char* name, const bool& val) {
  writeBool(name, val);
}

template<>
void IO::Writer::writeValue<double>(const char* name, const double& val) {
  writeDouble(name, val);
}


template<>
void IO::Writer::writeValue<fixed_string>(const char* name,
												  const fixed_string& val) {
  writeCstring(name, val.c_str());
}


template<>
void IO::Writer::writeValue<dynamic_string>(const char* name,
													 const dynamic_string& val) {
  writeCstring(name, val.c_str());
}

template<>
void IO::Writer::writeValue<Value>(const char* name,
										  const Value& value) {
  writeValueObj(name, value);
}

static const char vcid_writer_cc[] = "$Header$";
#endif // !WRITER_CC_DEFINED
