///////////////////////////////////////////////////////////////////////
//
// writer.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun  7 12:49:50 1999
// written: Thu Mar 30 00:02:12 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef WRITER_CC_DEFINED
#define WRITER_CC_DEFINED

#include "io/writer.h"

#include "util/strings.h"

#include <string>
#include <strstream.h>

WriteError::WriteError(const char* msg) :
  ErrorWithMsg("WriteError: ")
{
  appendMsg(msg);
}

Writer::~Writer () {}

template<>
void Writer::writeValue<char>(const char* name, const char& val) {
  writeChar(name, val);
}

template<>
void Writer::writeValue<int>(const char* name, const int& val) {
  writeInt(name, val);
}

template<>
void Writer::writeValue<size_t>(const char* name, const size_t& val) {
  writeInt(name, val);
}

template<>
void Writer::writeValue<bool>(const char* name, const bool& val) {
  writeBool(name, val);
}

template<>
void Writer::writeValue<double>(const char* name, const double& val) {
  writeDouble(name, val);
}

template<>
void Writer::writeValue<string>(const char* name, const string& val) {
  writeCstring(name, val.c_str());
}


template<>
void Writer::writeValue<fixed_string>(const char* name,
												  const fixed_string& val) {
  writeCstring(name, val.c_str());
}


template<>
void Writer::writeValue<dynamic_string>(const char* name,
													 const dynamic_string& val) {
  writeCstring(name, val.c_str());
}

template<>
void Writer::writeValue<const char*>(const char* name,
												 const char* const& val) {
  writeCstring(name, val);
}

template<>
void Writer::writeValue<Value>(const char* name,
										  const Value& value) {
  writeValueObj(name, value);
}

static const char vcid_writer_cc[] = "$Header$";
#endif // !WRITER_CC_DEFINED
