///////////////////////////////////////////////////////////////////////
//
// value.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Sep 28 11:19:17 1999
// written: Thu Aug  9 07:08:50 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef VALUE_H_DEFINED
#define VALUE_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(ERROR_H_DEFINED)
#include "util/error.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TRAITS_H_DEFINED)
#include "util/traits.h"
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
class ValueError : public Util::Error {
public:
  ValueError(const fstring& msg) : Util::Error(msg) {};

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

  /// Default constructor.
  Value();

  /// Virtual destructor ensures proper destruction of base classes.
  virtual ~Value();

  /// Returns a new dynamically allocated copy of the calling object.
  virtual Value* clone() const = 0;

  /// Return a string giving the name of the native type.
  virtual const char* getNativeTypeName() const = 0;

  /// Write the value to an \c STD_IO::ostream.
  virtual void printTo(STD_IO::ostream& os) const;

  /// Read the value from an \c STD_IO::istream.
  virtual void scanFrom(STD_IO::istream& is);

  /// Attempt to get an \c int representation of the value.
  virtual int get(Util::TypeCue<int>) const;
  /// Attempt to get a \c long representation of the value.
  virtual long get(Util::TypeCue<long>) const;
  /// Attempt to get a \c bool representation of the value.
  virtual bool get(Util::TypeCue<bool>) const;
  /// Attempt to get a \c double representation of the value.
  virtual double get(Util::TypeCue<double>) const;
  /// Attempt to get a C-style string (\c char*) representation of the value.
  virtual const char* get(Util::TypeCue<const char*>) const;

  // The default implementations of the set functions all throw
  // exceptions, so that the default case is a read-only value.

  /// Attempt to set the value from an \c int representation.
  virtual void set(int val);
  /// Attempt to set the value from a \c long representation.
  virtual void set(long val);
  /// Attempt to set the value from a \c bool representation.
  virtual void set(bool val);
  /// Attempt to set the value from a \c double representation.
  virtual void set(double val);
  /// Attempt to set the value from a C-style string (\c char*) representation.
  virtual void set(const char* val);

  /// Assign \a this's value to \a other, using the native type of \a this.
  virtual void assignTo(Value& other) const = 0;
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

  virtual const char* getNativeTypeName() const;

  virtual void printTo(STD_IO::ostream& os) const;
  virtual void scanFrom(STD_IO::istream& is);

  virtual int get(Util::TypeCue<int>) const;
  virtual long get(Util::TypeCue<long>) const;
  virtual bool get(Util::TypeCue<bool>) const;
  virtual double get(Util::TypeCue<double>) const;
  virtual const char* get(Util::TypeCue<const char*>) const;

  virtual void set(int val);
  virtual void set(long val);
  virtual void set(bool val);
  virtual void set(double val);
  virtual void set(const char* val);

  virtual void assignTo(Value& other) const;

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

  virtual const char* getNativeTypeName() const;

  virtual void printTo(STD_IO::ostream& os) const;
  virtual void scanFrom(STD_IO::istream& is);

  virtual int get(Util::TypeCue<int>) const;
  virtual long get(Util::TypeCue<long>) const;
  virtual bool get(Util::TypeCue<bool>) const;
  virtual double get(Util::TypeCue<double>) const;
  virtual const char* get(Util::TypeCue<const char*>) const;

  virtual void set(int val);
  virtual void set(long val);
  virtual void set(bool val);
  virtual void set(double val);
  virtual void set(const char* val);

  virtual void assignTo(Value& other) const;

  /// Return a reference to the currently pointed-to \c T object.
  T& operator()() { return *itsValPtr; }

  /// Return a const reference to the currently pointed-to \c T object.
  const T& operator()() const { return *itsValPtr; }

private:
  T* itsValPtr;
};

static const char vcid_value_h[] = "$Header$";
#endif // !VALUE_H_DEFINED
