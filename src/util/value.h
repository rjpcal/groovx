///////////////////////////////////////////////////////////////////////
//
// value.h
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Tue Sep 28 11:19:17 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
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

namespace rutz
{
  class fstring;
  class value;
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c rutz::value defines an interface for variant-type value objects
 * that can be converted to+from a string representation. Subclasses
 * may allow conversion among types so that a value of any basic type
 * can be retrieved, or they may allow only one basic type to be
 * retrieved.
 *
 **/
///////////////////////////////////////////////////////////////////////

class rutz::value
{
public:

  /// Default constructor.
  value();

  /// Virtual destructor ensures proper destruction of base classes.
  virtual ~value();

  /// Return a string giving the name of the native type.
  virtual rutz::fstring value_typename() const = 0;

  /// Write the value to an \c STD_IO::ostream.
  virtual void print_to(STD_IO::ostream& os) const = 0;

  /// Read the value from an \c STD_IO::istream.
  virtual void scan_from(STD_IO::istream& is) = 0;

  /// Get an \c rutz::fstring representation of the value.
  /** Builds a string stream, calls print_to(), and returns its
      value. */
  rutz::fstring get_string() const;

  /// Set the value from an rutz::fstring.
  /** Builds a string stream from \a val and calling scan_from() with
      it. */
  void set_string(rutz::fstring val);
};

///////////////////////////////////////////////////////////////////////
//
// Global stream insertion/extraction operators for rutz::value
//
///////////////////////////////////////////////////////////////////////

inline STD_IO::ostream& operator<<(STD_IO::ostream& os,
                                   const rutz::value& val)
{
  val.print_to(os);
  return os;
}

inline STD_IO::istream& operator>>(STD_IO::istream& is,
                                   rutz::value& val)
{
  val.scan_from(is);
  return is;
}

static const char vcid_value_h[] = "$Header$";
#endif // !VALUE_H_DEFINED
