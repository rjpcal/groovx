///////////////////////////////////////////////////////////////////////
//
// arrayvalue.h
//
// Copyright (c) 2002-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Nov  4 15:32:47 2002
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

#ifndef ARRAYVALUE_H_DEFINED
#define ARRAYVALUE_H_DEFINED

#include "util/fstring.h"
#include "util/value.h"

#include <iomanip>
#include <vector>

namespace rutz
{
  /// A rutz::value subclass representing a fixed-size set of homogeneous values.
  template <class T>
  class array_value : public rutz::value
  {
  public:
    array_value() : m_array() {}
    array_value(const rutz::fstring& s) { set_string(s); }
    virtual ~array_value() {}

    virtual rutz::fstring value_typename() const
    {
      return rutz::fstring("array_value");
    }

    virtual void print_to(STD_IO::ostream& os) const
    {
      for (unsigned int i = 0; i < m_array.size(); ++i)
        os << m_array[i] << "   ";
    }

    virtual void scan_from(STD_IO::istream& is)
    {
      std::vector<T> newarray;

      while (!is.eof() && !is.fail())
        {
          T val;
          is >> val >> std::ws;
          newarray.push_back(val);
        }
      m_array.swap(newarray);
    }

    unsigned int array_size() const { return m_array.size(); }

    T& array_at(unsigned int i)       { return m_array[i]; }
    const T& array_at(unsigned int i) const { return m_array[i]; }

  private:
    std::vector<T> m_array;
  };
}

static const char vcid_arrayvalue_h[] = "$Id$ $URL$";
#endif // !ARRAYVALUE_H_DEFINED
