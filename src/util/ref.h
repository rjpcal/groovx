///////////////////////////////////////////////////////////////////////
//
// ref.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Oct 26 17:50:59 2000
// written: Mon Jun 11 15:08:15 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef REF_H_DEFINED
#define REF_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(ERROR_H_DEFINED)
#include "util/error.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(OBJECT_H_DEFINED)
#include "util/object.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TRAITS_H_DEFINED)
#include "util/traits.h"
#endif

namespace Util
{
  template <class T> class Ref;
  template <class T> class MaybeRef;
}

using Util::Ref;
using Util::MaybeRef;

///////////////////////////////////////////////////////////////////////
//
// Util::RefHelper helper functions
//
///////////////////////////////////////////////////////////////////////

namespace Util { namespace RefHelper {
  bool isValidId(Util::UID id);
  Util::Object* getCheckedItem(Util::UID id);

  void insertItem(Util::Object* obj);

  void throwErrorWithMsg(const char* msg);

#ifndef ACC_COMPILER
#  define DYNCAST dynamic_cast
#else
  template <class T, class U>
  T DYNCAST(U& u) { return dynamic_cast<T>(u); }
#endif

  template <class T>
  inline T* getCastedItem(Util::UID id)
  {
    Util::Object* obj = getCheckedItem(id);
    T& t = DYNCAST<T&>(*obj);
    return &t;
  }

#ifdef DYNCAST
#undef DYNCAST
#endif

  template <>
  inline Util::Object* getCastedItem<Util::Object>(Util::UID id)
  { return getCheckedItem(id); }

}} // end namespace Util::RefHelpers



///////////////////////////////////////////////////////////////////////
/**
 *
 * Util::Ref<T> is a ref-counted smart pointer for holding RefCounted
 * objects. A Util::Ref<T> is guaranteed to always point to a valid
 * RefCounted object, and uses RefCounted's strong ref counts to
 * achieve this. Therefore, a Util::Ref<T> cannot be constructed for a
 * volatlie RefCounted object for which only weak references are
 * available.
 *
 **/
///////////////////////////////////////////////////////////////////////

namespace Util {

template <class T>
class Ref {
private:

  class Handle {
  public:
	 explicit Handle(T* master) : itsMaster(master)
	 {
		if (master == 0) Util::RefHelper::throwErrorWithMsg(
									 "attempted to construct a Ref with a null pointer");
		itsMaster->incrRefCount();
	 }

	 ~Handle()
    { itsMaster->decrRefCount(); }

	 Handle(const Handle& other) : itsMaster(other.itsMaster)
    { itsMaster->incrRefCount(); }

	 template <class U> friend class Handle;

	 template <class U>
	 Handle(const Handle<U>& other) : itsMaster(other.itsMaster)
    { itsMaster->incrRefCount(); }

	 Handle& operator=(const Handle& other)
    {
      Handle otherCopy(other);
      this->swap(otherCopy);
      return *this;
    }

	 T* get() const { return itsMaster; }

  private:
	 void swap(Handle& other)
    {
      T* otherMaster = other.itsMaster;
      other.itsMaster = this->itsMaster;
      this->itsMaster = otherMaster;
    }

	 T* itsMaster;
  };

  Handle itsHandle;

public:
  // Default destructor, copy constructor, operator=() are fine

  explicit Ref(Util::UID id) : itsHandle(RefHelper::getCastedItem<T>(id)) {}

  explicit Ref(T* ptr) : itsHandle(ptr)
    { RefHelper::insertItem(ptr); }

  Ref(T* ptr, bool /*noInsert*/) : itsHandle(ptr) {}

  template <class U>
  Ref(const Ref<U>& other) : itsHandle(other.get()) {}

  // Will raise an exception if the MaybeRef is invalid
  template <class U>
  Ref(const MaybeRef<U>& other);

  T* operator->() const { return get(); }
  T& operator*()  const { return *(get()); }

  T* get()        const { return itsHandle.get(); }

  Util::UID id() const { return get()->id(); }
};

// TypeTraits specialization for Util::Ref smart pointer

template <class T>
struct TypeTraits<Ref<T> > {
  typedef T Pointee;
};

} // end namespace Util

template <class To, class Fr>
Ref<To> dynamicCast(Ref<Fr> p)
{
  Fr* f = p.get();
  To& t = dynamic_cast<To&>(*f); // will throw bad_cast on failure
  return Ref<To>(&t);
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * MaybeRef<T> is a wrapper of a NullablePtrHandle<T> along
 * with an integer index from a PtrList<T>.
 *
 **/
///////////////////////////////////////////////////////////////////////

namespace Util {

template <class T>
class MaybeRef {
private:

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
		  Util::RefHelper::throwErrorWithMsg(
								"attempted to derefence an invalid WeakRef");
	 }

	 void swap(NullablePtrHandle& other)
    {
      T* otherMaster = other.itsMaster;
      other.itsMaster = this->itsMaster;
      this->itsMaster = otherMaster;
    }

	 T* itsMaster;
  };

  mutable NullablePtrHandle itsHandle;
  Util::UID itsId;

  void insertItem()
  {
    if (itsHandle.isValid())
      RefHelper::insertItem(itsHandle.get());
  }

public:
  MaybeRef() : itsHandle(0), itsId(0) {}

  explicit MaybeRef(Util::UID id_) : itsHandle(0), itsId(id_) {}

  explicit MaybeRef(T* master) : itsHandle(master), itsId(0)
  {
    if (master != 0) itsId = master->id();
    insertItem();
  }

  MaybeRef(T* master, bool /*noInsert*/) : itsHandle(master), itsId(0)
    { if (master != 0) itsId = master->id(); }


  template <class U>
  MaybeRef(const MaybeRef<U>& other) :
    itsHandle(0), itsId(other.id())
  {
	 if (other.isValid())
		itsHandle = NullablePtrHandle(other.get());
  }

  template <class U>
  MaybeRef(const Ref<U>& other) :
    itsHandle(other.get()), itsId(other.id()) {}

  // Default destructor, copy constructor, operator=() are fine

  T* operator->() const { refresh(); return itsHandle.get(); }
  T& operator*()  const { refresh(); return *(itsHandle.get()); }

  T* get()        const { refresh(); return itsHandle.get(); }

  /** This will try to refresh the handle from the id, and will throw
      an exception if the operation fails (if the id is invalid). */
  void refresh() const {
    if ( !itsHandle.isValid() )
      {
        Ref<T> p(itsId);
        itsHandle = NullablePtrHandle(p.get());
        if (itsId != itsHandle->id())
          throw ErrorWithMsg("assertion failed in refresh");
      }
  }


  /** This will try to refresh the handle from the id, but will not
      throw an exception if the operation fails; it will just leave
      the object with an invalid handle. */
  void attemptRefresh() const {
    if ( !itsHandle.isValid() )
      {
        if (RefHelper::isValidId(itsId)) {
          Ref<T> p(itsId);
          itsHandle = NullablePtrHandle(p.get());
          if (itsId != itsHandle->id())
            throw ErrorWithMsg("assertion failed in attemptRefresh");
        }
      }
  }


  bool isValid() const { attemptRefresh(); return itsHandle.isValid(); }

  Util::UID id() const { return itsId; }
};

// TypeTraits specialization for MaybeRef smart pointer

  template <class T>
  struct TypeTraits<MaybeRef<T> > {
    typedef T Pointee;
  };

} // end namespace Util

template <class To, class Fr>
MaybeRef<To> dynamicCast(MaybeRef<Fr> p)
{
  if (p.isValid())
    {
      Fr* f = p.get();
      To& t = dynamic_cast<To&>(*f); // will throw bad_cast on failure
      return MaybeRef<To>(&t);
    }
  return MaybeRef<To>(p.id());
}

///////////////////////////////////////////////////////////////////////
//
// inline member definitions
//
///////////////////////////////////////////////////////////////////////

template <class T>
template <class U>
inline Util::Ref<T>::Ref(const MaybeRef<U>& other) :
  itsHandle(other.get())
{}

static const char vcid_ref_h[] = "$Header$";
#endif // !REF_H_DEFINED
