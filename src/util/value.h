///////////////////////////////////////////////////////////////////////
//
// value.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Sep 28 11:19:17 1999
// written: Mon Sep  3 13:40:40 2001
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

class fstring;

/**
 *
 * Exception class to be thrown from failed calls to get/set a \c
 * Value object.
 *
 **/
class ValueError : public Util::Error
{
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

class Value
{
public:

  /// Default constructor.
  Value();

  /// Virtual destructor ensures proper destruction of base classes.
  virtual ~Value();

  /// Returns a new dynamically allocated copy of the calling object.
  virtual Value* clone() const = 0;

  /// Return a string giving the name of the native type.
  virtual fstring getNativeTypeName() const = 0;

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

  /** Attempt to get an \c fstring representation of the value. Calls
      the C-string get(). */
  fstring get(Util::TypeCue<fstring>) const;

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

  /** Attempt to set the value from an fstring. Calls the C-string set(). */
  void set(fstring val);

  /// Assign \a this's value to \a other, using the native type of \a this.
  virtual void assignTo(Value& other) const = 0;

  /** Assign \a other's value to \a this. Default is to call
      other.assignTo(this) */
  virtual void assignFrom(const Value& other);
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

static const char vcid_value_h[] = "$Header$";
#endif // !VALUE_H_DEFINED
