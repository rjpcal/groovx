///////////////////////////////////////////////////////////////////////
//
// response.h
//
// Copyright (c) 2000-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Mar 13 18:30:04 2000
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

#ifndef RESPONSE_H_DEFINED
#define RESPONSE_H_DEFINED

#include "util/value.h"

/// A Value subclass for observers' responses in a experiment trials.
class Response : public Value
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

  virtual rutz::fstring getNativeTypeName() const;

  virtual void printTo(STD_IO::ostream& os) const;
  virtual void scanFrom(STD_IO::istream& is);

  bool shouldIgnore() const { return itsVal == IGNORE; }

  bool isValid() const { return (itsVal >= 0); }

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

static const char vcid_response_h[] = "$Header$";
#endif // !RESPONSE_H_DEFINED
