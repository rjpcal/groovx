///////////////////////////////////////////////////////////////////////
//
// freelist.h
//
// Copyright (c) 2001-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Fri Jul 20 07:54:29 2001
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

#ifndef FREELIST_H_DEFINED
#define FREELIST_H_DEFINED

#include <cstddef>

namespace rutz
{
  class free_list_base;
  template <class T> class free_list;
}

/// Un-typesafe base class for maintaining a free-list memory pool.
class rutz::free_list_base
{
private:
  /// Free-node class for free-list memory pools.
  struct node
  {
    node* next;
  };

  node* m_node_list;
  const std::size_t m_size_check;

  free_list_base(const free_list_base&);
  free_list_base& operator=(const free_list_base&);

public:
  /// Construct an (empty) free list.
  /** All objects from this list must be of size \a size_check. */
  free_list_base(std::size_t size_check);

  /// Allocate space for a new object.
  /** If there are chunks available in the free list, one of those is
      returned; otherwise new memory is allocated with malloc() or
      equivalent. */
  void* allocate(std::size_t bytes);

  /// Return an object to the free list.
  void deallocate(void* space);
};

/// Typesafe wrapper of free_list_base for maintaining free-list memory pools.
template <class T>
class rutz::free_list
{
private:
  rutz::free_list_base m_base;

public:
  /// Construct an (empty) free list.
  /** All objects allocated from this list must be of size sizeof(T). */
  free_list() : m_base(sizeof(T)) {}

  void* allocate(std::size_t bytes)
  {
    return m_base.allocate(bytes);
  }

  void deallocate(void* space)
  {
    m_base.deallocate(space);
  }
};

static const char vcid_freelist_h[] = "$Header$";
#endif // !FREELIST_H_DEFINED
