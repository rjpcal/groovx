///////////////////////////////////////////////////////////////////////
//
// io.h
// Rob Peters 
// created: Jan-99
// written: Thu Sep 28 10:49:54 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IO_H_DEFINED
#define IO_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(ERROR_H_DEFINED)
#include "util/error.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IODECLS_H_DEFINED)
#include "io/iodecls.h"
#endif

#ifdef PRESTANDARD_IOSTREAMS
class istream;
class ostream;
#else
#  if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IOSFWD_DEFINED)
#    include <iosfwd>
#    define IOSFWD_DEFINED
#  endif
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TYPEINFO_DEFINED)
#include <typeinfo>
#define TYPEINFO_DEFINED
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

class IO::IoObject {
public:

  /// Default constructor
  IoObject();

  /// Virtual destructor to ensure correct destruction of subclasses
  virtual ~IoObject() = 0;

  void ioSerialize(STD_IO::ostream& os, IO::IOFlag flag) const;

  void ioDeserialize(STD_IO::istream& is, IO::IOFlag flag);

  int ioCharCount() const;

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

  /** Returns a unique id for this object. This function should NEVER
      be overridden by public clients; it will only be overridden by
      other classes internal to the io system, such as \c IO::IoProxy for
      example. */
  virtual unsigned long id() const;

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

  /** This function removes all whitespace characters from \a
		theStream, until the first non-whitespace character is seen. It
		returns the number of whitespace characters that were removed
		from \a theStream. */
  static int eatWhitespace(STD_IO::istream& theStream);

  /** The old-style function to send an object to a stream. Each
      subclass must implement its own formatting. \c writeTo() should
      be favored over \c legacySrlz(). */
  virtual void legacySrlz(IO::Writer* writer) const;

  /** The old-style function to read an object from a stream. Each
      subclass must implement its own formatting. \c readFrom() should
      be favored over \c legacyDesrlz(). */
  virtual void legacyDesrlz(IO::Reader* reader);

  virtual fixed_string legacyIoTypename() const;

  virtual fixed_string legacyValidTypenames() const;

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
  /// Construct, and call \c setMsg(typeInfo)
  IoError(const std::type_info& typeInfo);
  /// Virtual destructor
  virtual ~IoError();
protected:
  /// Change the message associated with the exception to \a newMessage
  virtual void setMsg(const char* newMessage);
  /// Change the message associated with the exception to \c typeInfo.name()
  virtual void setMsg(const std::type_info& typeInfo);
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
  ///
  InputError(const std::type_info& ti) { setMsg(ti); }
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
  ///
  OutputError(const std::type_info& ti) { setMsg(ti); }
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
  ///
  LogicError(const std::type_info& ti) { setMsg(ti); }
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
  ///
  ValueError(const std::type_info& ti) { setMsg(ti); }
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
