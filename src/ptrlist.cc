///////////////////////////////////////////////////////////////////////
//
// ptrlist.cc
// Rob Peters
// created: Fri Apr 23 00:35:32 1999
// written: Wed Feb 16 07:58:59 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PTRLIST_CC_DEFINED
#define PTRLIST_CC_DEFINED

#include "ptrlist.h"

template <class T>
PtrList<T>::PtrList(int size) :
  IoPtrList(size)
{}

template <class T>
PtrList<T>::~PtrList() {
  clear();
}

template <class T>
T* PtrList<T>::castToT(void* ptr) const { return static_cast<T*>(ptr); }

template <class T>
void* PtrList<T>::castFromT(T* ptr) const { return static_cast<void*>(ptr); }

template <class T>
IO* PtrList<T>::fromVoidToIO(void* ptr) const
{
  IO* io = dynamic_cast<IO*>(castToT(ptr));
  if ( io == 0 ) { throw ErrorWithMsg("bad cast in fromVoidToIO"); }
  return io;
}

template <class T>
void* PtrList<T>::fromIOToVoid(IO* obj) const
{
  void* v = castFromT(dynamic_cast<T*>(obj));
  if ( v == 0 ) { throw ErrorWithMsg("bad cast in fromIOToVoid"); }
  return v;
}

template <class T>
void PtrList<T>::destroyPtr(void* ptr)
{ delete castToT(ptr); }

static const char vcid_ptrlist_cc[] = "$Header$";
#endif // !PTRLIST_CC_DEFINED
