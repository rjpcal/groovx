///////////////////////////////////////////////////////////////////////
//
// reader.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jun  7 12:46:08 1999
// written: Thu May 17 15:11:04 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef READER_H_DEFINED
#define READER_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(ERROR_H_DEFINED)
#include "util/error.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(STRINGS_H_DEFINED)
#include "util/strings.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IODECLS_H_DEFINED)
#include "io/iodecls.h"
#endif

template <class T> class IdItem;
template <class T> class MaybeIdItem;

namespace IO {
  class IoObject;
  class ReadError;
  class ReadVersionError;
  class Reader;
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

class IO::ReadError : public ErrorWithMsg {
public:
  /// Construct with a descriptive message \a msg.
  ReadError(const char* msg);

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

class IO::ReadVersionError : public ErrorWithMsg {
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

class IO::Reader {
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
  virtual char readChar(const fixed_string& name) = 0;

  /// Read the \c int attribute associated with the tag \a name.
  virtual int readInt(const fixed_string& name) = 0;

  /// Read the \c bool attribute associated with the tag \a name.
  virtual bool readBool(const fixed_string& name) = 0;

  /// Read the \c double attribute associated with the tag \a name.
  virtual double readDouble(const fixed_string& name) = 0;

  /// Read the \c Value attribute associated with the tag \a name.
  virtual void readValueObj(const fixed_string& name, Value& value) = 0;

  /** This generic function reads a value attribute of any basic type,
      of type \c Value, or of several supported string types (\c
      std::string, \c fixed_string, and \c dyanmic_string). The value
      associated with tag \a name will be copied into \a
      returnValue. */
  template <class T>
  void readValue(const fixed_string& name, T& returnValue);

  /** Get an \c IdItem associated with the tag \a name. A new object
      of the appropriate type will be created and inserted into the \c
      IoDb, if necessary. */
  IdItem<IO::IoObject> readObject(const fixed_string& name);

  /** Get a \c MaybeIdItem associated with the tag \a name. If no such
      object exists, a null object is returned; otherwise, a new
      object of the appropriate type will be created and inserted into
      the \c IoDb, if necessary. */
  MaybeIdItem<IO::IoObject> readMaybeObject(const fixed_string& name);

  /** Restore the state of the IO object \a obj, associated with the
      tag \a name. The \c Reader will not create a new object, but
      will use the IO* provided here. */
  virtual void readOwnedObject(const fixed_string& name,
										 IO::IoObject* obj) = 0;

  /** Read the named base class into the IO object \a obj, which
      should be arranged to point or refer to the appropriate base
      class part of the object. In particular, \a obj's virtual
      functions must NOT call the fully derived versions. This effect
      can be best accomplished with an \c IO::IoProxy. */
  virtual void readBaseClass(const fixed_string& baseClassName,
									  IO::IoObject* basePart) = 0;

  /** Restore an entire object hierarchy, starting with the root
		object. If \a root is non-null, the function will use \a root as
		the root object. Otherwise the function will create a new root
		object. In either case, the function returns the object that was
		actually used as the root object. */
  virtual IO::IoObject* readRoot(IO::IoObject* root=0) = 0;

protected:
  /// Read the string attribute associated with the tag \a name.
  virtual fixed_string readStringImpl(const fixed_string& name) = 0;

  /** Get a pointer to the \c IO object associated with the tag \a
      name, creating a new object of the appropriate type, if
      necessary. */
  virtual IO::IoObject* readObjectImpl(const fixed_string& name) = 0;
};

static const char vcid_reader_h[] = "$Header$";
#endif // !READER_H_DEFINED
