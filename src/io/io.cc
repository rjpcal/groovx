///////////////////////////////////////////////////////////////////////
//
// io.cc
// Rob Peters
// created: Tue Mar  9 20:25:02 1999
// written: Sat Mar  4 04:37:43 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IO_CC_DEFINED
#define IO_CC_DEFINED

#include "io.h"

#include <cctype>
#include <cstring>
#include <typeinfo>
#include <strstream.h>
#include <vector>

#include "demangle.h"

#define NO_TRACE
#include "trace.h"
#include "debug.h"

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

// These members are declared and initialized within the class, but
// they still must be given a unique *definition* here.
const IO::IOFlag IO::NO_FLAGS;
const IO::IOFlag IO::TYPENAME;
const IO::IOFlag IO::BASES;

const char IO::SEP;

IO::IO() : itsId(++idCounter) {
DOTRACE("IO::IO");
  DebugEvalNL(itsId);
}

// Must be defined out of line to avoid duplication of IO's vtable
IO::~IO() {
DOTRACE("IO::~IO");
}

void IO::serialize(ostream&, IOFlag) const {}
void IO::deserialize(istream&, IOFlag) {}
int IO::charCount() const { return 0; }

unsigned long IO::id() const {
DOTRACE("IO::id");
 return itsId;
}

string IO::ioTypename() const {
DOTRACE("IO::ioTypename");
  return demangle(typeid(*this).name());
}

int IO::eatWhitespace(istream& is) {
DOTRACE("IO::eatWhitespace");
  int c=0;
  while ( isspace(is.peek()) )
	 { is.get(); ++c; }
  return c;
}

void IO::readTypename(istream& is, const string& correctNames, bool doCheck) {
DOTRACE("IO::readTypename");
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
  throw InputError(first_candidate + " typename");
}

///////////////////////////////////////////////////////////////////////
//
// Global charCount functions
//
///////////////////////////////////////////////////////////////////////

template<class T>
int gCharCount(T val) {
  static const int BUF_SIZE = 64;
  static char buf[BUF_SIZE];
  ostrstream ost(buf, BUF_SIZE);
  ost << val << '\0';
  return strlen(buf);
}

template int gCharCount<int>(int val);
template int gCharCount<bool>(bool val);
template int gCharCount<char>(char val);
template int gCharCount<double>(double val);

template <>
int gCharCount(const char* val) {
  return strlen(val);
}

template <>
int gCharCount(char* val) {
  return strlen(val);
}

template <>
int gCharCount(string val) {
  return val.length();
}

template <>
int gCharCount(const string& val) {
  return val.length();
}

///////////////////////////////////////////////////////////////////////
//
// IoError member function definitions
//
///////////////////////////////////////////////////////////////////////

IoError::IoError() :
  ErrorWithMsg()
{
DOTRACE("IoError::IoError");
}

IoError::IoError(const string& str) :
  ErrorWithMsg(demangle(typeid(*this).name()) + ": " + str)
{
DOTRACE("IoError::IoError(const string&)");
}

IoError::IoError(const type_info& ti) :
  ErrorWithMsg(demangle(typeid(*this).name()) + ": " +
					demangle(ti.name()))
{
DOTRACE("IoError::IoError(const type_info&)");
}

void IoError::setMsg(const string& str) {
DOTRACE("IoError::setMsg(const string&)");
  ErrorWithMsg::setMsg(demangle(typeid(*this).name()) + ": " + str);
}

void IoError::setMsg(const type_info& ti) {
DOTRACE("IoError::setMsg(const type_info&)");
  ErrorWithMsg::setMsg(demangle(typeid(*this).name()) + ": " +
							  demangle(ti.name()));
}

static const char vcid_io_cc[] = "$Header$";
#endif // !IO_CC_DEFINED
