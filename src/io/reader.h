///////////////////////////////////////////////////////////////////////
//
// reader.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun  7 12:46:08 1999
// written: Wed Dec  1 11:44:27 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef READER_H_DEFINED
#define READER_H_DEFINED

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
 * ReadError should be thrown by subclasses of Reader when an error
 * occurs during a call to any of the Reader methods.
 *
 * @memo Exception class for errors in Reader methods.
 **/
///////////////////////////////////////////////////////////////////////

class ReadError : public ErrorWithMsg {
public:
  ///
  ReadError(const string& msg) : ErrorWithMsg(msg) {}
};


///////////////////////////////////////////////////////////////////////
/**
 *
 * Reader is the abstract interface that contains the methods that IO
 * objects can use when they deserialize themselves in a readFrom()
 * method. It provides the inverse operations to those in Writer. To
 * reconstruct an object tree, a client should call readRoot() on the
 * root object in that tree.
 *
 * @memo Provides the interface for IO objects to deserialize
 * themselves when they implement the IO interface readFrom()/writeTo().
 **/
///////////////////////////////////////////////////////////////////////

class Reader {
public:
  ///
  virtual ~Reader();

  ///
  virtual char readChar(const string& name) = 0;
  ///
  virtual int readInt(const string& name) = 0;
  ///
  virtual bool readBool(const string& name) = 0;
  ///
  virtual double readDouble(const string& name) = 0;

  ///
  virtual string readString(const string& name) = 0;
  ///
  virtual char* readCstring(const string& name) = 0;

  ///
  virtual void readValueObj(const string& name, Value& value) = 0;

  /** This template simply calls the appropriate readXxx() function
		for the type T */
  template <class T>
  void readValue(const string& name, T& return_value);

  /// A new object of the appropriate type will be created, if necessary.
  virtual IO* readObject(const string& name) = 0;

  /** The Reader will use the IO* provided here, rather than creating
      a new object. */
  virtual void readOwnedObject(const string& name, IO* obj) = 0;

  /** This function can either create a new root object (if 'root' is
		passed as 0), or use one that has already been constructed and
		passed as the 'root' argument. In either case, the function
		returns the root object that was actually read. */
  virtual IO* readRoot(IO* root=0) = 0;

protected:
  ///
  string makeNumberString(int number);
};

static const char vcid_reader_h[] = "$Header$";
#endif // !READER_H_DEFINED
