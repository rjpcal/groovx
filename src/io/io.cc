///////////////////////////////////////////////////////////////////////
//
// io.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Mar  9 20:25:02 1999
// written: Sat Jun  2 15:52:21 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IO_CC_DEFINED
#define IO_CC_DEFINED

#include "io/io.h"

#include "io/iditem.h"
#include "io/iodb.h"
#include "io/writer.h"

#include "system/demangle.h"

#include "util/strings.h"

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
  virtual void writeObject(const char*, MaybeIdItem<const IO::IoObject>)
    { ++itsCount; }
  virtual void writeOwnedObject(const char*, IdItem<const IO::IoObject>)
    { ++itsCount; }
  virtual void writeBaseClass(const char*, IdItem<const IO::IoObject>)
    { ++itsCount; }
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
  DebugEval(itsId); DebugEvalNL(this);
}

// Must be defined out of line to avoid duplication of IO's vtable
IO::IoObject::~IoObject() {
DOTRACE("IO::IoObject::~IoObject");
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

IO::IoError::~IoError() {}

void IO::IoError::setMsg(const char* str) {
DOTRACE("IO::IoError::setMsg(const char*)");
  ErrorWithMsg::setMsg(demangle_cstr(typeid(*this).name()));
  ErrorWithMsg::appendMsg(": ");
  ErrorWithMsg::appendMsg(str);
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
