///////////////////////////////////////////////////////////////////////
//
// io.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Mar  9 20:25:02 1999
// written: Wed Aug  8 20:16:39 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IO_CC_DEFINED
#define IO_CC_DEFINED

#include "io/io.h"

#include "io/writer.h"

#include "system/demangle.h"

#include "util/ref.h"
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

class DummyCountingWriter : public IO::Writer {
public:
  DummyCountingWriter() : itsCount(0) {}

  virtual void writeChar(const char*, char)             { ++itsCount; }
  virtual void writeInt(const char*, int)               { ++itsCount; }
  virtual void writeBool(const char*, bool)             { ++itsCount; }
  virtual void writeDouble(const char*, double)         { ++itsCount; }
  virtual void writeCstring(const char*, const char*)   { ++itsCount; }
  virtual void writeValueObj(const char*, const Value&) { ++itsCount; }
  virtual void writeObject(const char*, WeakRef<const IO::IoObject>)
    { ++itsCount; }
  virtual void writeOwnedObject(const char*, Ref<const IO::IoObject>)
    { ++itsCount; }
  virtual void writeBaseClass(const char*, Ref<const IO::IoObject>)
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

IO::IoObject::IoObject() {
DOTRACE("IO::IoObject::IoObject");
  DebugEvalNL(this);
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

IO::VersionId IO::IoObject::serialVersionId() const {
DOTRACE("IO::IoObject::serialVersionId");
  return 0;
}

fstring IO::IoObject::ioTypename() const {
DOTRACE("IO::IoObject::ioTypename");
  return demangle_cstr(typeid(*this).name());
}

///////////////////////////////////////////////////////////////////////
//
// IoFilenameError member function definitions
//
///////////////////////////////////////////////////////////////////////

IO::FilenameError::FilenameError(const char* str)
{
DOTRACE("IO::FilenameError::FilenameError");
  append("couldn't use file '", str, "'");
}

IO::FilenameError::~FilenameError() {}

static const char vcid_io_cc[] = "$Header$";
#endif // !IO_CC_DEFINED
