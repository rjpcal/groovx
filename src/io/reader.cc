///////////////////////////////////////////////////////////////////////
//
// reader.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun  7 12:47:00 1999
// written: Tue Feb 15 10:42:55 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef READER_CC_DEFINED
#define READER_CC_DEFINED

#include "reader.h"

#include <strstream.h>

Reader::~Reader() {}

template <>
void Reader::readValue<char>(const string& name, char& return_value) {
  return_value = readChar(name);
}

template <>
void Reader::readValue<int>(const string& name, int& return_value) {
  return_value = readInt(name);
}

template <>
void Reader::readValue<size_t>(const string& name, size_t& return_value) {
  return_value = readInt(name);
}

template <>
void Reader::readValue<bool>(const string& name, bool& return_value) {
  return_value = readBool(name);
}

template <>
void Reader::readValue<double>(const string& name, double& return_value) {
  return_value = readDouble(name);
}

template <>
void Reader::readValue<string>(const string& name, string& return_value) {
  return_value = readString(name);
}

template <>
void Reader::readValue<const char*>(const string& name, const char*& return_value) {
  return_value = readCstring(name);
}

template <>
void Reader::readValue<Value>(const string& name, Value& return_value) {
  readValueObj(name, return_value);
}

static const char vcid_reader_cc[] = "$Header$";
#endif // !READER_CC_DEFINED
