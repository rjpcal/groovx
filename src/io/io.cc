///////////////////////////////////////////////////////////////////////
//
// io.cc
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Mar  9 20:25:02 1999
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_IO_IO_CC_UTC20050626084021_DEFINED
#define GROOVX_IO_IO_CC_UTC20050626084021_DEFINED

#include "io/io.h"

#include "io/writer.h"

#include "nub/ref.h"

#include "rutz/fstring.h"

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

///////////////////////////////////////////////////////////////////////
//
// File scope declarations
//
///////////////////////////////////////////////////////////////////////

namespace
{
  using namespace nub;

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
                             soft_ref<const IO::IoObject>) { ++itsC; }

    virtual void writeOwnedObject(const char*,
                                  ref<const IO::IoObject>){ ++itsC; }

    virtual void writeBaseClass(const char*,
                                ref<const IO::IoObject>)  { ++itsC; }

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
GVX_TRACE("IO::IoObject::IoObject");
  dbg_eval_nl(3, this);
}

// Must be defined out of line to avoid duplication of IO's vtable
IO::IoObject::~IoObject() throw()
{
GVX_TRACE("IO::IoObject::~IoObject");
}

unsigned int IO::IoObject::ioAttribCount() const
{
GVX_TRACE("IO::IoObject::ioAttribCount");
  static DummyCountingWriter counter;
  counter.reset();
  this->writeTo(counter);
  return counter.attribCount();
}

IO::VersionId IO::IoObject::serialVersionId() const
{
GVX_TRACE("IO::IoObject::serialVersionId");
  return 0;
}

static const char vcid_groovx_io_io_cc_utc20050626084021[] = "$Id$ $HeadURL$";
#endif // !GROOVX_IO_IO_CC_UTC20050626084021_DEFINED
