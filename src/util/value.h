///////////////////////////////////////////////////////////////////////
//
// value.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Sep 28 11:19:17 1999
// written: Sat Sep 23 15:38:01 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef VALUE_H_DEFINED
#define VALUE_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(ERROR_H_DEFINED)
#include "util/error.h"
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

/**
 *
 * Exception class to be thrown from failed calls to get/set a \c
 * Value object.
 *
 **/
class ValueError : public ErrorWithMsg {
public:
  /// Default constructor.
  ValueError();
  /// Construct with an informative message \c msg.
  ValueError(const char* msg);
  /// Virtual destructor.
  virtual ~ValueError();
};

///////////////////////////////////////////////////////////////////////
//
// Value abstract class definition
//
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c Value defines an interface for variant-type value
 * objects. Clients can attempt to retrieve values of the basic types
 * from a \c Value object. If such an operation fails, \c ValueError
 * is thrown. Subclasses may allow conversion among types so that a
 * value of any basic type can be retrieved, or they may allow only
 * one basic type to be retrieved.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Value {
public:

  /// Virtual destructor ensures proper destruction of base classes.
  virtual ~Value();

  /// Returns a new dynamically allocated copy of the calling object.
  virtual Value* clone() const = 0;

  /** The symbolic constants of type \c Type can be used by subclasses
      to indicate the native type of their underlying representation. */
  enum Type { NONE, INT, LONG, BOOL, DOUBLE, CSTRING, UNKNOWN };

  /** Return the \c Type constant indicating the native type used in
      the implementation. */
  virtual Type getNativeType() const = 0;

  /// Return a string giving the name of the native type.
  virtual const char* getNativeTypeName() const = 0;

  /// Write the value to an \c STD_IO::ostream.
  virtual void printTo(STD_IO::ostream& os) const;

  /// Read the value from an \c STD_IO::istream.
  virtual void scanFrom(STD_IO::istream& is);

  // Two sets of functions are provided to allow values to be
  // retrieved either as the return value, or as a reference
  // argument. Depending on a subclass's implementation, one or the
  // other type of function may be more efficient.

  /// Attempt to get an \c int representation of the value.
  virtual int getInt() const;
  /// Attempt to get a \c long representation of the value.
  virtual long getLong() const;
  /// Attempt to get a \c bool representation of the value.
  virtual bool getBool() const;
  /// Attempt to get a \c double representation of the value.
  virtual double getDouble() const;
  /// Attempt to get a C-style string (\c char*) representation of the value.
  virtual const char* getCstring() const;

  /// Attempt to get an \c int representation of the value.
  virtual void get(int& val) const;
  /// Attempt to get a \c long representation of the value.
  virtual void get(long& val) const;
  /// Attempt to get a \c bool representation of the value.
  virtual void get(bool& val) const;
  /// Attempt to get a \c double representation of the value.
  virtual void get(double& val) const;
  /// Attempt to get a C-style string (\c char*) representation of the value.
  virtual void get(const char*& val) const;

  // The default implementations of the set functions all throw
  // exceptions, so that the default case is a read-only value.

  /// Attempt to set the value from an \c int representation.
  virtual void setInt(int val);
  /// Attempt to set the value from a \c long representation.
  virtual void setLong(long val);
  /// Attempt to set the value from a \c bool representation.
  virtual void setBool(bool val);
  /// Attempt to set the value from a \c double representation.
  virtual void setDouble(double val);
  /// Attempt to set the value from a C-style string (\c char*) representation.
  virtual void setCstring(const char* val);

  /// Attempt to set the value from an \c int representation.
  void set(int val) { setInt(val); }
  /// Attempt to set the value from a \c long representation.
  void set(long val) { setLong(val); }
  /// Attempt to set the value from a \c bool representation.
  void set(bool val) { setBool(val); }
  /// Attempt to set the value from a \c double representation.
  void set(double val) { setDouble(val); }
  /// Attempt to set the value from a C-style string (\c char*) representation.
  void set(const char* val) { setCstring(val); }
}; 

///////////////////////////////////////////////////////////////////////
//
// Global stream insertion/extraction operators for Value's
//
///////////////////////////////////////////////////////////////////////

inline STD_IO::ostream& operator<<(STD_IO::ostream& os, const Value& val)
{
  val.printTo(os);
  return os;
}

inline STD_IO::istream& operator>>(STD_IO::istream& is, Value& val)
{
  val.scanFrom(is);
  return is;
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c TValue provides a simple templated implementation of \c Value
 * for the basic types. All attempts to get or set a \c TValue from a
 * type other than the template type \c T will fail with a \c
 * ValueError exception being thrown.
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class T>
class TValue : public Value {
public:
  /// Construct with an initial value \a val.
  TValue(const T& val) : itsVal(val) {}

  /// Copy constructor.
  TValue(const TValue& other) :
	 Value(other), itsVal(other.itsVal) {}

  /// Assignment operator.
  TValue& operator=(const TValue& other)
	 { itsVal = other.itsVal; return *this; }

  /// Virtual destructor.
  virtual ~TValue ();

  virtual Value* clone() const;
  
  virtual Type getNativeType() const;

  virtual const char* getNativeTypeName() const;

  virtual void printTo(STD_IO::ostream& os) const;
  virtual void scanFrom(STD_IO::istream& is);

  virtual int getInt() const;
  virtual long getLong() const;
  virtual bool getBool() const;
  virtual double getDouble() const;
  virtual const char* getCstring() const;

  virtual void get(int& val) const;
  virtual void get(long& val) const;
  virtual void get(bool& val) const;
  virtual void get(double& val) const;
  virtual void get(const char*& val) const;

  virtual void setInt(int val);
  virtual void setLong(long val);
  virtual void setBool(bool val);
  virtual void setDouble(double val);
  virtual void setCstring(const char* val);

  /** Publicly accessible lone data member allows efficient access to
      those who know the true type of the object. */
  T itsVal;
};


///////////////////////////////////////////////////////////////////////
/**
 *
 * \c TValuePtr provides a simple templated implementation of \c Value
 * for the basic types. As in \c TValue, all attempts to get or set a
 * \c TValuePtr from a type other than the template type \c T will
 * fail with a \c ValueError exception being thrown. Internally, \c
 * TValuePtr stores a \c T*, so that the \c TValuePtr can be reseated
 * after construction to point to a different \c T.
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class T>
class TValuePtr : public Value {
public:
  /// Construct and refer to \c valRef.
  TValuePtr(T& valRef) : itsValPtr(&valRef) {}

  /// Copy constructor.
  TValuePtr(const TValuePtr& other) :
	 Value(other), itsValPtr(other.itsValPtr) {}

  /// Assignment operator.
  TValuePtr& operator=(const TValuePtr& other)
	 { itsValPtr = other.itsValPtr; return *this; }

  /// Virtual destructor.
  virtual ~TValuePtr();

  /// Reseat the internal pointer to refer to \a valRef.
  void reseat(T& valRef) { itsValPtr = &valRef; }

  virtual Value* clone() const;
  
  virtual Type getNativeType() const;

  virtual const char* getNativeTypeName() const;

  virtual void printTo(STD_IO::ostream& os) const;
  virtual void scanFrom(STD_IO::istream& is);

  virtual int getInt() const;
  virtual long getLong() const;
  virtual bool getBool() const;
  virtual double getDouble() const;
  virtual const char* getCstring() const;

  virtual void get(int& val) const;
  virtual void get(long& val) const;
  virtual void get(bool& val) const;
  virtual void get(double& val) const;
  virtual void get(const char*& val) const;

  virtual void setInt(int val);
  virtual void setLong(long val);
  virtual void setBool(bool val);
  virtual void setDouble(double val);
  virtual void setCstring(const char* val);

  /// Return a reference to the currently pointed-to \c T object.
  T& operator()() { return *itsValPtr; }

  /// Return a const reference to the currently pointed-to \c T object.
  const T& operator()() const { return *itsValPtr; }

private:
  T* itsValPtr;
};

static const char vcid_value_h[] = "$Header$";
#endif // !VALUE_H_DEFINED
