///////////////////////////////////////////////////////////////////////
//
// freelist.h
//
// Copyright (c) 2001-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Fri Jul 20 07:54:29 2001
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

#ifndef FREELIST_H_DEFINED
#define FREELIST_H_DEFINED

#include <cstddef>

/// Un-typesafe base class for maintaining a free-list memory pool.
class FreeListBase
{
private:
  /// Free-node class for free-list memory pools.
  struct Node
  {
    Node* next;
  };

  Node* itsNodeList;
  std::size_t itsSizeCheck;

  FreeListBase(const FreeListBase&);
  FreeListBase& operator=(const FreeListBase&);

public:
  /// Construct an (empty) free list.
  /** All objects from this list are expected to be of size \a size_check. */
  FreeListBase(std::size_t size_check) :
    itsNodeList(0), itsSizeCheck(size_check) {}

  /// Allocate space for a new object.
  /** If there are chunks available in the free list, one of those is
      returned; otherwise new memory is allocated with malloc() or
      equivalent. */
  void* allocate(std::size_t bytes);

  /// Return an object to the free list.
  void deallocate(void* space);
};

/// Typesafe wrapper of FreeListBase for maintaining free-list memory pools.
template <class T>
class FreeList : private FreeListBase
{
public:
  /// Construct an (empty) free list.
  /** All objects allocated from this list must be of size sizeof(T). */
  FreeList() : FreeListBase(sizeof(T)) {}

  void* allocate(std::size_t bytes)
  {
    return FreeListBase::allocate(bytes);
  }

  void deallocate(void* space)
  {
    FreeListBase::deallocate(space);
  }
};

static const char vcid_freelist_h[] = "$Header$";
#endif // !FREELIST_H_DEFINED
