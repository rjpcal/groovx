///////////////////////////////////////////////////////////////////////
//
// reader.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun  7 12:47:00 1999
// written: Tue Oct 31 11:51:16 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef READER_CC_DEFINED
#define READER_CC_DEFINED

#include "io/reader.h"

#include "util/strings.h"

#include <cstddef>

IO::ReadError::ReadError(const char* msg) :
  ErrorWithMsg("IO::ReadError: ")
{
  appendMsg(msg);
}

IO::ReadError::~ReadError() {}

IO::ReadVersionError::ReadVersionError(const char* classname,
													IO::VersionId attempted_id,
													IO::VersionId lowest_supported_id,
													const char* msg) :
  ErrorWithMsg("IO::ReadVersionError: ")
{
  appendMsg("in ", classname, ", serial version ");
  appendNumber(int(attempted_id));
  appendMsg(" is not supported. The lowest supported version is ");
  appendNumber(int(lowest_supported_id));
  appendMsg(". ", msg);
}

IO::ReadVersionError::~ReadVersionError() {}

IO::Reader::~Reader() {}

template <>
void IO::Reader::readValue<char>(const char* name, char& return_value) {
  return_value = readChar(name);
}

template <>
void IO::Reader::readValue<int>(const char* name, int& return_value) {
  return_value = readInt(name);
}

template <>
void IO::Reader::readValue<size_t>(const char* name, size_t& return_value) {
  return_value = readInt(name);
}

template <>
void IO::Reader::readValue<bool>(const char* name, bool& return_value) {
  return_value = readBool(name);
}

template <>
void IO::Reader::readValue<double>(const char* name, double& return_value) {
  return_value = readDouble(name);
}

template <>
void IO::Reader::readValue<fixed_string>(const char* name,
												 fixed_string& return_value) {
  char* temp = readCstring(name);
  return_value = temp;
  delete [] temp;
}

template <>
void IO::Reader::readValue<dynamic_string>(const char* name,
													dynamic_string& return_value) {
  char* temp = readCstring(name);
  return_value = temp;
  delete [] temp;
}

template <>
void IO::Reader::readValue<Value>(const char* name, Value& return_value) {
  readValueObj(name, return_value);
}

static const char vcid_reader_cc[] = "$Header$";
#endif // !READER_CC_DEFINED
