///////////////////////////////////////////////////////////////////////
//
// writer.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jun  7 12:49:49 1999
// written: Mon Jan 13 11:04:47 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef WRITER_H_DEFINED
#define WRITER_H_DEFINED

#include "util/error.h"

#include "io/iodecls.h"

namespace Util
{
  template <class T> class Ref;
  template <class T> class SoftRef;
};

namespace IO
{
  class IoObject;
  class Writer;
}

class Value;

///////////////////////////////////////////////////////////////////////
/**
 *
 * Exception class for errors in \c IO::Writer methods. \c WriteError
 * should be thrown by subclasses of \c IO::Writer when an error occurs
 * during a call to any of the methods in the \c IO::Writer interface.
 *
 **/
///////////////////////////////////////////////////////////////////////

class IO::WriteError : public Util::Error
{
public:
  WriteError(const fstring& msg) : Util::Error(msg) {};

  virtual ~WriteError();
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * WriteVersionError is an exception class that can be thrown by
 * clients of Writer if there is a problem serial version id during
 * the read (e.g. the version is no longer supported).
 *
 **/
///////////////////////////////////////////////////////////////////////

class IO::WriteVersionError : public Util::Error
{
public:
  /// Construct with information relevant to the problem
  WriteVersionError(const char* classname,
                   IO::VersionId attempted_id,
                   IO::VersionId lowest_supported_id,
                   const char* msg);

  virtual ~WriteVersionError();
};

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
  virtual ~Writer();

  /** A convenience function to ensure that the current serial version
      is no less than the \a lowest_supported_version. Returns the \a
      actual_version. If this test fails, a WriteVersionError will be
      thrown.*/
  int ensureWriteVersionId(const char* name,
                           IO::VersionId actual_version,
                           IO::VersionId lowest_supported_version,
                           const char* msg);

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

  /** This generic function stores a value attribute of any basic
      type, or of type \c Value. The value \a val will be stored in
      association with the tag \a name. */
  template <class T>
  void writeValue(const char* name, const T& val);

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
};

static const char vcid_writer_h[] = "$Header$";
#endif // !WRITER_H_DEFINED
