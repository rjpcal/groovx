///////////////////////////////////////////////////////////////////////
//
// reader.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun  7 12:47:00 1999
// written: Wed Mar  8 17:55:16 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef READER_CC_DEFINED
#define READER_CC_DEFINED

#include "reader.h"

#include "util/strings.h"

#include <string>

Reader::~Reader() {}

template <>
void Reader::readValue<char>(const char* name, char& return_value) {
  return_value = readChar(name);
}

template <>
void Reader::readValue<int>(const char* name, int& return_value) {
  return_value = readInt(name);
}

template <>
void Reader::readValue<size_t>(const char* name, size_t& return_value) {
  return_value = readInt(name);
}

template <>
void Reader::readValue<bool>(const char* name, bool& return_value) {
  return_value = readBool(name);
}

template <>
void Reader::readValue<double>(const char* name, double& return_value) {
  return_value = readDouble(name);
}

template <>
void Reader::readValue<string>(const char* name, string& return_value) {
  char* temp = readCstring(name);
  return_value = temp;
  delete [] temp;
}

template <>
void Reader::readValue<fixed_string>(const char* name,
												 fixed_string& return_value) {
  char* temp = readCstring(name);
  return_value = temp;
  delete [] temp;
}

template <>
void Reader::readValue<dynamic_string>(const char* name,
													dynamic_string& return_value) {
  char* temp = readCstring(name);
  return_value = temp;
  delete [] temp;
}

template <>
void Reader::readValue<const char*>(const char* name, const char*& return_value) {
  return_value = readCstring(name);
}

template <>
void Reader::readValue<Value>(const char* name, Value& return_value) {
  readValueObj(name, return_value);
}

static const char vcid_reader_cc[] = "$Header$";
#endif // !READER_CC_DEFINED
