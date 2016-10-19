/** @file nub/handle.h strong reference-counted handle for nub::object */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Nov 16 08:50:12 2005
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

#ifndef GROOVX_NUB_HANDLE_H_UTC20051116165012_DEFINED
#define GROOVX_NUB_HANDLE_H_UTC20051116165012_DEFINED

#include "nub/types.h"

#include "rutz/algo.h"         // for rutz::swap2()
#include "rutz/fileposition.h" // for SRC_POS macro

#include <typeinfo>

namespace nub
{
  namespace detail
  {
    template <class T, class unref_policy> class handle;

    [[noreturn]] void throw_ref_null(const std::type_info& info, const rutz::file_pos& pos);
    [[noreturn]] void throw_ref_unshareable(const std::type_info& msg, const rutz::file_pos& pos);
  }
}

/// A shared implementation class for nub::floating_ref and nub::ref.
/** Note that the only operation that can throw is the
    constructor, which throws in case it is passed a null or
    unshareable object pointer .*/
template <class T, class unref_policy>
class nub::detail::handle
{
public:
  explicit handle(T* master) : m_master(master)
  {
    if (master == 0)
      throw_ref_null(typeid(T), SRC_POS);

    if (master->is_not_shareable())
      throw_ref_unshareable(typeid(T), SRC_POS);

    m_master->incr_ref_count();
  }

  ~handle() noexcept
  { unref_policy::unref(m_master); }

  handle(const handle& other) noexcept
    :
    m_master(other.m_master)
  {
    m_master->incr_ref_count();
  }

  handle& operator=(const handle& other) noexcept
  {
    handle other_copy(other);
    this->swap(other_copy);
    return *this;
  }

  T* get() const noexcept
  { return m_master; }

  bool operator==(const handle& other) const noexcept
  { return m_master == other.m_master; }

private:
  void swap(handle& other) noexcept
  {
    rutz::swap2(m_master, other.m_master);
  }

  T* m_master;

};

#endif // !GROOVX_NUB_HANDLE_H_UTC20051116165012DEFINED
