///////////////////////////////////////////////////////////////////////
//
// io.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Tue Mar  9 20:25:02 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef IO_CC_DEFINED
#define IO_CC_DEFINED

#include "io/io.h"

#include "io/writer.h"

#include "util/ref.h"
#include "util/strings.h"

#include "util/trace.h"
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// File scope declarations
//
///////////////////////////////////////////////////////////////////////

class DummyCountingWriter : public IO::Writer
{
public:
  DummyCountingWriter() : itsCount(0) {}

  virtual void writeChar(const char*, char)             { ++itsCount; }
  virtual void writeInt(const char*, int)               { ++itsCount; }
  virtual void writeBool(const char*, bool)             { ++itsCount; }
  virtual void writeDouble(const char*, double)         { ++itsCount; }
  virtual void writeCstring(const char*, const char*)   { ++itsCount; }
  virtual void writeValueObj(const char*, const Value&) { ++itsCount; }
  virtual void writeObject(const char*, SoftRef<const IO::IoObject>)
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

IO::IoObject::IoObject()
{
DOTRACE("IO::IoObject::IoObject");
  dbgEvalNL(3, this);
}

// Must be defined out of line to avoid duplication of IO's vtable
IO::IoObject::~IoObject()
{
DOTRACE("IO::IoObject::~IoObject");
}

unsigned int IO::IoObject::ioAttribCount() const
{
DOTRACE("IO::IoObject::ioAttribCount");
  static DummyCountingWriter counter;
  counter.reset();
  this->writeTo(&counter);
  return counter.attribCount();
}

IO::VersionId IO::IoObject::serialVersionId() const
{
DOTRACE("IO::IoObject::serialVersionId");
  return 0;
}

fstring IO::IoObject::ioTypename() const
{
DOTRACE("IO::IoObject::ioTypename");
  return objTypename();
}

static const char vcid_io_cc[] = "$Header$";
#endif // !IO_CC_DEFINED
