///////////////////////////////////////////////////////////////////////
//
// reader.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Jun  7 12:46:08 1999
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

#ifndef READER_H_DEFINED
#define READER_H_DEFINED

#include "util/error.h"
#include "util/strings.h"

#include "io/iodecls.h"

namespace Util
{
  template <class T> class Ref;
  template <class T> class SoftRef;
}

class Value;

///////////////////////////////////////////////////////////////////////
/**
 *
 * Exception class for errors in \c IO::Reader methods. \c ReadError
 * should be thrown by subclasses of \c IO::Reader when an error occurs
 * during a call to any of the \c IO::Reader methods.
 *
 **/
///////////////////////////////////////////////////////////////////////

class IO::ReadError : public Util::Error
{
public:
  ReadError(const fstring& msg) : Util::Error(msg) {};

  virtual ~ReadError();
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * ReadVersionError is an exception class that can be thrown by
 * clients of Reader if there is a problem serial version id during
 * the read (e.g. the version is no longer supported).
 *
 **/
///////////////////////////////////////////////////////////////////////

class IO::ReadVersionError : public Util::Error
{
public:
  /// Construct with information relevant to the problem
  ReadVersionError(const char* classname,
                   IO::VersionId attempted_id,
                   IO::VersionId lowest_supported_id,
                   const char* msg);

  virtual ~ReadVersionError();
};

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
  virtual ~Reader();

  /** A convenience function to ensure that the current serial version
      is no less than the \a lowest_supported_version. Returns the
      actual version. If this test fails, a ReadVersionError will be
      thrown. */
  int ensureReadVersionId(const char* name,
                          IO::VersionId lowest_supported_version,
                          const char* msg);

  /** Returns the serialization version id that was stored with the
      object currently being read. */
  virtual IO::VersionId readSerialVersionId() = 0;

  /// Read the \c char attribute associated with the tag \a name.
  virtual char readChar(const fstring& name) = 0;

  /// Read the \c int attribute associated with the tag \a name.
  virtual int readInt(const fstring& name) = 0;

  /// Read the \c bool attribute associated with the tag \a name.
  virtual bool readBool(const fstring& name) = 0;

  /// Read the \c double attribute associated with the tag \a name.
  virtual double readDouble(const fstring& name) = 0;

  /// Read the \c Value attribute associated with the tag \a name.
  virtual void readValueObj(const fstring& name, Value& value) = 0;

  /** @name Overloaded read functions

      These functions offer a set of overloads to provide compile-time
      polymorphism so that any supported type (any basic type, or Value, a
      string type) or can be read with the same source code.
  */
  //@{

  void readValue(const fstring& name, char& ret)
  { ret = readChar(name); }

  void readValue(const fstring& name, int& ret)
  { ret = readInt(name); }

  void readValue(const fstring& name, unsigned int& ret)
  { ret = readInt(name); }

  void readValue(const fstring& name, unsigned long& ret)
  { ret = readInt(name); }

  void readValue(const fstring& name, bool& ret)
  { ret = readBool(name); }

  void readValue(const fstring& name, double& ret)
  { ret = readDouble(name); }

  void readValue(const fstring& name, fstring& ret)
  { ret = readStringImpl(name); }

  void readValue(const fstring& name, Value& ret)
  { readValueObj(name, ret); }

  //@}

  /** Get a \c Util::Ref associated with the tag \a name. A new object
      of the appropriate type will be created and inserted into the \c
      ObjDb, if necessary. */
  virtual Util::Ref<IO::IoObject> readObject(const fstring& name) = 0;

  /** Get a \c SoftRef associated with the tag \a name. If no such
      object exists, a null object is returned; otherwise, a new
      object of the appropriate type will be created and inserted into
      the \c ObjDb, if necessary. */
  virtual Util::SoftRef<IO::IoObject> readMaybeObject(const fstring& name) = 0;

  /** Restore the state of the IO object \a obj, associated with the
      tag \a name. The \c Reader will not create a new object, but
      will use the IO* provided here. */
  virtual void readOwnedObject(const fstring& name,
                               Util::Ref<IO::IoObject> obj) = 0;

  /** Read the named base class into the IO object \a obj, which
      should be arranged to point or refer to the appropriate base
      class part of the object. In particular, \a obj's virtual
      functions must NOT call the fully derived versions. This effect
      can be best accomplished with an \c IO::IoProxy. */
  virtual void readBaseClass(const fstring& baseClassName,
                             Util::Ref<IO::IoObject> basePart) = 0;

  /** Restore an entire object hierarchy, starting with the root
      object. If \a root is non-null, the function will use \a root as
      the root object. Otherwise the function will create a new root
      object. In either case, the function returns the object that was
      actually used as the root object. */
  virtual Util::Ref<IO::IoObject> readRoot(IO::IoObject* root=0) = 0;

protected:
  /// Read the string attribute associated with the tag \a name.
  virtual fstring readStringImpl(const fstring& name) = 0;
};

static const char vcid_reader_h[] = "$Header$";
#endif // !READER_H_DEFINED
