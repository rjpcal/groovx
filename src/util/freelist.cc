///////////////////////////////////////////////////////////////////////
//
// freelist.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jul 20 08:00:31 2001
// written: Fri Jul 20 08:07:40 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FREELIST_CC_DEFINED
#define FREELIST_CC_DEFINED

#include "util/freelist.h"

#define LOCAL_ASSERT
#include "util/debug.h"

void* FreeListBase::allocate(size_t bytes, size_t size_check)
{
  Assert(bytes == size_check);
  if (itsNodeList == 0)
    return ::operator new(bytes);
  Node* node = itsNodeList;
  itsNodeList = itsNodeList->next;
  return (void*) node;
}

void FreeListBase::deallocate(void* space)
{
  ((Node*)space)->next = itsNodeList;
  itsNodeList = (Node*)space;
}

static const char vcid_freelist_cc[] = "$Header$";
#endif // !FREELIST_CC_DEFINED
