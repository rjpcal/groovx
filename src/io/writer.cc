///////////////////////////////////////////////////////////////////////
//
// writer.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun  7 12:49:50 1999
// written: Sat Sep 23 16:33:03 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef WRITER_CC_DEFINED
#define WRITER_CC_DEFINED

#include "io/writer.h"

#include "util/strings.h"

#include <string>
#include <strstream.h>

IO::WriteError::WriteError(const char* msg) :
  ErrorWithMsg("IO::WriteError: ")
{
  appendMsg(msg);
}

IO::WriteError::~WriteError() {}

IO::Writer::~Writer () {}

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
void IO::Writer::writeValue<std::string>(const char* name, const std::string& val) {
  writeCstring(name, val.c_str());
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
void IO::Writer::writeValue<const char*>(const char* name,
												 const char* const& val) {
  writeCstring(name, val);
}

template<>
void IO::Writer::writeValue<Value>(const char* name,
										  const Value& value) {
  writeValueObj(name, value);
}

static const char vcid_writer_cc[] = "$Header$";
#endif // !WRITER_CC_DEFINED
