///////////////////////////////////////////////////////////////////////
//
// value.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Sep 28 11:19:17 1999
// written: Fri Jan 18 16:06:54 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef VALUE_H_DEFINED
#define VALUE_H_DEFINED

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

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c Value defines an interface for variant-type value objects that
 * can be converted to+from a string representation. Subclasses may
 * allow conversion among types so that a value of any basic type can
 * be retrieved, or they may allow only one basic type to be
 * retrieved.
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

  /// Return a string giving the name of the native type.
  virtual fstring getNativeTypeName() const = 0;

  /// Write the value to an \c STD_IO::ostream.
  virtual void printTo(STD_IO::ostream& os) const = 0;

  /// Read the value from an \c STD_IO::istream.
  virtual void scanFrom(STD_IO::istream& is) = 0;

  /** Get a C-style string (\c char*) representation of the value by
      building a string stream, calling printTo(), and returning its
      value. The result of this function is only valid until the next
      call.*/
  const char* getCstring() const;

  /** Attempt to get an \c fstring representation of the value. Calls
      the C-string get(). */
  fstring getFstring() const;

  /** Set the value from a C-style string (\c char*) representation,
      by building a string stream from \a val and calling scanFrom()
      with it. */
  void setCstring(const char* val);

  /// Set the value from an fstring. Calls the setCstring().
  void setFstring(fstring val);
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
