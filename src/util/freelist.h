///////////////////////////////////////////////////////////////////////
//
// freelist.h
//
// Copyright (c) 2001-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jul 20 07:54:29 2001
// written: Mon Jan 13 11:08:26 2003
// $Id$
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
