/** @file rutz/freelist.h memory allocation via a free-list pool */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Fri Jul 20 07:54:29 2001
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#ifndef GROOVX_RUTZ_FREELIST_H_UTC20050626084020_DEFINED
#define GROOVX_RUTZ_FREELIST_H_UTC20050626084020_DEFINED

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
  std::size_t m_num_allocations;
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

  /// Release all nodes currently on the free list (e.g. to conserve memory).
  void release_free_nodes();

  /// Query the chunk size that this freelist is for.
  std::size_t alloc_size() const { return m_size_check; }

  /// Query how many allocations have been made.
  std::size_t num_allocations() const { return m_num_allocations; }
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

  /// Release all nodes currently on the free list (e.g. to conserve memory).
  void release_free_nodes() { m_base.release_free_nodes(); }
};

#endif // !GROOVX_RUTZ_FREELIST_H_UTC20050626084020_DEFINED
