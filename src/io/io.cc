///////////////////////////////////////////////////////////////////////
//
// Io.cc
// Rob Peters
// created: Tue Mar  9 20:25:02 1999
// written: Thu Sep 28 16:12:06 2000
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

fixed_string IO::IoObject::legacyIoTypename() const {
DOTRACE("IO::IoObject::legacyIoTypename");
  return demangle_cstr(typeid(*this).name());
}

fixed_string IO::IoObject::legacyValidTypenames() const {
DOTRACE("IO::IoObject::legacyValidTypenames");
  return demangle_cstr(typeid(*this).name());
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
