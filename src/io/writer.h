///////////////////////////////////////////////////////////////////////
//
// writer.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun  7 12:49:49 1999
// written: Thu Nov 11 11:50:32 1999
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
 * WriteError should be thrown by subclasses of Writer when an error
 * occurs during a call to any of the methods in the Writer interface.
 *
 * @memo Exception class for errors in Writer methods.
 **/
///////////////////////////////////////////////////////////////////////

class WriteError : public ErrorWithMsg {
public:
  ///
  WriteError(const string& msg) : ErrorWithMsg(msg) {}
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * Writer is the abstract interface that contains the methods that IO
 * objects can use when they serialize themselves in a writeTo()
 * method. It provides the inverse operations to those in Reader. To
 * write an object tree, a client should call writeRoot() on the root
 * object in that tree.
 *
 * @memo Provides the interface for IO objects to serialize themselves
 * when they implement the IO interface readFrom()/writeTo().
 **/
///////////////////////////////////////////////////////////////////////

class Writer {
public:
  ///
  virtual ~Writer();

  ///
  virtual void writeChar(const string& name, char val) = 0;
  ///
  virtual void writeInt(const string& name, int val) = 0;
  ///
  virtual void writeBool(const string& name, bool val) = 0;
  ///
  virtual void writeDouble(const string& name, double val) = 0;

  ///
  virtual void writeString(const string& name, const string& val) = 0;
  ///
  virtual void writeCstring(const string& name, const char* val) = 0;

  ///
  virtual void writeValueObj(const string& name, const Value& value) = 0;

  /** This template simply calls the appropriate writeXxx() function
      for the template type T */
  template <class T>
  void writeValue(const string& name, const T& val);

  ///
  virtual void writeObject(const string& name, const IO* obj) = 0;

  ///
  virtual void writeOwnedObject(const string& name, const IO* obj) = 0;

  /** A generic interface for handling containers, sequences, etc. of
      value types. */
  template <class Itr>
  void writeValueSeq(const string& name, Itr begin, Itr end) {
	 int count = 0;

	 while (begin != end) {
		writeValue(name+makeNumberString(count), *begin);
		++count;
		++begin;
	 }

	 writeValue(name+"Count", count);
  }

  /// A generic interface for handling containers, sequences, etc. of objects
  template <class Itr>
  void writeObjectSeq(const string& name, Itr begin, Itr end) {
	 int count = 0;
	 
	 while (begin != end) {
		writeObject(name+makeNumberString(count), *begin);
		++count;
		++begin;
	 }

	 writeValue(name+"Count", count);
  }

  ///
  virtual void writeRoot(const IO* root) = 0;

protected:
  ///
  string makeNumberString(int number);
};

static const char vcid_writer_h[] = "$Header$";
#endif // !WRITER_H_DEFINED
