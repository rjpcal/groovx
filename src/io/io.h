///////////////////////////////////////////////////////////////////////
//
// io.h
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Jan-99
// written: Fri Nov 10 17:23:54 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IO_H_DEFINED
#define IO_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(ERROR_H_DEFINED)
#include "util/error.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(REFCOUNTED_H_DEFINED)
#include "util/refcounted.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IODECLS_H_DEFINED)
#include "io/iodecls.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(CSTDLIB_DEFINED)
#include <cstdlib>
#define CSTDLIB_DEFINED
#endif

class fixed_string;

///////////////////////////////////////////////////////////////////////
/**
 *
 * The IO class defines the abstract interface for object
 * input/output. Classes which need to be read/written
 * should inherit virtually from IO.
 *
 **/
///////////////////////////////////////////////////////////////////////

class IO::IoObject : public RefCounted {
public:

  /// Class-specific operator new.
  void* operator new(size_t bytes);

  /// Class-specific operator delete.
  void operator delete(void* space, size_t bytes);

  /// Default constructor
  IoObject();

  /// Virtual destructor to ensure correct destruction of subclasses
  virtual ~IoObject() = 0;

  /** The preferred method for saving an object's state via the
      generic interface provided by \c IO::Reader. Each subclass of \c
      IO::Reader provides its own formatting scheme, so that subclasses of
      \c IO don't need to implement any formatting. */
  virtual void readFrom(IO::Reader* reader) = 0;

  /** The preferred method for restoring an object's state via the
      generic interface provided by \c IO::Writer. Each subclass of \c
      IO::Writer provides its own formatting scheme, so that subclasses of
      \c IO don't need to implement any formatting. */
  virtual void writeTo(IO::Writer* writer) const = 0;

  /** This function returns the number of attributes that are written
		in the object's \c writeTo() function. The default
		implementation simply calls writeTo() with a dummy Writer and
		counts how many attributes are written. However, this is
		somewhat inefficient since it defers the counting to runtime
		when the number may in fact be known at compile time. Thus
		subclasses may which to override this function to return a
		compile-time constant. **/
  virtual unsigned int ioAttribCount() const;

  /** Returns a unique id for this object. */
  IO::UID id() const;

  /** Returns a serialization version id for the class. The default
      implementation returns zero. Classes should override this when
      they make a change that requires a change to their serialization
      protocol. Overriding versions of this function should follow the
      convention that a higher id refers to a later version of the
      class. Implementations of \c IO::Reader and \c IO::Writer will provide a
      way for a class to store and retrieve the serialization version
      of an object. */
  virtual IO::VersionId serialVersionId() const;

  /** Returns a string specifying the typename of the actual
		object. The implementation provided by \c IO returns a demangled
		version of \c typeid(*this).name(), which should very closely
		resemble the way the object was declared in source code. */
  virtual fixed_string ioTypename() const;

private:
  unsigned long itsId;
};



///////////////////////////////////////////////////////////////////////
/**
 *
 * \defgroup IoExcept Input/OutputException Classes
 *
 * These exception classes represent the various types of errors that
 * may occur during IO operations. The base class for this group of
 * classes is IoError. All IO-related exception classes defined by
 * clients should also derive from IoError.
 *
 **/
///////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////
/**
 * \ingroup IoExcept
 * IoError is base class for the various exception types that may be
 * thrown in IO's input and output methods. All IO-related exception
 * classes defined by clients should also derive from IoError.
 *
 **/
///////////////////////////////////////////////////////////////////////

class IO::IoError : public ErrorWithMsg {
public:
  /// Default constructor
  IoError();
  /// Construct, and call \c setMsg(message)
  IoError(const char* message);
  /// Virtual destructor
  virtual ~IoError();
protected:
  /// Change the message associated with the exception to \a newMessage
  virtual void setMsg(const char* newMessage);
};

/**
 * \ingroup IoExcept
 *
 * A subclass of IoError for general input errors. 
 **/
class IO::InputError : public IO::IoError {
public:
  ///
  InputError() : IoError() {}
  ///
  InputError(const char* str);
  /// Virtual destructor
  virtual ~InputError();
};

/**
 * \ingroup IoExcept
 *
 * A subclass of IoError for general output errors. \ingroup IoExcept
 **/
class IO::OutputError : public IO::IoError {
public:
  ///
  OutputError() : IoError() {}
  ///
  OutputError(const char* str);
  /// Virtual destructor
  virtual ~OutputError();
};

/**
 * \ingroup IoExcept
 *
 * A subclass of IoError for "can't-happen" errors that are discovered
 * during input or output. This type of error suggests * that either
 * the object or its legacySrlzd description has been * corrupted.
 **/
class IO::LogicError : public IO::IoError {
public:
  ///
  LogicError() : IoError() {}
  ///
  LogicError(const char* str);
  /// Virtual destructor
  virtual ~LogicError();
};

/**
 * \ingroup IoExcept
 *
 * A subclass of IoError for out-of-bounds values that are encountered
 * during input or output.
 **/
class IO::ValueError : public IO::IoError {
public:
  ///
  ValueError() : IoError() {}
  ///
  ValueError(const char* str);
  /// Virtual destructor
  virtual ~ValueError();
};

/**
 * \ingroup IoExcept
 *
 * A subclass of IoError for invalid filenames or filenames referring
 * to inaccessible files.
 **/
class IO::FilenameError : public IO::IoError {
public:
  ///
  FilenameError() : IoError() {}
  ///
  FilenameError(const char* filename);
  /// Virtual destructor
  virtual ~FilenameError();
};

static const char vcid_io_h[] = "$Header$";
#endif // !IO_H_DEFINED
