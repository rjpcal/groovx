///////////////////////////////////////////////////////////////////////
//
// asciistreamwriter.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Jun  7 13:05:56 1999
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

#ifndef ASCIISTREAMWRITER_H_DEFINED
#define ASCIISTREAMWRITER_H_DEFINED

#include "io/writer.h"

#ifdef HAVE_IOSFWD
#  include <iosfwd>
#else
class ostream;
#endif

// This is a hack to help shorten up names for assemblers on systems
// that need short identifier names. A typedef at the end of this file
// re-introduces AsciiStreamWriter as a typedef for ASW, so that
// client code should be able to use the typename AsciiStreamWriter in
// all cases.
#if defined(SHORTEN_SYMBOL_NAMES)
#define AsciiStreamWriter ASW
#endif

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c AsciiStreamWriter implements the \c IO::Writer interface, using nice
 * human-readable ascii formatting. \c AsciiStreamWriter writes
 * objects to an output stream in a ascii format that is readable by
 * \AsciiStreamReader. With this data format, objects may read and
 * write their attributes in any order.
 *
 **/
///////////////////////////////////////////////////////////////////////

class AsciiStreamWriter : public IO::Writer
{
public:
  /// Construct with a connection to an open \c STD_IO::ostream.
  AsciiStreamWriter(STD_IO::ostream& os);

  /// Convenience constructor which opens a std::fstream with \a filename.
  AsciiStreamWriter(const char* filename);

  /// Virtual destructor.
  virtual ~AsciiStreamWriter();

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
  class Impl;
  Impl* rep;
};


// The second part of the hack to shorten mangled names.
#if defined(SHORTEN_SYMBOL_NAMES)
#undef AsciiStreamWriter
typedef ASW AsciiStreamWriter;
#endif

static const char vcid_asciistreamwriter_h[] = "$Header$";
#endif // !ASCIISTREAMWRITER_H_DEFINED
