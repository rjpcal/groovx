///////////////////////////////////////////////////////////////////////
//
// io.cc
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Tue Mar  9 20:25:02 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef IO_CC_DEFINED
#define IO_CC_DEFINED

#include "io/io.h"

#include "io/writer.h"

#include "util/fstring.h"
#include "util/ref.h"

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

///////////////////////////////////////////////////////////////////////
//
// File scope declarations
//
///////////////////////////////////////////////////////////////////////

namespace
{
  using namespace Util;

  class DummyCountingWriter : public IO::Writer
  {
  public:
    DummyCountingWriter() : itsC(0) {}

    virtual void writeChar(const char*, char)             { ++itsC; }
    virtual void writeInt(const char*, int)               { ++itsC; }
    virtual void writeBool(const char*, bool)             { ++itsC; }
    virtual void writeDouble(const char*, double)         { ++itsC; }
    virtual void writeCstring(const char*, const char*)   { ++itsC; }
    virtual void writeValueObj(const char*, const rutz::value&) { ++itsC; }

    virtual void writeRawData(const char*,
                              const unsigned char*,
                              unsigned int)               { ++itsC; }

    virtual void writeObject(const char*,
                             SoftRef<const IO::IoObject>) { ++itsC; }

    virtual void writeOwnedObject(const char*,
                                  Ref<const IO::IoObject>){ ++itsC; }

    virtual void writeBaseClass(const char*,
                                Ref<const IO::IoObject>)  { ++itsC; }

    virtual void writeRoot(const IO::IoObject*) {}

    void reset() { itsC = 0; }
    unsigned int attribCount() const { return itsC; }

  private:
    unsigned int itsC;
  };
}

///////////////////////////////////////////////////////////////////////
//
// IO member definitions
//
///////////////////////////////////////////////////////////////////////

IO::IoObject::IoObject() throw()
{
DOTRACE("IO::IoObject::IoObject");
  dbg_eval_nl(3, this);
}

// Must be defined out of line to avoid duplication of IO's vtable
IO::IoObject::~IoObject() throw()
{
DOTRACE("IO::IoObject::~IoObject");
}

unsigned int IO::IoObject::ioAttribCount() const
{
DOTRACE("IO::IoObject::ioAttribCount");
  static DummyCountingWriter counter;
  counter.reset();
  this->writeTo(counter);
  return counter.attribCount();
}

IO::VersionId IO::IoObject::serialVersionId() const
{
DOTRACE("IO::IoObject::serialVersionId");
  return 0;
}

static const char vcid_io_cc[] = "$Header$";
#endif // !IO_CC_DEFINED
