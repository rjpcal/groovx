///////////////////////////////////////////////////////////////////////
//
// freelist.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jul 20 07:54:29 2001
// written: Fri Jul 20 08:01:45 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FREELIST_H_DEFINED
#define FREELIST_H_DEFINED

class FreeListBase {
private:
  struct Node
  {
    Node* next;
  };

  Node* itsNodeList;

public:
  FreeListBase() : itsNodeList(0) {}

  void* allocate(size_t bytes, size_t size_check);
  void deallocate(void* space);
};

template <class T>
class FreeList : private FreeListBase {
public:
  void* allocate(size_t bytes)
  {
    return FreeListBase::allocate(bytes, sizeof(T));
  }

  void deallocate(void* space)
  {
    FreeListBase::deallocate(space);
  }
};

static const char vcid_freelist_h[] = "$Header$";
#endif // !FREELIST_H_DEFINED
