///////////////////////////////////////////////////////////////////////
//
// multivalue.h
//
// Copyright (c) 2001-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Aug 22 16:45:38 2001
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

#ifndef MULTIVALUE_H_DEFINED
#define MULTIVALUE_H_DEFINED

#include "util/value.h"

namespace rutz
{
  /// A rutz::value subclass representing a fixed-size set of homogeneous values.
  template <class T>
  class multi_value : public rutz::value
  {
  public:
    multi_value(int num);
    virtual ~multi_value();

    /// Get a string describing the underlying native type.
    virtual rutz::fstring value_typename() const = 0;

    virtual void print_to(STD_IO::ostream& os) const;
    virtual void scan_from(STD_IO::istream& is);

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

static const char vcid_multivalue_h[] = "$Header$";
#endif // !MULTIVALUE_H_DEFINED
