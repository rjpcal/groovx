///////////////////////////////////////////////////////////////////////
//
// arrayvalue.h
//
// Copyright (c) 2002-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Nov  4 15:32:47 2002
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

#ifndef ARRAYVALUE_H_DEFINED
#define ARRAYVALUE_H_DEFINED

#include "util/strings.h"
#include "util/value.h"

#include <iomanip>
#include <vector>

/// A Value subclass representing a fixed-size set of homogeneous values.
template <class T>
class TArrayValue : public Value
{
public:
  TArrayValue() : itsArray() {}
  TArrayValue(const fstring& s) { setFstring(s); }
  virtual ~TArrayValue() {}

  virtual fstring getNativeTypeName() const
  {
    return fstring("TArrayValue");
  }

  virtual void printTo(STD_IO::ostream& os) const
  {
    for (unsigned int i = 0; i < itsArray.size(); ++i)
      os << itsArray[i] << "   ";
  }

  virtual void scanFrom(STD_IO::istream& is)
  {
    std::vector<T> newarray;

    while (!is.eof() && !is.fail())
      {
        T val;
        is >> val >> std::ws;
        newarray.push_back(val);
      }
    itsArray.swap(newarray);
  }

  unsigned int arraySize() const { return itsArray.size(); }

        T& arrayAt(unsigned int i)       { return itsArray[i]; }
  const T& arrayAt(unsigned int i) const { return itsArray[i]; }

private:
  std::vector<T> itsArray;
};

static const char vcid_arrayvalue_h[] = "$Header$";
#endif // !ARRAYVALUE_H_DEFINED
