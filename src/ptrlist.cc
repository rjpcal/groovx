///////////////////////////////////////////////////////////////////////
//
// ptrlist.cc
// Rob Peters
// created: Fri Apr 23 00:35:32 1999
// written: Sat Oct  7 20:06:18 2000
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
