///////////////////////////////////////////////////////////////////////
//
// Io.cc
// Rob Peters
// created: Tue Mar  9 20:25:02 1999
// written: Wed Sep 27 15:22:56 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IO_CC_DEFINED
#define IO_CC_DEFINED

#include "io/io.h"

#include "io/writer.h"
#include "io/iolegacy.h"

#include "system/demangle.h"

#include "util/strings.h"

#include <cctype>
#include <cstring>
#include <string>
#include <strstream.h>
#include <typeinfo>

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

class DummyCountingWriter : public IO::Writer {
public:
  DummyCountingWriter() : itsCount(0) {}

  virtual void writeChar(const char*, char)             { ++itsCount; }
  virtual void writeInt(const char*, int)               { ++itsCount; }
  virtual void writeBool(const char*, bool)             { ++itsCount; }
  virtual void writeDouble(const char*, double)         { ++itsCount; }
  virtual void writeCstring(const char*, const char*)   { ++itsCount; }
  virtual void writeValueObj(const char*, const Value&) { ++itsCount; }
  virtual void writeObject(const char*, const IO::IoObject*)      { ++itsCount; }
  virtual void writeOwnedObject(const char*, const IO::IoObject*) { ++itsCount; }
  virtual void writeBaseClass(const char*, const IO::IoObject*)   { ++itsCount; }
  virtual void writeRoot(const IO::IoObject*) {}

  void reset() { itsCount = 0; }
  unsigned int attribCount() const { return itsCount; }

private:
  unsigned int itsCount;
};

///////////////////////////////////////////////////////////////////////
//
// IO member definitions
//
///////////////////////////////////////////////////////////////////////

IO::IoObject::IoObject() : itsId(++idCounter) {
DOTRACE("IO::IoObject::IoObject");
  DebugEvalNL(itsId);
}

// Must be defined out of line to avoid duplication of IO's vtable
IO::IoObject::~IoObject() {
DOTRACE("IO::IoObject::~IoObject");
}

void IO::IoObject::ioSerialize(STD_IO::ostream& os, IO::IOFlag flag) const {
DOTRACE("IO::IoObject::ioSerialize");
  LegacyWriter writer(os, flag);
  writer.writeRoot(this);
}

void IO::IoObject::ioDeserialize(STD_IO::istream& is, IO::IOFlag flag) {
DOTRACE("IO::IoObject::ioDeserialize");
  LegacyReader reader(is, flag);
  reader.readRoot(this);
}

int IO::IoObject::ioCharCount() const {
DOTRACE("IO::IoObject::ioCharCount");
  ostrstream ost;
  ioSerialize(ost, IO::TYPENAME|IO::BASES);
  return strlen(ost.str());
}

void IO::IoObject::legacySrlz(IO::Writer* writer) const {
  throw IO::OutputError("legacySrlz not implemented for this object");
}

void IO::IoObject::legacyDesrlz(IO::Reader* reader) {
  throw IO::InputError("legacyDesrlz not implemented for this object");
}

unsigned int IO::IoObject::ioAttribCount() const {
DOTRACE("IO::IoObject::ioAttribCount");
  static DummyCountingWriter counter;
  counter.reset();
  this->writeTo(&counter);
  return counter.attribCount();
}

unsigned long IO::IoObject::id() const {
DOTRACE("IO::IoObject::id");
  return itsId;
}

IO::VersionId IO::IoObject::serialVersionId() const {
DOTRACE("IO::IoObject::serialVersionId");
  return 0; 
}

fixed_string IO::IoObject::ioTypename() const {
DOTRACE("IO::IoObject::ioTypename");
  return demangle_cstr(typeid(*this).name());
}

int IO::IoObject::eatWhitespace(STD_IO::istream& is) {
DOTRACE("IO::IoObject::eatWhitespace");
  int c=0;
  while ( isspace(is.peek()) )
	 { is.get(); ++c; }
  return c;
}

void IO::IoObject::readTypename(STD_IO::istream& is,
										  const char* correctNames_cstr,
										  bool doCheck) {
DOTRACE("IO::IoObject::readTypename");
  std::string correctNames = correctNames_cstr;

  std::string name;
  is >> name;

  DebugEval(name); DebugPrintNL("");
  DebugEvalNL(is.peek());

  // If we aren't checking, then we can skip the rest of the routine.
  if (!doCheck) return;
  
  DebugEval(correctNames);
  DebugEvalNL(doCheck);

  std::string candidate;

  std::string first_candidate = correctNames.substr(0, correctNames.find(' ', 0));

  std::string::size_type end_pos = 0;
  std::string::size_type beg_pos = 0;

  // Loop over the space-separated substrings of correctNames.
  while ( end_pos < std::string::npos ) {
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

IO::IoError::IoError(const std::type_info& ti) :
  ErrorWithMsg(demangle_cstr(typeid(*this).name()))
{
DOTRACE("IO::IoError::IoError");
  appendMsg(": ");
  appendMsg(demangle_cstr(ti.name()));
}

IO::IoError::~IoError() {}

void IO::IoError::setMsg(const char* str) {
DOTRACE("IO::IoError::setMsg(const char*)");
  ErrorWithMsg::setMsg(demangle_cstr(typeid(*this).name()));
  ErrorWithMsg::appendMsg(": ");
  ErrorWithMsg::appendMsg(str);
}

void IO::IoError::setMsg(const std::type_info& ti) {
DOTRACE("IO::IoError::setMsg(const std::type_info&)");
  IoError::setMsg(demangle_cstr(ti.name()));
}

IO::InputError::InputError(const char* str) {
DOTRACE("IO::InputError::InputError");
  setMsg(str);
}

IO::InputError::~InputError() {}

IO::OutputError::OutputError(const char* str) {
DOTRACE("IO::OutputError::OutputError");
  setMsg(str);
}

IO::OutputError::~OutputError() {}

IO::LogicError::LogicError(const char* str) {
DOTRACE("IO::LogicError::LogicError");
  setMsg(str);
}

IO::LogicError::~LogicError() {}

IO::ValueError::ValueError(const char* str) {
DOTRACE("IO::ValueError::ValueError");
  setMsg(str);
}

IO::ValueError::~ValueError() {}

IO::FilenameError::FilenameError(const char* str) {
DOTRACE("IO::FilenameError::FilenameError");
  setMsg(str);
}

IO::FilenameError::~FilenameError() {}

static const char vcid_io_cc[] = "$Header$";
#endif // !IO_CC_DEFINED
