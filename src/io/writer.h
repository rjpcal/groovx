///////////////////////////////////////////////////////////////////////
//
// writer.h
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Jun  7 12:49:49 1999
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

#ifndef WRITER_H_DEFINED
#define WRITER_H_DEFINED

#include "util/fileposition.h"
#include "util/strings.h"

#include "io/iodecls.h"

namespace Util
{
  template <class T> class Ref;
  template <class T> class SoftRef;
}

namespace IO
{
  class IoObject;
  class Writer;
}

class Value;

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c IO::Writer provides the interface that \c IO objects use to save
 * their state in a \c writeTo() implementation. It provides the
 * inverse operations to those in \c IO::Reader. To save an object tree, a
 * client should call \c writeRoot() on the root object in that
 * tree. Subclasses of \c IO::Writer will implement this interface using
 * different back ends for the actual storage medium, for example, an
 * ASCII text file, an on-screen dialog box, a relational database,
 * etc.
 *
 **/
///////////////////////////////////////////////////////////////////////

class IO::Writer
{
public:

  /// Virtual destructor allows correct destruction of subclasses.
  virtual ~Writer() throw();

  /** A convenience function to ensure that the current serial version
      is no less than the \a lowest_supported_version. Returns the \a
      actual_version. If this test fails, a WriteVersionError will be
      thrown.*/
  int ensureWriteVersionId(const char* name,
                           IO::VersionId actual_version,
                           IO::VersionId lowest_supported_version,
                           const char* msg,
                           const rutz::file_pos& pos);

  /// Store the \c char attribute \a val in association with the tag \a name.
  virtual void writeChar(const char* name, char val) = 0;

  /// Store the \c int attribute \a val in association with the tag \a name.
  virtual void writeInt(const char* name, int val) = 0;

  /// Store the \c bool attribute \a val in association with the tag \a name.
  virtual void writeBool(const char* name, bool val) = 0;

  /// Store the \c double attribute \a val in association with the tag \a name.
  virtual void writeDouble(const char* name, double val) = 0;

  /// Store the \c Value attribute \a val in association with the tag \a name.
  virtual void writeValueObj(const char* name, const Value& value) = 0;

  /// Store the raw data array in association with the tag \a name.
  virtual void writeRawData(const char* name,
                            const unsigned char* data,
                            unsigned int length) = 0;

  /** @name Overloaded write functions

      These functions offer a set of overloads to provide compile-time
      polymorphism so that any supported type (any basic type, or Value, a
      string type) or can be written with the same source code.
  */
  //@{

  void writeValue(const char* name, char val)
  { writeChar(name, val); }

  void writeValue(const char* name, int val)
  { writeInt(name, val); }

  void writeValue(const char* name, unsigned int val)
  { writeInt(name, val); }

  void writeValue(const char* name, unsigned long val)
  { writeInt(name, val); }

  void writeValue(const char* name, bool val)
  { writeBool(name, val); }

  void writeValue(const char* name, double val)
  { writeDouble(name, val); }

  void writeValue(const char* name, const fstring& val)
  { writeCstring(name, val.c_str()); }

  void writeValue(const char* name, const Value& value)
  { writeValueObj(name, value); }

  //@}

  /// Store the \c IO object \a val in association with the tag \a name.
  virtual void writeObject(const char* name,
                           Util::SoftRef<const IO::IoObject> obj) = 0;

  /** Store the owned \c IO object \a obj in association with the tag
      \a name. This function should only be used if \a obj is \b owned
      by the storing object; no other objects should reference \a
      obj. This allows the \c Writer subclass to implement the storage
      of an owned object as a contained object. */
  virtual void writeOwnedObject(const char* name,
                                Util::Ref<const IO::IoObject> obj) = 0;

  /** Write the named base class using the IO object \a obj, which
      should be arranged to point or refer to the appropriate base
      class part of the object. In particular, \a obj's virtual
      functions must NOT call the fully derived versions. This effect
      can be best accomplished with an \c IO::IoProxy. */
  virtual void writeBaseClass(const char* baseClassName,
                              Util::Ref<const IO::IoObject> basePart) = 0;

  /** Store an entire object hierarchy, starting with the root object
      \a root. All objects and values referenced by \a root will be
      stored recursively, until there are no more remaining
      references. */
  virtual void writeRoot(const IO::IoObject* root) = 0;

protected:
  /// Store the C-style string (\c char*) attribute \a val in association with the tag \a name.
  virtual void writeCstring(const char* name, const char* val) = 0;


  /// Base64 implementation of writeRawData() for use by subclasses.
  void defaultWriteRawData(const char* name,
                           const unsigned char* data,
                           unsigned int length);
};

static const char vcid_writer_h[] = "$Header$";
#endif // !WRITER_H_DEFINED
