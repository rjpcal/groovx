///////////////////////////////////////////////////////////////////////
//
// freelist.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jul 20 08:00:31 2001
// written: Mon Aug 13 11:18:03 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FREELIST_CC_DEFINED
#define FREELIST_CC_DEFINED

#include "util/freelist.h"

#define LOCAL_ASSERT
#include "util/debug.h"

void* FreeListBase::allocate(std::size_t bytes)
{
  Assert(bytes == itsSizeCheck);
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
