///////////////////////////////////////////////////////////////////////
//
// io.h
// Rob Peters 
// created: Jan-99
// written: Wed May 26 21:23:04 1999
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

class istream; class ostream;
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

  virtual ~IO() = 0;
  virtual void serialize(ostream& os, IOFlag flag) const = 0;
  virtual void deserialize(istream& is, IOFlag flag) = 0;

  virtual int charCount() const = 0;
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

class IoError {
public:
  IoError() : itsInfo() {}
  IoError(const string& str);
  IoError(const type_info& ti);
  virtual const string& info() { return itsInfo; }
protected:
  void setInfo(const string& str);
  void setInfo(const type_info& ti);
private:
  string itsInfo;
};

class InputError : public IoError {
public:
  InputError() : IoError() {}
  InputError(const string& str) { setInfo(str); }
  InputError(const type_info& ti) { setInfo(ti); }
};

class OutputError : public IoError {
public:
  OutputError() : IoError() {}
  OutputError(const string& str) { setInfo(str); }
  OutputError(const type_info& ti) { setInfo(ti); }
};

class IoLogicError : public IoError {
public:
  IoLogicError() : IoError() {}
  IoLogicError(const string& str) { setInfo(str); }
  IoLogicError(const type_info& ti) { setInfo(ti); }
};

class IoValueError : public IoError {
public:
  IoValueError() : IoError() {}
  IoValueError(const string& str) { setInfo(str); }
  IoValueError(const type_info& ti) { setInfo(ti); }
};

class IoFilenameError : public IoError {
public:
  IoFilenameError() : IoError() {}
  IoFilenameError(const string& filename) { setInfo(filename); }
};

static const char vcid_io_h[] = "$Header$";
#endif // !IO_H_DEFINED
