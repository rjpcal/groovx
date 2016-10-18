/** @file rutz/multivalue.h rutz::value class for fixed-size sets of
    homogeneous values */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Aug 22 16:45:38 2001
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

#ifndef GROOVX_RUTZ_MULTIVALUE_H_UTC20050626084020_DEFINED
#define GROOVX_RUTZ_MULTIVALUE_H_UTC20050626084020_DEFINED

#include "rutz/value.h"

namespace rutz
{
  /// A rutz::value subclass representing a fixed-size set of homogeneous values.
  template <class T>
  class multi_value : public rutz::value
  {
  public:
    multi_value(int num);

    /// Get a string describing the underlying native type.
    virtual rutz::fstring value_typename() const = 0;

    virtual void print_to(std::ostream& os) const;
    virtual void scan_from(std::istream& is);

  protected:
    /// Returns a const pointer to the start of the underlying storage.
    virtual const T*   const_begin() const = 0;
    /// Returns a const pointer to one-past-the-end of the underlying storage.
    const T*   const_end()   const { return const_begin() + m_num_values; }
    /// Returns a non-const pointer to the start of the underlying storage.
    T* mutable_begin()       { return const_cast<T*>(const_begin()); }
    /// Returns a non-const pointer to one-past-the-end of the underlying storage.
    T* mutable_end()         { return const_cast<T*>(const_end()); }

  private:
    int m_num_values;
  };
}

#endif // !GROOVX_RUTZ_MULTIVALUE_H_UTC20050626084020_DEFINED
