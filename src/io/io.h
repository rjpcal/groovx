///////////////////////////////////////////////////////////////////////
//
// io.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Jan-99
// written: Tue Jun  5 10:34:00 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IO_H_DEFINED
#define IO_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(ERROR_H_DEFINED)
#include "util/error.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(OBJECT_H_DEFINED)
#include "util/object.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IODECLS_H_DEFINED)
#include "io/iodecls.h"
#endif

class fixed_string;

///////////////////////////////////////////////////////////////////////
/**
 *
 * The IO::IoObject class defines the abstract interface for object
 * persistence. Classes which need these facilities should inherit
 * virtually from IO::IoObject. IO::IoObject is reference counted (by
 * subclassing Util::Object), providing automatic memory management
 * when IO::IoObject's are managed with a smart pointer that manages
 * the reference count, such as PtrHandle or IdItem.
 *
 **/
///////////////////////////////////////////////////////////////////////

class IO::IoObject : public Util::Object {
protected:
  /** Default constructor. */
  IoObject();

  /// Virtual destructor to ensure correct destruction of subclasses
  virtual ~IoObject() = 0;

public:
  /** Subclasses implement this method to save the object's state via
      the generic interface provided by \c IO::Reader. Parsing the
      format of the input is handled by the \c IO::Reader, so
      implementors of \c readFrom() of don't need to deal with
      formatting. */
  virtual void readFrom(IO::Reader* reader) = 0;

  /** Subclasses implement this method to restore the object's state
      via the generic interface provided by \c IO::Writer. Formatting
      the output is handled by the \c IO::Writer, so implementors of
      \c writeTo() of don't need to deal with formatting. */
  virtual void writeTo(IO::Writer* writer) const = 0;

  /** Returns the number of attributes that are written in the
		object's \c writeTo() function. The default implementation
		simply calls writeTo() with a dummy Writer and counts how many
		attributes are written. However, this is somewhat inefficient
		since it defers the counting to runtime when the number may in
		fact be known at compile time. Thus subclasses may wish to
		override this function to return a compile-time constant. */
  virtual unsigned int ioAttribCount() const;

  /** Returns a serialization version id for the class. The default
      implementation returns zero. Classes should override this when
      they make a change that requires a change to their serialization
      protocol. Overriding versions of this function should follow the
      convention that a higher id refers to a later version of the
      class. Implementations of \c IO::Reader and \c IO::Writer will
      provide a way for a class to store and retrieve the
      serialization version of an object. */
  virtual IO::VersionId serialVersionId() const;

  /** Returns the typename of the full object. The implementation
		provided by \c IO returns a demangled version of \c
		typeid(*this).name(), which should very closely resemble the way
		the object was declared in source code. */
  virtual fixed_string ioTypename() const;
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
