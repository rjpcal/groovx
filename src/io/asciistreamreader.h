///////////////////////////////////////////////////////////////////////
//
// asciistreamreader.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jun  7 12:54:54 1999
// written: Wed Sep 25 18:53:50 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ASCIISTREAMREADER_H_DEFINED
#define ASCIISTREAMREADER_H_DEFINED

#include "io/reader.h"

#ifdef HAVE_IOSFWD
#  include <iosfwd>
#else
class istream;
class ostream;
#endif

// This is a hack to help shorten up names for assemblers on systems
// that need short identifier names. A typedef at the end of this file
// re-introduces AsciiStreamReader as a typedef for ASR, so that
// client code should be able to use the typename AsciiStreamReader in
// all cases.
#if defined(SHORTEN_SYMBOL_NAMES)
#define AsciiStreamReader ASR
#endif

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c AsciiStreamReader implements the \c IO::Reader interface, using nice
 * human-readable ascii formatting. \c AsciiStreamReader restores
 * objects from an input stream containing ascii data that has been
 * generated by AsciiStreamWriter. With this data format, objects may
 * read and write their attributes in any order.
 *
 **/
///////////////////////////////////////////////////////////////////////

class AsciiStreamReader : public IO::Reader
{
public:
  /// Construct with a connection to an open \c STD_IO::istream.
  AsciiStreamReader(STD_IO::istream& is);

  /// Virtual destructor.
  virtual ~AsciiStreamReader();

  virtual IO::VersionId readSerialVersionId();

  virtual char readChar(const fstring& name);
  virtual int readInt(const fstring& name);
  virtual bool readBool(const fstring& name);
  virtual double readDouble(const fstring& name);
  virtual void readValueObj(const fstring& name, Value& value);

  virtual Util::Ref<IO::IoObject> readObject(const fstring& name);
  virtual Util::SoftRef<IO::IoObject>
    readMaybeObject(const fstring& name);

  virtual void readOwnedObject(const fstring& name,
                               Util::Ref<IO::IoObject> obj);
  virtual void readBaseClass(const fstring& baseClassName,
                             Util::Ref<IO::IoObject> basePart);

  virtual Util::Ref<IO::IoObject> readRoot(IO::IoObject* root=0);

protected:
  virtual fstring readStringImpl(const fstring& name);

private:
  class Impl;
  Impl& itsImpl;
};

// The second part of the hack to shorten mangled names.
#if defined(SHORTEN_SYMBOL_NAMES)
#undef AsciiStreamReader
typedef ASR AsciiStreamReader;
#endif

static const char vcid_asciistreamreader_h[] = "$Header$";
#endif // !ASCIISTREAMREADER_H_DEFINED
