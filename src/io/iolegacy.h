///////////////////////////////////////////////////////////////////////
//
// iolegacy.h
//
// Copyright (c) 2000-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Tue Sep 26 18:47:31 2000
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

#ifndef IOLEGACY_H_DEFINED
#define IOLEGACY_H_DEFINED

#include "io/io.h"
#include "io/reader.h"
#include "io/writer.h"

#ifdef HAVE_IOSFWD
#  include <iosfwd>
#else
class istream;
class ostream;
#endif

namespace IO
{
  class LegacyReader;
  class LegacyWriter;
}

// ########################################################

/// Implements the IO::Reader interface using the "legacy" data format.

class IO::LegacyReader : public IO::Reader
{
public:
  LegacyReader(STD_IO::istream& is);
  virtual ~LegacyReader() throw();

  virtual IO::VersionId readSerialVersionId();

  virtual char readChar(const rutz::fstring& name);
  virtual int readInt(const rutz::fstring& name);
  virtual bool readBool(const rutz::fstring& name);
  virtual double readDouble(const rutz::fstring& name);
  virtual void readValueObj(const rutz::fstring& name, rutz::value& v);

  virtual void readRawData(const rutz::fstring& name, rutz::byte_array& data)
  { defaultReadRawData(name, data); }

  virtual Nub::Ref    <IO::IoObject>      readObject(const rutz::fstring& name);
  virtual Nub::SoftRef<IO::IoObject> readMaybeObject(const rutz::fstring& name);

  virtual void readOwnedObject(const rutz::fstring& name,
                               Nub::Ref<IO::IoObject> obj);

  virtual void readBaseClass(const rutz::fstring& baseClassName,
                             Nub::Ref<IO::IoObject> basePart);

  virtual Nub::Ref<IO::IoObject> readRoot(IO::IoObject* root=0);

protected:
  virtual rutz::fstring readStringImpl(const rutz::fstring& name);

private:
  LegacyReader(const LegacyReader&);
  LegacyReader& operator=(const LegacyReader&);

  class Impl;
  friend class Impl;

  Impl* const rep;
};


// ########################################################

/// Implements the IO::Writer interface using the "legacy" data format.

class IO::LegacyWriter : public IO::Writer
{
public:
  LegacyWriter(STD_IO::ostream& os, bool write_bases=true);
  virtual ~LegacyWriter() throw();

  void usePrettyPrint(bool yes=true);

  virtual void writeChar(const char* name, char val);
  virtual void writeInt(const char* name, int val);
  virtual void writeBool(const char* name, bool val);
  virtual void writeDouble(const char* name, double val);
  virtual void writeValueObj(const char* name, const rutz::value& v);

  virtual void writeRawData(const char* name,
                            const unsigned char* data,
                            unsigned int length);

  virtual void writeObject(const char* name,
                           Nub::SoftRef<const IO::IoObject> obj);

  virtual void writeOwnedObject(const char* name,
                                Nub::Ref<const IO::IoObject> obj);

  virtual void writeBaseClass(const char* baseClassName,
                              Nub::Ref<const IO::IoObject> basePart);

  virtual void writeRoot(const IO::IoObject* root);

protected:
  virtual void writeCstring(const char* name, const char* val);

private:
  LegacyWriter(const LegacyWriter&);
  LegacyWriter& operator=(const LegacyWriter&);

  class Impl;
  friend class Impl;

  Impl* const rep;
};

static const char vcid_iolegacy_h[] = "$Id$ $URL$";
#endif // !IOLEGACY_H_DEFINED
