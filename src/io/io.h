///////////////////////////////////////////////////////////////////////
//
// io.h
// Rob Peters 
// created: Jan-99
// written: Sat Mar  4 14:37:27 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IO_H_DEFINED
#define IO_H_DEFINED

#ifndef STRINGFWD_H_DEFINED
#include "stringfwd.h"
#endif

#ifndef ERROR_H_DEFINED
#include "util/error.h"
#endif

class istream;
class ostream;

class Reader;
class Writer;

class type_info;

///////////////////////////////////////////////////////////////////////
/**
 *
 * The IO class defines the abstract interface for object
 * input/output. Classes which need to be read/written
 * should inherit virtually from IO.
 *
 **/
///////////////////////////////////////////////////////////////////////

class IO {
public:

  /** The symbolic constants of type \c IOFlag flags may be OR'ed
   * together and passed to the flag argument of \c serialize() or \c
   * deserialize() to control different aspects of the formatting used
   * to read and write objects. In general, the same flags must be
   * used to read an object as were used to write it. */
  typedef int IOFlag;

  /// Neither the class's name or its bases will be written/read
  static const IOFlag NO_FLAGS   = 0;
  /// The class's name is written/read
  static const IOFlag TYPENAME   = 1 << 0;
  /// The class's bases are written/read
  static const IOFlag BASES      = 1 << 1;

  /// A default separator to be used between elements in a serialized object.
  static const char SEP = ' ';

  /// Default constructor
  IO();

  /// Virtual destructor to ensure correct destruction of subclasses
  virtual ~IO() = 0;

  /** The old-style function to send an object to a stream. Each
      subclass must implement its own formatting. \c writeTo() should
      be favored over \c serialize(). */
  virtual void serialize(ostream& os, IOFlag flag) const;

  /** The old-style function to read an object from a stream. Each
      subclass must implement its own formatting. \c readFrom() should
      be favored over \c deserialize(). */
  virtual void deserialize(istream& is, IOFlag flag);

  /** The old-style function to return an upper limit on the number of
      characters that will be sent to a stream by \c serialize(). Each
      subclass must implement its own formatting. */
  virtual int charCount() const;

  /** The preferred method for saving an object's state via the
      generic interface provided by \c Reader. Each subclass of \c
      Reader provides its own formatting scheme, so that subclasses of
      \c IO don't need to implement any formatting. */
  virtual void readFrom(Reader* reader) = 0;

  /** The preferred method for restoring an object's state via the
      generic interface provided by \c Writer. Each subclass of \c
      Writer provides its own formatting scheme, so that subclasses of
      \c IO don't need to implement any formatting. */
  virtual void writeTo(Writer* writer) const = 0;
  
  /// Returns a unique id for this object
  unsigned long id() const;

  /** Returns a string specifying the typename of the actual
		object. The implementation provided by \c IO returns a demangled
		version of \c typeid(*this).name(), which should very closely
		resemble the way the object was declared in source code. */
  virtual string ioTypename() const;

  /** This function removes all whitespace characters from \a
		theStream, until the first non-whitespace character is seen. It
		returns the number of whitespace characters that were removed
		from \a theStream. */
  static int eatWhitespace(istream& theStream);

  /** This function reads the next word from \a theStream, and
      compares it to the correct names in \a correctNames. \a
      correctNames should consist of one or more space-separated
      words. If the word read from the stream does not match any of
      these words, and \a doCheck is true, an \c InputError is thrown
      with the last word in correctNames as its info. If \a doCheck is
      false, the function does nothing except read a word from \a
      theStream. */
  static void readTypename(istream& theStream,
									const string& correctNames,
									bool doCheck = true);

  static void readTypename(istream& theStream,
									const char* correctNames,
									bool doCheck = true);

private:
  unsigned long itsId;
};




///////////////////////////////////////////////////////////////////////
/**
 *
 * This template function returns the number of characters needed to
 * write a type using the << insertion operator. It is instantiated
 * for the basic types.
 *
 * @short Gives the number of characters for a string representation of
 * a basic type.
 **/
///////////////////////////////////////////////////////////////////////
template<class T>
int gCharCount(T val);




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

class IoError : public ErrorWithMsg {
public:
  /// Default constructor
  IoError();
  /// Construct, and call \c setMsg(message)
  IoError(const string& message);
  /// Construct, and call \c setMsg(typeInfo)
  IoError(const type_info& typeInfo);
protected:
  /// Change the message associated with the exception to \a newMessage
  virtual void setMsg(const string& newMessage);
  /// Change the message associated with the exception to \c typeInfo.name()
  virtual void setMsg(const type_info& typeInfo);
};

/**
 * \ingroup IoExcept
 *
 * A subclass of IoError for general input errors. 
 **/
class InputError : public IoError {
public:
  ///
  InputError() : IoError() {}
  ///
  InputError(const char* str);
  ///
  InputError(const string& str) { setMsg(str); }
  ///
  InputError(const type_info& ti) { setMsg(ti); }
};

/**
 * \ingroup IoExcept
 *
 * A subclass of IoError for general output errors. \ingroup IoExcept
 **/
class OutputError : public IoError {
public:
  ///
  OutputError() : IoError() {}
  ///
  OutputError(const char* str);
  ///
  OutputError(const string& str) { setMsg(str); }
  ///
  OutputError(const type_info& ti) { setMsg(ti); }
};

/**
 * \ingroup IoExcept
 *
 * A subclass of IoError for "can't-happen" errors that are discovered
 * during input or output. This type of error suggests * that either
 * the object or its serialized description has been * corrupted.
 **/
class IoLogicError : public IoError {
public:
  ///
  IoLogicError() : IoError() {}
  ///
  IoLogicError(const char* str);
  ///
  IoLogicError(const string& str) { setMsg(str); }
  ///
  IoLogicError(const type_info& ti) { setMsg(ti); }
};

/**
 * \ingroup IoExcept
 *
 * A subclass of IoError for out-of-bounds values that are encountered
 * during input or output.
 **/
class IoValueError : public IoError {
public:
  ///
  IoValueError() : IoError() {}
  ///
  IoValueError(const char* str);
  ///
  IoValueError(const string& str) { setMsg(str); }
  ///
  IoValueError(const type_info& ti) { setMsg(ti); }
};

/**
 * \ingroup IoExcept
 *
 * A subclass of IoError for invalid filenames or filenames referring
 * to inaccessible files.
 **/
class IoFilenameError : public IoError {
public:
  ///
  IoFilenameError() : IoError() {}
  ///
  IoFilenameError(const char* filename);
  ///
  IoFilenameError(const string& filename) { setMsg(filename); }
};

static const char vcid_io_h[] = "$Header$";
#endif // !IO_H_DEFINED
