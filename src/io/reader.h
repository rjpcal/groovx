///////////////////////////////////////////////////////////////////////
//
// reader.h
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Jun  7 12:46:08 1999
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

#ifndef READER_H_DEFINED
#define READER_H_DEFINED

#include "util/fileposition.h"
#include "util/fstring.h"

#include "io/iodecls.h"

namespace rutz
{
  class byte_array;
  class value;
}

namespace Util
{
  template <class T> class Ref;
  template <class T> class SoftRef;
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c IO::Reader provides the interface that \c IO objects use to restore
 * their state in a \c readFrom() implementation. It provides the
 * inverse operations to those in \c IO::Writer. To reconstruct an object
 * tree, a client should call \c readRoot() on the root object in that
 * tree. Subclasses of \c IO::Reader will implement this interface using
 * different back ends for the actual storage medium, for example,
 * an ASCII text file, an on-screen dialog box, a relational database,
 * etc.
 *
 **/
///////////////////////////////////////////////////////////////////////

class IO::Reader
{
public:

  /// Virtual destructor allows correct destruction of subclasses.
  virtual ~Reader() throw();

  /** A convenience function to ensure that the current serial version
      is no less than the \a lowest_supported_version. Returns the
      actual version. If this test fails, an exception will be
      thrown. */
  int ensureReadVersionId(const char* name,
                          IO::VersionId lowest_supported_version,
                          const char* msg,
                          const rutz::file_pos& pos);

  /** Returns the serialization version id that was stored with the
      object currently being read. */
  virtual IO::VersionId readSerialVersionId() = 0;

  /// Read the \c char attribute associated with the tag \a name.
  virtual char readChar(const rutz::fstring& name) = 0;

  /// Read the \c int attribute associated with the tag \a name.
  virtual int readInt(const rutz::fstring& name) = 0;

  /// Read the \c bool attribute associated with the tag \a name.
  virtual bool readBool(const rutz::fstring& name) = 0;

  /// Read the \c double attribute associated with the tag \a name.
  virtual double readDouble(const rutz::fstring& name) = 0;

  /// Read the \c rutz::value attribute associated with the tag \a name.
  virtual void readValueObj(const rutz::fstring& name, rutz::value& v) = 0;

  /// Read the raw data array associated with the tag \a name.
  virtual void readRawData(const rutz::fstring& name, rutz::byte_array& data) = 0;

  /** @name Overloaded read functions

      These functions offer a set of overloads to provide compile-time
      polymorphism so that any supported type (any basic type, or
      rutz::value, a string type) or can be read with the same source
      code.
  */
  //@{

  void readValue(const rutz::fstring& name, char& ret)
  { ret = readChar(name); }

  void readValue(const rutz::fstring& name, int& ret)
  { ret = readInt(name); }

  void readValue(const rutz::fstring& name, unsigned int& ret)
  { ret = readInt(name); }

  void readValue(const rutz::fstring& name, unsigned long& ret)
  { ret = readInt(name); }

  void readValue(const rutz::fstring& name, bool& ret)
  { ret = readBool(name); }

  void readValue(const rutz::fstring& name, double& ret)
  { ret = readDouble(name); }

  void readValue(const rutz::fstring& name, rutz::fstring& ret)
  { ret = readStringImpl(name); }

  void readValue(const rutz::fstring& name, rutz::value& ret)
  { readValueObj(name, ret); }

  //@}

  /** Get a \c Util::Ref associated with the tag \a name. A new object
      of the appropriate type will be created and inserted into the \c
      ObjDb, if necessary. */
  virtual Util::Ref<IO::IoObject> readObject(const rutz::fstring& name) = 0;

  /** Get a \c SoftRef associated with the tag \a name. If no such
      object exists, a null object is returned; otherwise, a new
      object of the appropriate type will be created and inserted into
      the \c ObjDb, if necessary. */
  virtual Util::SoftRef<IO::IoObject> readMaybeObject(const rutz::fstring& name) = 0;

  /** Restore the state of the IO object \a obj, associated with the
      tag \a name. The \c Reader will not create a new object, but
      will use the IO* provided here. */
  virtual void readOwnedObject(const rutz::fstring& name,
                               Util::Ref<IO::IoObject> obj) = 0;

  /** Read the named base class into the IO object \a obj, which
      should be arranged to point or refer to the appropriate base
      class part of the object. In particular, \a obj's virtual
      functions must NOT call the fully derived versions. This effect
      can be best accomplished with an \c IO::IoProxy. */
  virtual void readBaseClass(const rutz::fstring& baseClassName,
                             Util::Ref<IO::IoObject> basePart) = 0;

  /** Restore an entire object hierarchy, starting with the root
      object. If \a root is non-null, the function will use \a root as
      the root object. Otherwise the function will create a new root
      object. In either case, the function returns the object that was
      actually used as the root object. */
  virtual Util::Ref<IO::IoObject> readRoot(IO::IoObject* root=0) = 0;

protected:
  /// Read the string attribute associated with the tag \a name.
  virtual rutz::fstring readStringImpl(const rutz::fstring& name) = 0;

  /// Base64 implementation of readRawData() for use by subclasses.
  void defaultReadRawData(const rutz::fstring& name, rutz::byte_array& data);
};

static const char vcid_reader_h[] = "$Header$";
#endif // !READER_H_DEFINED
