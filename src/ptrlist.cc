///////////////////////////////////////////////////////////////////////
//
// ptrlist.cc
// Rob Peters
// created: Fri Apr 23 00:35:32 1999
// written: Fri Oct 27 15:51:41 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PTRLIST_CC_DEFINED
#define PTRLIST_CC_DEFINED

#include "ptrlist.h"

template <class T>
PtrList<T>::PtrList() :
  IoPtrList()
{}

template <class T>
PtrList<T>::~PtrList() {}

static const char vcid_ptrlist_cc[] = "$Header$";
#endif // !PTRLIST_CC_DEFINED
