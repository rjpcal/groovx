/** @file visx/response.h user responses in a psychophysics experiment */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Mar 13 18:30:04 2000
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_VISX_RESPONSE_H_UTC20050626084016_DEFINED
#define GROOVX_VISX_RESPONSE_H_UTC20050626084016_DEFINED

#include "rutz/value.h"

/// A rutz::value subclass for observers' responses in a experiment trials.
class Response : public rutz::value
{
public:
  static const int INVALID_VALUE = -1;
  static const int ALWAYS_CORRECT = -2;

  /// IGNORE means to treat the response as if it never happened.
  /** For instance, this may be useful in handling key or button events
      that occur when the user hits an errant key. */
  static const int IGNORE = -3;

  Response(int v = INVALID_VALUE, int m = -1) :
    itsVal(v), itsMsec(m)
  {}

  virtual ~Response() throw();

  virtual rutz::fstring value_typename() const;

  virtual void print_to(std::ostream& os) const;
  virtual void scan_from(std::istream& is);

  bool shouldIgnore() const { return itsVal == IGNORE; }

  bool is_valid() const { return (itsVal >= 0); }

  bool matchesCorrectValue(int cval) const
  {
    return (cval == ALWAYS_CORRECT) || (itsVal == cval);
  }

  int val() const { return itsVal; }
  int msec() const { return itsMsec; }

  void setVal(int new_val) { itsVal = new_val; }
  void setMsec(int new_val) { itsMsec = new_val; }

private:
  int itsVal;
  int itsMsec;
};

static const char __attribute__((used)) vcid_groovx_visx_response_h_utc20050626084016[] = "$Id$ $HeadURL$";
#endif // !GROOVX_VISX_RESPONSE_H_UTC20050626084016_DEFINED
