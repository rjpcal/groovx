///////////////////////////////////////////////////////////////////////
//
// ptrlist.cc
// Rob Peters
// created: Fri Apr 23 00:35:32 1999
// written: Sun Oct  8 18:42:06 2000
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
MasterPtr<T>::MasterPtr(T* ptr) :
  MasterIoPtr(),
  itsPtr(ptr)
{}

template <class T>
MasterPtr<T>::~MasterPtr()
{
  delete itsPtr;
}

template <class T>
IO::IoObject* MasterPtr<T>::ioPtr() const
{
  return dynamic_cast<IO::IoObject*>(itsPtr);
}

template <class T>
bool MasterPtr<T>::isValid() const
{
  return (itsPtr != 0);
}

template <class T>
bool MasterPtr<T>::operator==(const MasterPtrBase& other)
{
  const MasterPtr<T>* otherPtr = dynamic_cast<const MasterPtr<T>*>(&other);
  if (otherPtr == 0) return false;

  return (otherPtr->itsPtr == this->itsPtr);
}

template <class T>
PtrList<T>::PtrList(int size) :
  IoPtrList(size)
{}

template <class T>
PtrList<T>::~PtrList() {}

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

template <class T>
MasterIoPtr* PtrList<T>::makeMasterIoPtr(IO::IoObject* obj) const {
  T& t = dynamic_cast<T&>(*obj);

  return new MasterPtr<T>(&t);
}

static const char vcid_ptrlist_cc[] = "$Header$";
#endif // !PTRLIST_CC_DEFINED
