///////////////////////////////////////////////////////////////////////
//
// value.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Sep 28 11:19:17 1999
// written: Tue Nov 16 12:04:53 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef VALUE_H_DEFINED
#define VALUE_H_DEFINED

#ifndef STRING_DEFINED
#include <string>
#define STRING_DEFINED
#endif

#ifndef ERROR_H_DEFINED
#include "error.h"
#endif

class istream;
class ostream;

class ValueError : public ErrorWithMsg {
public:
  ValueError(const string& msg="");
};

///////////////////////////////////////////////////////////////////////
//
// Value abstract class definition
//
///////////////////////////////////////////////////////////////////////

class Value {
public:

  virtual ~Value();

  enum Type { NONE, INT, LONG, BOOL, DOUBLE, CSTRING, STRING, UNKNOWN };

  virtual Value* clone() const = 0;

  virtual Type getNativeType() const = 0;

  // Input/Output
  virtual void printTo(ostream& os) const = 0;
  virtual void scanFrom(istream& is) = 0;

  // Two sets of functions are provided to allow values to be
  // retrieved either as the return value, or as a reference
  // argument. Depending on a subclass's implementation, one or the
  // other type of function may be more efficient.

  virtual int getInt() const = 0;
  virtual long getLong() const = 0;
  virtual bool getBool() const = 0;
  virtual double getDouble() const = 0;
  virtual const char* getCstring() const = 0;
  virtual string getString() const = 0;

  virtual void get(int& val) const = 0;
  virtual void get(long& val) const = 0;
  virtual void get(bool& val) const = 0;
  virtual void get(double& val) const = 0;
  virtual void get(const char*& val) const = 0;
  virtual void get(string& val) const = 0;

  // The default implementations of the set functions all throw
  // exceptions, so that the default case is a read-only value.
  virtual void setInt(int val);
  virtual void setLong(long val);
  virtual void setBool(bool val);
  virtual void setDouble(double val);
  virtual void setCstring(const char* val);
  virtual void setString(const string& val);

  void set(int val) { setInt(val); }
  void set(long val) { setLong(val); }
  void set(bool val) { setBool(val); }
  void set(double val) { setDouble(val); }
  void set(const char* val) { setCstring(val); }
  void set(const string& val) { setString(val); }
}; 

///////////////////////////////////////////////////////////////////////
//
// Global stream insertion/extraction operators for Value's
//
///////////////////////////////////////////////////////////////////////

ostream& operator<<(ostream& os, const Value& val)
{
  val.printTo(os);
  return os;
}

istream& operator>>(istream& is, Value& val)
{
  val.scanFrom(is);
  return is;
}

///////////////////////////////////////////////////////////////////////
//
// TValue template class definition
//
///////////////////////////////////////////////////////////////////////

template <class T>
class TValue : public Value {
public:
  TValue(const T& val) : itsVal(val) {}

  virtual ~TValue ();

  virtual Value* clone() const;
  
  virtual Type getNativeType() const;

  virtual void printTo(ostream& os) const;
  virtual void scanFrom(istream& is);

  virtual int getInt() const;
  virtual long getLong() const;
  virtual bool getBool() const;
  virtual double getDouble() const;
  virtual const char* getCstring() const;
  virtual string getString() const;

  virtual void get(int& val) const;
  virtual void get(long& val) const;
  virtual void get(bool& val) const;
  virtual void get(double& val) const;
  virtual void get(const char*& val) const;
  virtual void get(string& val) const;

  virtual void setInt(int val);
  virtual void setLong(long val);
  virtual void setBool(bool val);
  virtual void setDouble(double val);
  virtual void setCstring(const char* val);
  virtual void setString(const string& val);

  T itsVal;
};

///////////////////////////////////////////////////////////////////////
//
// TValuePtr template class defintion
//
///////////////////////////////////////////////////////////////////////

template <class T>
class TValuePtr : public Value {
public:
  TValuePtr(T& valRef) : itsValPtr(&valRef) {}

  virtual ~TValuePtr();

  void reseat(T& valRef) { itsValPtr = &valRef; }

  virtual Value* clone() const;
  
  virtual Type getNativeType() const;

  virtual void printTo(ostream& os) const;
  virtual void scanFrom(istream& is);

  virtual int getInt() const;
  virtual long getLong() const;
  virtual bool getBool() const;
  virtual double getDouble() const;
  virtual const char* getCstring() const;
  virtual string getString() const;

  virtual void get(int& val) const;
  virtual void get(long& val) const;
  virtual void get(bool& val) const;
  virtual void get(double& val) const;
  virtual void get(const char*& val) const;
  virtual void get(string& val) const;

  virtual void setInt(int val);
  virtual void setLong(long val);
  virtual void setBool(bool val);
  virtual void setDouble(double val);
  virtual void setCstring(const char* val);
  virtual void setString(const string& val);

  T& operator()() { return *itsValPtr; }
  const T& operator()() const { return *itsValPtr; }

private:
  T* itsValPtr;
};

static const char vcid_value_h[] = "$Header$";
#endif // !VALUE_H_DEFINED
