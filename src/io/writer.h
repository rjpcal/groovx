///////////////////////////////////////////////////////////////////////
//
// writer.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun  7 12:49:49 1999
// written: Tue Feb 15 10:59:16 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef WRITER_H_DEFINED
#define WRITER_H_DEFINED

#ifndef STRING_DEFINED
#include <string>
#define STRING_DEFINED
#endif

#ifndef ERROR_H_DEFINED
#include "error.h"
#endif

class IO;
class Value;

///////////////////////////////////////////////////////////////////////
/**
 *
 * Exception class for errors in \c Writer methods. \c WriteError
 * should be thrown by subclasses of \c Writer when an error occurs
 * during a call to any of the methods in the \c Writer interface.
 *
 **/
///////////////////////////////////////////////////////////////////////

class WriteError : public ErrorWithMsg {
public:
  /// Construct with a descriptive message \a msg.
  WriteError(const string& msg) : ErrorWithMsg(msg) {}
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c Writer provides the interface that \c IO objects use to save
 * their state in a \c writeTo() implementation. It provides the
 * inverse operations to those in \c Reader. To save an object tree, a
 * client should call \c writeRoot() on the root object in that
 * tree. Subclasses of \c Writer will implement this interface using
 * different back ends for the actual storage medium, for example, an
 * ASCII text file, an on-screen dialog box, a relational database,
 * etc.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Writer {
public:

  /// Virtual destructor allows correct destruction of subclasses.
  virtual ~Writer();

  /// Store the \c char attribute \a val in association with the tag \a name.
  virtual void writeChar(const string& name, char val) = 0;

  /// Store the \c int attribute \a val in association with the tag \a name.
  virtual void writeInt(const string& name, int val) = 0;

  /// Store the \c bool attribute \a val in association with the tag \a name.
  virtual void writeBool(const string& name, bool val) = 0;

  /// Store the \c double attribute \a val in association with the tag \a name.
  virtual void writeDouble(const string& name, double val) = 0;

  /// Store the STL \c string attribute \a val in association with the tag \a name.
  virtual void writeString(const string& name, const string& val) = 0;

  /// Store the C-style string (\c char*) attribute \a val in association with the tag \a name.
  virtual void writeCstring(const string& name, const char* val) = 0;

  /// Store the \c Value attribute \a val in association with the tag \a name.
  virtual void writeValueObj(const string& name, const Value& value) = 0;

  /** This generic function stores a value attribute of any basic
      type, or of type \c Value. The value \a val will be stored in
      association with the tag \a name. */
  template <class T>
  void writeValue(const string& name, const T& val);

  /// Store the \c IO object \a val in association with the tag \a name.
  virtual void writeObject(const string& name, const IO* obj) = 0;

  /** Store the owned \c IO object \a obj in association with the tag
      \a name. This function should only be used if \a obj is \b owned
      by the storing object; no other objects should reference \a
      obj. This allows the \c Writer subclass to implement the storage
      of an owned object as a contained object. */
  virtual void writeOwnedObject(const string& name, const IO* obj) = 0;

  /** Store an entire object hierarchy, starting with the root object
      \a root. All objects and values referenced by \a root will be
      stored recursively, until there are no more remaining
      references. */
  virtual void writeRoot(const IO* root) = 0;
};

static const char vcid_writer_h[] = "$Header$";
#endif // !WRITER_H_DEFINED
