///////////////////////////////////////////////////////////////////////
//
// reader.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun  7 12:46:08 1999
// written: Wed Mar 22 21:59:42 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef READER_H_DEFINED
#define READER_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(ERROR_H)
#include "util/error.h"
#endif

class IO;
class Value;

///////////////////////////////////////////////////////////////////////
/**
 *
 * Exception class for errors in \c Reader methods. \c ReadError
 * should be thrown by subclasses of \c Reader when an error occurs
 * during a call to any of the \c Reader methods.
 *
 **/
///////////////////////////////////////////////////////////////////////

class ReadError : public ErrorWithMsg {
public:
  /// Construct with a descriptive message \a msg.
  ReadError(const char* msg);
};


///////////////////////////////////////////////////////////////////////
/**
 *
 * \c Reader provides the interface that \c IO objects use to restore
 * their state in a \c readFrom() implementation. It provides the
 * inverse operations to those in \c Writer. To reconstruct an object
 * tree, a client should call \c readRoot() on the root object in that
 * tree. Subclasses of \c Reader will implement this interface using
 * different back ends for the actual storage medium, for example,
 * an ASCII text file, an on-screen dialog box, a relational database,
 * etc.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Reader {
public:

  /// Virtual destructor allows correct destruction of subclasses.
  virtual ~Reader();

  /** Returns the serialization version id that was stored with the
      object currently being read. */
  virtual unsigned long readSerialVersionId() = 0;

  /// Read the \c char attribute associated with the tag \a name.
  virtual char readChar(const char* name) = 0;

  /// Read the \c int attribute associated with the tag \a name.
  virtual int readInt(const char* name) = 0;

  /// Read the \c bool attribute associated with the tag \a name.
  virtual bool readBool(const char* name) = 0;

  /// Read the \c double attribute associated with the tag \a name.
  virtual double readDouble(const char* name) = 0;

  /// Read the C-style string (\c char*) attribute associated with the tag \a name.
  virtual char* readCstring(const char* name) = 0;

  /// Read the \c Value attribute associated with the tag \a name.
  virtual void readValueObj(const char* name, Value& value) = 0;

  /** This generic function reads a value attribute of any basic type,
      of type \c Value, or of several supported string types (\c
      std::string, \c fixed_string, and \c dyanmic_string). The value
      associated with tag \a name will be copied into \a
      returnValue. */
  template <class T>
  void readValue(const char* name, T& returnValue);

  /** Get a pointer to the \c IO object associated with the tag \a
      name. A new object of the appropriate type will be created, if
      necessary. */
  virtual IO* readObject(const char* name) = 0;

  /** Restore the state of the IO object \a obj, associated with the
      tag \a name. The \c Reader will not create a new object, but
      will use the IO* provided here. */
  virtual void readOwnedObject(const char* name, IO* obj) = 0;

  /** Read the named base class into the IO object \a obj, which
      should be arranged to point or refer to the appropriate base
      class part of the object. In particular, \a obj's virtual
      functions must NOT call the fully derived versions. This effect
      can be best accomplished with an \c IOProxy. */
  virtual void readBaseClass(const char* baseClassName, IO* basePart) = 0;

  /** Restore an entire object hierarchy, starting with the root
		object. If \a root is non-null, the function will use \a root as
		the root object. Otherwise the function will create a new root
		object. In either case, the function returns the object that was
		actually used as the root object. */
  virtual IO* readRoot(IO* root=0) = 0;
};

static const char vcid_reader_h[] = "$Header$";
#endif // !READER_H_DEFINED
