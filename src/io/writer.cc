///////////////////////////////////////////////////////////////////////
//
// writer.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun  7 12:49:50 1999
// written: Tue Oct 19 16:02:37 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef WRITER_CC_DEFINED
#define WRITER_CC_DEFINED

#include "writer.h"

#include <strstream.h>

Writer::~Writer () {}

template<>
void Writer::writeValue<char>(const string& name, const char& val) {
  writeChar(name, val);
}

template<>
void Writer::writeValue<int>(const string& name, const int& val) {
  writeInt(name, val);
}

template<>
void Writer::writeValue<bool>(const string& name, const bool& val) {
  writeBool(name, val);
}

template<>
void Writer::writeValue<double>(const string& name, const double& val) {
  writeDouble(name, val);
}

template<>
void Writer::writeValue<string>(const string& name, const string& val) {
  writeString(name, val);
}

template<>
void Writer::writeValue<const char*>(const string& name,
												 const char* const& val) {
  writeCstring(name, val);
}

template<>
void Writer::writeValue<Value>(const string& name,
										  const Value& value) {
  writeValueObj(name, value);
}

string Writer::makeNumberString(int number) {
  char buf[16];
  ostrstream ost(buf, 16);
  ost << number << '\0';
  return buf;
}

static const char vcid_writer_cc[] = "$Header$";
#endif // !WRITER_CC_DEFINED
