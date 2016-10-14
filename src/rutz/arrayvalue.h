/** @file rutz/arrayvalue.h rutz::value subclass for variable-sized
    sets of homegeneous values */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Nov  4 15:32:47 2002
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

#ifndef GROOVX_RUTZ_ARRAYVALUE_H_UTC20050626084020_DEFINED
#define GROOVX_RUTZ_ARRAYVALUE_H_UTC20050626084020_DEFINED

#include "rutz/fstring.h"
#include "rutz/value.h"

#include <iomanip>
#include <istream>
#include <vector>

namespace rutz
{
  /// A rutz::value subclass representing a variable-size set of homogeneous values.
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

    virtual void print_to(std::ostream& os) const
    {
      for (unsigned int i = 0; i < m_array.size(); ++i)
        os << m_array[i] << "   ";
    }

    virtual void scan_from(std::istream& is)
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

#endif // !GROOVX_RUTZ_ARRAYVALUE_H_UTC20050626084020_DEFINED
