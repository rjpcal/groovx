///////////////////////////////////////////////////////////////////////
//
// io.cc
// Rob Peters
// created: Tue Mar  9 20:25:02 1999
// written: Thu Mar 30 12:29:20 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IO_CC_DEFINED
#define IO_CC_DEFINED

#include "io/io.h"

#include <cctype>
#include <cstring>
#include <string>
#include <strstream.h>
#include <typeinfo>

#include "system/demangle.h"

#include "util/strings.h"

#define NO_TRACE
#include "util/trace.h"
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// File scope declarations
//
///////////////////////////////////////////////////////////////////////

namespace {
  unsigned long idCounter = 0;
}

///////////////////////////////////////////////////////////////////////
//
// IO member definitions
//
///////////////////////////////////////////////////////////////////////

// // These members are declared and initialized within the class, but
// // they still must be given a unique *definition* here.
// const IO::IOFlag IO::NO_FLAGS;
// const IO::IOFlag IO::TYPENAME;
// const IO::IOFlag IO::BASES;

// const char IO::SEP;

IO::IoObject::IoObject() : itsId(++idCounter) {
DOTRACE("IO::IoObject::IoObject");
  DebugEvalNL(itsId);
}

// Must be defined out of line to avoid duplication of IO's vtable
IO::IoObject::~IoObject() {
DOTRACE("IO::IoObject::~IoObject");
}

void IO::IoObject::serialize(ostream&, IO::IOFlag) const {}
void IO::IoObject::deserialize(istream&, IO::IOFlag) {}
int IO::IoObject::charCount() const { return 0; }

unsigned long IO::IoObject::id() const {
DOTRACE("IO::IoObject::id");
  return itsId;
}

unsigned long IO::IoObject::serialVersionId() const {
DOTRACE("IO::IoObject::serialVersionId");
  return 0; 
}

fixed_string IO::IoObject::ioTypename() const {
DOTRACE("IO::IoObject::ioTypename");
  return demangle_cstr(typeid(*this).name());
}

int IO::IoObject::eatWhitespace(istream& is) {
DOTRACE("IO::IoObject::eatWhitespace");
  int c=0;
  while ( isspace(is.peek()) )
	 { is.get(); ++c; }
  return c;
}

void IO::IoObject::readTypename(istream& is, const char* correctNames_cstr,
							 bool doCheck) {
DOTRACE("IO::IoObject::readTypename");
  string correctNames = correctNames_cstr;

  string name;
  is >> name;

  DebugEval(name); DebugPrintNL("");
  DebugEvalNL(is.peek());

  // If we aren't checking, then we can skip the rest of the routine.
  if (!doCheck) return;
  
  DebugEval(correctNames);
  DebugEvalNL(doCheck);

  string candidate;

  string first_candidate = correctNames.substr(0, correctNames.find(' ', 0));

  string::size_type end_pos = 0;
  string::size_type beg_pos = 0;

  // Loop over the space-separated substrings of correctNames.
  while ( end_pos < string::npos ) {
	 end_pos = correctNames.find(' ', beg_pos);

	 candidate = correctNames.substr(beg_pos, end_pos);

	 DebugEval(name);
	 DebugEval(candidate);
	 DebugEval(beg_pos);
	 DebugEvalNL(end_pos);
	 
	 if (name == candidate) {
		DebugEval(is.peek());
		DebugPrintNL("found a name match");
		return;
	 }

	 // Skip ahead until we find the first non-space character. This
	 // character will mark the beginning of the next candidate
	 // typename.
	 beg_pos = correctNames.find_first_not_of(' ', end_pos+1);
  }

  // If we got here, then none of the substrings matched so we must
  // raise an exception.
  IO::InputError err("couldn't read typename for ");
  err.appendMsg(first_candidate.c_str());
  throw err;
}

///////////////////////////////////////////////////////////////////////
//
// Global charCount functions
//
///////////////////////////////////////////////////////////////////////

namespace IO {

template<class T>
int gCharCount(T val) {
  static const int BUF_SIZE = 64;
  static char buf[BUF_SIZE];
  ostrstream ost(buf, BUF_SIZE);
  ost << val << '\0';
  return strlen(buf);
}

// Explicit instantiations
template int gCharCount<int>(int val);
template int gCharCount<bool>(bool val);
template int gCharCount<char>(char val);
template int gCharCount<double>(double val);

// Specializations for string types
template <> int gCharCount(const char* val)   { return strlen(val); }
template <> int gCharCount(char* val)         { return strlen(val); }
template <> int gCharCount(string val)        { return val.length(); }
template <> int gCharCount(const string& val) { return val.length(); }

} // end namespace IO

///////////////////////////////////////////////////////////////////////
//
// IoError member function definitions
//
///////////////////////////////////////////////////////////////////////

IO::IoError::IoError() :
  ErrorWithMsg()
{
DOTRACE("IO::IoError::IoError");
}

IO::IoError::IoError(const char* str) :
  ErrorWithMsg(demangle_cstr(typeid(*this).name()))
{
DOTRACE("IO::IoError::IoError");
  appendMsg(": ");
  appendMsg(str);
}

IO::IoError::IoError(const type_info& ti) :
  ErrorWithMsg(demangle_cstr(typeid(*this).name()))
{
DOTRACE("IO::IoError::IoError");
  appendMsg(": ");
  appendMsg(demangle_cstr(ti.name()));
}

void IO::IoError::setMsg(const char* str) {
DOTRACE("IO::IoError::setMsg(const char*)");
  ErrorWithMsg::setMsg(demangle_cstr(typeid(*this).name()));
  ErrorWithMsg::appendMsg(": ");
  ErrorWithMsg::appendMsg(str);
}

void IO::IoError::setMsg(const type_info& ti) {
DOTRACE("IO::IoError::setMsg(const type_info&)");
  IoError::setMsg(demangle_cstr(ti.name()));
}

IO::InputError::InputError(const char* str) {
DOTRACE("IO::InputError::InputError");
  setMsg(str);
}

IO::OutputError::OutputError(const char* str) {
DOTRACE("IO::OutputError::OutputError");
  setMsg(str);
}

IO::LogicError::LogicError(const char* str) {
DOTRACE("IO::LogicError::LogicError");
  setMsg(str);
}

IO::ValueError::ValueError(const char* str) {
DOTRACE("IO::ValueError::ValueError");
  setMsg(str);
}

IO::FilenameError::FilenameError(const char* str) {
DOTRACE("IO::FilenameError::FilenameError");
  setMsg(str);
}

static const char vcid_io_cc[] = "$Header$";
#endif // !IO_CC_DEFINED
