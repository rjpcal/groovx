///////////////////////////////////////////////////////////////////////
//
// ptrhandle.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sun Oct 22 15:56:41 2000
// written: Thu May 17 06:55:46 2001
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
 * PtrHandle<T> is a stack-based wrapper for ref-counted T's.
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class T>
class PtrHandle {
public:
  explicit PtrHandle(T* master) : itsMaster(master)
  {
	 if (master == 0) PtrHandleUtil::throwErrorWithMsg(
								"T* was null in PtrHandle<T>()");
	 itsMaster->incrRefCount();
  }

  ~PtrHandle()
    { itsMaster->decrRefCount(); }

  PtrHandle(const PtrHandle& other) : itsMaster(other.itsMaster)
    { itsMaster->incrRefCount(); }

  template <class U> friend class PtrHandle;

  template <class U>
  PtrHandle(const PtrHandle<U>& other) : itsMaster(other.itsMaster)
    { itsMaster->incrRefCount(); }

  PtrHandle& operator=(const PtrHandle& other)
    {
		PtrHandle otherCopy(other);
		this->swap(otherCopy);
		return *this;
	 }

  T* operator->() const { return itsMaster; }
  T& operator*()  const { return *itsMaster; }
  
  T* get()        const { return itsMaster; }

private:
  void swap(PtrHandle& other)
    {
		T* otherMaster = other.itsMaster;
		other.itsMaster = this->itsMaster;
		this->itsMaster = otherMaster;
	 }

  T* itsMaster;
};


template <class To, class Fr>
PtrHandle<To> dynamicCast(PtrHandle<Fr> p)
{
  Fr* f = p.get();
  To& t = dynamic_cast<To&>(*f); // will throw bad_cast on failure
  return PtrHandle<To>(&t);
}


///////////////////////////////////////////////////////////////////////
/**
 *
 * NullablePtrHandle<T> is a stack-based wrapper for ref-counted T's,
 * similar to PtrHandle<T>, except that a NullablePtrHandle may point
 * to no T at all. isValid() may be called to determine if there is
 * currently a non-null pointee.
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

  explicit NullablePtrHandle(PtrHandle<T> other) :
	 itsMaster(other.get())
    {
		ensureValid();
		itsMaster->incrRefCount();
	 }

  ~NullablePtrHandle()
    {
		if (itsMaster != 0)
		  itsMaster->decrRefCount();
	 }

  NullablePtrHandle(const NullablePtrHandle& other) : itsMaster(other.itsMaster)
    {
		if (itsMaster != 0)
		  itsMaster->incrRefCount();
	 }

  NullablePtrHandle& operator=(const NullablePtrHandle& other)
    {
		NullablePtrHandle otherCopy(other);
		this->swap(otherCopy);
		return *this;
	 }

  NullablePtrHandle& operator=(const PtrHandle<T>& other)
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
