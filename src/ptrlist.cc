///////////////////////////////////////////////////////////////////////
//
// ptrlist.cc
// Rob Peters
// created: Fri Apr 23 00:35:32 1999
// written: Thu Mar 30 12:29:05 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PTRLIST_CC_DEFINED
#define PTRLIST_CC_DEFINED

#include "ptrlist.h"

#include "system/demangle.h"

#include "util/strings.h"

#include <typeinfo>

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
IO::IoObject* PtrList<T>::fromVoidToIO(void* ptr) const
{
  IO::IoObject* io = dynamic_cast<IO::IoObject*>(castToT(ptr));
  if ( io == 0 ) { throw ErrorWithMsg("bad cast in fromVoidToIO"); }
  return io;
}

template <class T>
void* PtrList<T>::fromIOToVoid(IO::IoObject* obj) const
{
  void* v = castFromT(dynamic_cast<T*>(obj));
  if ( v == 0 ) { throw ErrorWithMsg("bad cast in fromIOToVoid"); }
  return v;
}

template <class T>
void PtrList<T>::destroyPtr(void* ptr)
{ delete castToT(ptr); }

template <class T>
const char* PtrList<T>::alternateIoTags() const {
  static dynamic_string result;

  static bool inited = false;
  if (!inited) {
	 result = IoPtrList::alternateIoTags();
	 result += " PtrList<";
	 result += demangle_cstr(typeid(T).name());
	 result += ">";
	 inited = true;
  }

  return result.c_str();
}


static const char vcid_ptrlist_cc[] = "$Header$";
#endif // !PTRLIST_CC_DEFINED
