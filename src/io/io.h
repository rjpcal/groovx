///////////////////////////////////////////////////////////////////////
//
// io.h
// Rob Peters 
// created: Jan-99
// written: Tue Oct 19 14:25:04 1999
// $Id$
//
// This file defines the IO abstract interface. This interface
// provides the framwork necessary for object I/O. Classes which need
// to be serialized/deserialized should inherit virtually from
// IO. Also defined in this file are the various exception types that
// may be thrown in serialize and deserialize methods. All IO-related
// exception classes are derived from IoError.
//
///////////////////////////////////////////////////////////////////////

#ifndef IO_H_DEFINED
#define IO_H_DEFINED

#ifndef STRING_DEFINED
#include <string>
#define STRING_DEFINED
#endif

#ifndef CMATH_DEFINED
#include <cmath>
#define CMATH_DEFINED
#endif

#ifndef ERROR_H_DEFINED
#include "error.h"
#endif

class istream;
class ostream;

class Reader;
class Writer;

class type_info;

///////////////////////////////////////////////////////////////////////
//
// IO class -- abstract interface definition
//
///////////////////////////////////////////////////////////////////////

class IO {
public:
  // The following flags may be OR'ed together and passed to the flag
  // argument of serialize/deserialize to control different aspects of
  // the formatting used to read and write objects. In general, the
  // same flags must be used to read an object as were used to write
  // it.
  typedef int IOFlag;
  static const IOFlag NO_FLAGS   = 0;
  static const IOFlag TYPENAME   = 1 << 0; // The class's name is written/read
  static const IOFlag BASES      = 1 << 1; // The class's bases is written/read

  // A default separator to be used between elements in a serialized object.
  static const char SEP = ' ';

  IO();
  virtual ~IO() = 0;

  // "Old" IO functions
  virtual void serialize(ostream& os, IOFlag flag) const = 0;
  virtual void deserialize(istream& is, IOFlag flag) = 0;

  virtual int charCount() const = 0;

  // "New" IO functions
  virtual void readFrom(Reader* reader);
  virtual void writeTo(Writer* writer) const;

  // Returns a unique id for this object
  unsigned long id() const;

  // Returns a string specifying the typename of the actual
  // object. The implementation provided by IO returns a demangled
  // version of typeid(*this).name(), which should very closely
  // resemble the way the object was declared in source code.
  virtual string ioTypename() const;

  // This function removes all whitespace characters from the istream,
  // until the first non-whitespace character is seen. It returns the
  // number of whitespace characters that were removed from the stream.
  static int eatWhitespace(istream& is);

  // This function reads the next word from the istream is, and
  // compares it to the correct names in correctNames. correctNames
  // should consist of one or more space-separated words. If the word
  // read from the stream does not match any of these words, and
  // doCheck is true, an InputError is thrown with the last word in
  // correctNames as its info. If doCheck is false, the function does
  // nothing except read a word from the istream.
  static void readTypename(istream& is,
									const string& correctNames,
									bool doCheck = true);

private:
  unsigned long itsId;
};

// This template function returns the number of characters needed to
// write a type using the << insertion operator. It is instantiated
// for the basic types.
template<class T>
int gCharCount(T val);

///////////////////////////////////////////////////////////////////////
//
// IO exception classes
//
///////////////////////////////////////////////////////////////////////

class IoError : public ErrorWithMsg {
public:
  IoError();
  IoError(const string& str);
  IoError(const type_info& ti);
protected:
  virtual void setMsg(const string& str);
  virtual void setMsg(const type_info& ti);
};

class InputError : public IoError {
public:
  InputError() : IoError() {}
  InputError(const string& str) { setMsg(str); }
  InputError(const type_info& ti) { setMsg(ti); }
};

class OutputError : public IoError {
public:
  OutputError() : IoError() {}
  OutputError(const string& str) { setMsg(str); }
  OutputError(const type_info& ti) { setMsg(ti); }
};

class IoLogicError : public IoError {
public:
  IoLogicError() : IoError() {}
  IoLogicError(const string& str) { setMsg(str); }
  IoLogicError(const type_info& ti) { setMsg(ti); }
};

class IoValueError : public IoError {
public:
  IoValueError() : IoError() {}
  IoValueError(const string& str) { setMsg(str); }
  IoValueError(const type_info& ti) { setMsg(ti); }
};

class IoFilenameError : public IoError {
public:
  IoFilenameError() : IoError() {}
  IoFilenameError(const string& filename) { setMsg(filename); }
};

static const char vcid_io_h[] = "$Header$";
#endif // !IO_H_DEFINED
