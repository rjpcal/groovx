///////////////////////////////////////////////////////////////////////
//
// iolegacy.h
//
// Copyright (c) 2000-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Sep 26 18:47:31 2000
// written: Mon Jan 20 13:03:01 2003
// $Id$
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

// ####################################################################

/// Implements the IO::Reader interface using the "legacy" data format.

class IO::LegacyReader : public IO::Reader
{
public:
  LegacyReader(STD_IO::istream& is);
  virtual ~LegacyReader();

  virtual IO::VersionId readSerialVersionId();

  virtual char readChar(const fstring& name);
  virtual int readInt(const fstring& name);
  virtual bool readBool(const fstring& name);
  virtual double readDouble(const fstring& name);
  virtual void readValueObj(const fstring& name, Value& value);

  virtual Util::Ref    <IO::IoObject>      readObject(const fstring& name);
  virtual Util::SoftRef<IO::IoObject> readMaybeObject(const fstring& name);

  virtual void readOwnedObject(const fstring& name,
                               Util::Ref<IO::IoObject> obj);

  virtual void readBaseClass(const fstring& baseClassName,
                             Util::Ref<IO::IoObject> basePart);

  virtual Util::Ref<IO::IoObject> readRoot(IO::IoObject* root=0);

protected:
  virtual fstring readStringImpl(const fstring& name);

private:
  LegacyReader(const LegacyReader&);
  LegacyReader& operator=(const LegacyReader&);

  class Impl;
  friend class Impl;

  Impl* const rep;
};


// ####################################################################

/// Implements the IO::Writer interface using the "legacy" data format.

class IO::LegacyWriter : public IO::Writer
{
public:
  LegacyWriter(STD_IO::ostream& os, bool write_bases=true);
  virtual ~LegacyWriter();

  void usePrettyPrint(bool yes=true);

  virtual void writeChar(const char* name, char val);
  virtual void writeInt(const char* name, int val);
  virtual void writeBool(const char* name, bool val);
  virtual void writeDouble(const char* name, double val);
  virtual void writeValueObj(const char* name, const Value& value);

  virtual void writeObject(const char* name,
                           Util::SoftRef<const IO::IoObject> obj);

  virtual void writeOwnedObject(const char* name,
                                Util::Ref<const IO::IoObject> obj);

  virtual void writeBaseClass(const char* baseClassName,
                              Util::Ref<const IO::IoObject> basePart);

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

static const char vcid_iolegacy_h[] = "$Header$";
#endif // !IOLEGACY_H_DEFINED
