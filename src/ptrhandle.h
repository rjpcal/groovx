///////////////////////////////////////////////////////////////////////
//
// ptrhandle.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sun Oct 22 15:56:41 2000
// written: Tue Jun 12 12:01:57 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PTRHANDLE_H_DEFINED
#define PTRHANDLE_H_DEFINED


namespace PtrHandleUtil {
  void throwErrorWithMsg(const char* msg);
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * NullablePtrHandle<T> is a stack-based wrapper for ref-counted T's,
 * that may point to no T at all. isValid() may be called to determine
 * if there is currently a non-null pointee.
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class T>
class NullablePtrHandle {
public:
  explicit NullablePtrHandle(T* master) : itsMaster(master)
    {
      if (itsMaster != 0)
        itsMaster->incrRefCount();
    }

  ~NullablePtrHandle()
    {
      if (itsMaster != 0)
        itsMaster->decrRefCount();
    }

  //
  // Copy constructors
  //

  NullablePtrHandle(const NullablePtrHandle& other) : itsMaster(other.itsMaster)
    {
      if (itsMaster != 0)
        itsMaster->incrRefCount();
    }

  template <class U>
  NullablePtrHandle(const NullablePtrHandle<U>& other) :
    itsMaster(other.isValid() ? other.get() : 0)
    {
      if (itsMaster != 0)
        itsMaster->incrRefCount();
    }

  //
  // Assignment operators
  //

  NullablePtrHandle& operator=(const NullablePtrHandle& other)
    {
      NullablePtrHandle otherCopy(other);
      this->swap(otherCopy);
      return *this;
    }


  bool isValid() const { return itsMaster != 0; }

  void release()
    {
      if (itsMaster != 0)
        itsMaster->decrRefCount();
      itsMaster = 0;
    }

  T* operator->() const { return get(); }
  T& operator*()  const { return *(get()); }

  T* get()        const { ensureValid(); return itsMaster; }

private:
  void ensureValid() const
  {
    if (itsMaster == 0)
      PtrHandleUtil::throwErrorWithMsg(
           "attempted to derefence invalid NullablePtrHandle");
  }

  void swap(NullablePtrHandle& other)
    {
      T* otherMaster = other.itsMaster;
      other.itsMaster = this->itsMaster;
      this->itsMaster = otherMaster;
    }

  T* itsMaster;
};

static const char vcid_ptrhandle_h[] = "$Header$";
#endif // !PTRHANDLE_H_DEFINED
