///////////////////////////////////////////////////////////////////////
//
// masterptrt.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Oct  9 08:32:17 2000
// written: Mon Oct  9 13:37:14 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MASTERPTRT_CC_DEFINED
#define MASTERPTRT_CC_DEFINED

#include "masterptr.h"

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
bool MasterPtr<T>::isNull() const
{
  return (itsPtr == 0);
}

template <class T>
bool MasterPtr<T>::operator==(const MasterPtrBase& other)
{
  const MasterPtr<T>* otherPtr = dynamic_cast<const MasterPtr<T>*>(&other);
  if (otherPtr == 0) return false;

  return (otherPtr->itsPtr == this->itsPtr);
}

static const char vcid_masterptrt_cc[] = "$Header$";
#endif // !MASTERPTRT_CC_DEFINED
