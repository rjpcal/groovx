///////////////////////////////////////////////////////////////////////
//
// reader.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jun  7 12:47:00 1999
// written: Wed Aug  8 18:57:56 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef READER_CC_DEFINED
#define READER_CC_DEFINED

#include "io/reader.h"

#include "io/io.h"

#include "util/ref.h"

#include <cstddef>

#define LOCAL_ASSERT
#include "util/debug.h"

IO::ReadError::~ReadError() {}

IO::ReadVersionError::ReadVersionError(const char* classname,
                                       IO::VersionId attempted_id,
                                       IO::VersionId lowest_supported_id,
                                       const char* msg) :
  Util::Error()
{
  append("IO::ReadVersionError: ");
  append("in ", classname, ", serial version ");
  append(attempted_id, " is not supported. The lowest supported version is ");
  append(lowest_supported_id, ". ", msg);
}

IO::ReadVersionError::~ReadVersionError() {}

IO::Reader::~Reader() {}

int IO::Reader::ensureReadVersionId(const char* name,
                                    IO::VersionId lowest_supported_version,
                                    const char* msg) {
  IO::VersionId actual_version = this->readSerialVersionId();

  if (actual_version < lowest_supported_version)
    throw IO::ReadVersionError(name, actual_version,
                               lowest_supported_version, msg);

  Assert(actual_version >= lowest_supported_version);

  return actual_version;
}

template <>
void IO::Reader::readValue<char>(const fixed_string& name, char& return_value) {
  return_value = readChar(name);
}

template <>
void IO::Reader::readValue<int>(const fixed_string& name, int& return_value) {
  return_value = readInt(name);
}

template <>
void IO::Reader::readValue<size_t>(const fixed_string& name,
                                   size_t& return_value) {
  return_value = readInt(name);
}

template <>
void IO::Reader::readValue<bool>(const fixed_string& name,
                                 bool& return_value) {
  return_value = readBool(name);
}

template <>
void IO::Reader::readValue<double>(const fixed_string& name,
                                   double& return_value) {
  return_value = readDouble(name);
}

template <>
void IO::Reader::readValue<fixed_string>(const fixed_string& name,
                                         fixed_string& return_value) {
  return_value = readStringImpl(name);
}

template <>
void IO::Reader::readValue<Value>(const fixed_string& name,
                                  Value& return_value) {
  readValueObj(name, return_value);
}

static const char vcid_reader_cc[] = "$Header$";
#endif // !READER_CC_DEFINED
