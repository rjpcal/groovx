///////////////////////////////////////////////////////////////////////
//
// freelist.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jul 20 07:54:29 2001
// written: Fri Jan 18 16:06:55 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FREELIST_H_DEFINED
#define FREELIST_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(CSTDDEF_DEFINED)
#include <cstddef>
#define CSTDDEF_DEFINED
#endif

class FreeListBase
{
private:
  struct Node
  {
    Node* next;
  };

  Node* itsNodeList;
  std::size_t itsSizeCheck;

  FreeListBase(const FreeListBase&);
  FreeListBase& operator=(const FreeListBase&);

public:
  FreeListBase(std::size_t size_check) :
    itsNodeList(0), itsSizeCheck(size_check) {}

  void* allocate(std::size_t bytes);
  void deallocate(void* space);
};

template <class T>
class FreeList : private FreeListBase
{
public:
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
