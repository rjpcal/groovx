///////////////////////////////////////////////////////////////////////
//
// value.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Tue Sep 28 11:19:17 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef VALUE_H_DEFINED
#define VALUE_H_DEFINED

#ifdef HAVE_IOSFWD
#  include <iosfwd>
#else
class istream;
class ostream;
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

  /// Get a C-style string (\c char*) representation of the value
  /** Builds a string stream, calls printTo(), and returns its value. The
      result of this function is only valid until the next call.*/
  const char* getCstring() const;

  /// Get an \c fstring representation of the value.
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
