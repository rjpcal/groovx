///////////////////////////////////////////////////////////////////////
//
// writer.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun  7 12:49:49 1999
// written: Tue Oct 19 16:02:15 1999
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
//
// WriterError class definition
//
///////////////////////////////////////////////////////////////////////

class WriteError : public ErrorWithMsg {
public:
  WriteError(const string& msg) : ErrorWithMsg(msg) {}
};

///////////////////////////////////////////////////////////////////////
//
// Writer abstract class definition
//
///////////////////////////////////////////////////////////////////////

class Writer {
public:
  virtual ~Writer();

  virtual void writeChar(const string& name, char val) = 0;
  virtual void writeInt(const string& name, int val) = 0;
  virtual void writeBool(const string& name, bool val) = 0;
  virtual void writeDouble(const string& name, double val) = 0;

  virtual void writeString(const string& name, const string& val) = 0;
  virtual void writeCstring(const string& name, const char* val) = 0;

  virtual void writeValueObj(const string& name, const Value& value) = 0;

  template <class T>
  void writeValue(const string& name, const T& val);

  virtual void writeObject(const string& name, const IO* obj) = 0;

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

  virtual void writeRoot(const IO* root) = 0;

protected:
  string makeNumberString(int number);
};

static const char vcid_writer_h[] = "$Header$";
#endif // !WRITER_H_DEFINED
