///////////////////////////////////////////////////////////////////////
//
// asciistreamreader.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Jun  7 12:54:54 1999
// written: Wed Mar 19 17:55:55 2003
// $Id$
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
  Impl* rep;
};

// The second part of the hack to shorten mangled names.
#if defined(SHORTEN_SYMBOL_NAMES)
#undef AsciiStreamReader
typedef ASR AsciiStreamReader;
#endif

static const char vcid_asciistreamreader_h[] = "$Header$";
#endif // !ASCIISTREAMREADER_H_DEFINED
