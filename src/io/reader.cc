///////////////////////////////////////////////////////////////////////
//
// reader.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun  7 12:47:00 1999
// written: Thu Mar 30 09:47:06 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef READER_CC_DEFINED
#define READER_CC_DEFINED

#include "io/reader.h"

#include "util/strings.h"

#include <string>

IO::ReadError::ReadError(const char* msg) :
  ErrorWithMsg("IO::ReadError: ")
{
  appendMsg(msg);
}

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
void IO::Reader::readValue<string>(const char* name, string& return_value) {
  char* temp = readCstring(name);
  return_value = temp;
  delete [] temp;
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
void IO::Reader::readValue<const char*>(const char* name, const char*& return_value) {
  return_value = readCstring(name);
}

template <>
void IO::Reader::readValue<Value>(const char* name, Value& return_value) {
  readValueObj(name, return_value);
}

static const char vcid_reader_cc[] = "$Header$";
#endif // !READER_CC_DEFINED
