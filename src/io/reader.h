///////////////////////////////////////////////////////////////////////
//
// reader.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun  7 12:46:08 1999
// written: Thu Nov 11 11:56:32 1999
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


#ifndef ACC_COMPILER
class IO;
#else
// For some reason aCC wants to see the full definition of class IO
// before parsing the dynamic_cast<C*>(IO*) in readObjectSeq
#  ifndef IO_H_DEFINED
#  include "io.h"
#  endif
#endif

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

  /** Provides a generic interface for handling containers, sequences,
		etc. of value types. */
  template <class Inserter, class T>
  void readValueSeq(const string& name, Inserter inserter, T* = 0) {
	 int count = readInt(name+"Count");

	 for (int i = 0; i < count; ++i) {
		T temp;
		readValue(name+makeNumberString(i), temp);
		*inserter = temp;
		++inserter;
	 }
  }

  /** Provides a generic interface for handling containers, sequences,
      etc. of IO objects */
  template <class Inserter, class C>
  void readObjectSeq(const string& name, Inserter inserter, C* /*dummy*/) {
	 int count = readInt(name+"Count");

	 for (int i = 0; i < count; ++i) {
		IO* io = readObject(name+makeNumberString(i));

		if (io == 0) {
		  *inserter = static_cast<C*>(0);
		}

		else {
		  C* obj = dynamic_cast<C*>(io);
		  if (obj == 0) throw ReadError("failed cast in readObjectSeq");
		  *inserter = obj;
		}

	 }
  }

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
