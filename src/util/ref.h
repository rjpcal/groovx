///////////////////////////////////////////////////////////////////////
//
// ref.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Oct 26 17:50:59 2000
// written: Wed Jun 13 13:16:50 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef REF_H_DEFINED
#define REF_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(OBJECT_H_DEFINED)
#include "util/object.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TRAITS_H_DEFINED)
#include "util/traits.h"
#endif

namespace Util
{
  template <class T> class Ref;
  template <class T> class WeakRef;
}

using Util::Ref;
using Util::WeakRef;

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
 * achieve this. In order to provide this guarantee, it is possible
 * that construction of a Util::Ref<T> may throw an exception. For
 * example, a Util::Ref<T> cannot be constructed for a volatlie
 * RefCounted object for which only weak references are available.
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

      if (master->isVolatile()) Util::RefHelper::throwErrorWithMsg(
                         "attempted to construct a Ref with a volatile object");

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

  // Will raise an exception if the WeakRef is invalid
  template <class U>
  Ref(const WeakRef<U>& other);

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
 * Util::WeakRef<T> is a ref-counted smart pointer (like
 * Util::Ref<T>) for holding RefCounted objects. Construction of a
 * Util::WeakRef<T> is guaranteed not to fail. Because of this,
 * however, a Util::WeakRef<T> is not guaranteed to always point to a
 * valid object (this must be tested with isValid() before
 * dereferencing). With these characteristics, a Util::WeakRef<T> can
 * be used with volatile RefCounted objects for which only weak
 * references are available.
 *
 **/
///////////////////////////////////////////////////////////////////////

namespace Util {

template <class T>
class WeakRef {
private:

  // This internal helper class manages memory etc., and provides one
  // important guarantee: it will never return an invalid pointer from
  // get() (an exception will be raised if this would fail)
  class WeakHandle {
  private:
    Util::RefCounts* getCounts(T* master)
    {
      return master ? master->refCounts() : 0;
    }

    bool isVolatile(T* master)
    {
      return master ? master->isVolatile() : true;
    }

    static void ensure(bool expr, const char* where)
    {
      if (!expr) RefHelper::throwErrorWithMsg(where);
    }

  public:
    explicit WeakHandle(T* master) : itsMaster(master),
                                     itsCounts(getCounts(master)),
                                     itsIsWeakRef(isVolatile(master))
    { acquire(); }

    ~WeakHandle()
    { release(); }

    WeakHandle(const WeakHandle& other) : itsMaster(other.itsMaster),
                                          itsCounts(getCounts(itsMaster)),
                                          itsIsWeakRef(isVolatile(itsMaster))
    { acquire(); }

    template <class U> friend class WeakHandle;

    template <class U>
    WeakHandle(const WeakHandle<U>& other) : itsMaster(other.itsMaster),
                                             itsCounts(getCounts(itsMaster)),
                                             itsIsWeakRef(isVolatile(itsMaster))
    { acquire(); }

    WeakHandle& operator=(const WeakHandle& other)
    {
      WeakHandle otherCopy(other);
      this->swap(otherCopy);
      return *this;
    }

    bool isValid() const
    {
      if (itsCounts != 0 && itsCounts->strongCount() > 0) return true;

      release(); return false;
    }

    T* get()        const { ensureValid(); return itsMaster; }

  private:
    void acquire() const
    {
      ensure(bool(itsMaster) == bool(itsCounts), "WeakHandle::acquire");

      if (itsCounts)
        {
          if (itsIsWeakRef) itsCounts->acquireWeak();
          else              itsMaster->incrRefCount();
        }
    }

    void release() const
    {
      ensure(bool(itsMaster) == bool(itsCounts), "WeakHandle::release");

      if (itsCounts)
        {
          if (itsIsWeakRef) itsCounts->releaseWeak();
          else              itsMaster->decrRefCount();
        }

      itsCounts = 0; itsMaster = 0; itsIsWeakRef = true;
    }

    void ensureValid() const
    {
      if (!isValid())
        Util::RefHelper::throwErrorWithMsg(
                         "attempt to dereference invalid WeakHandle");
    }

    void swap(WeakHandle& other)
    {
      T* otherMaster = other.itsMaster;
      other.itsMaster = this->itsMaster;
      this->itsMaster = otherMaster;

      Util::RefCounts* otherCounts = other.itsCounts;
      other.itsCounts = this->itsCounts;
      this->itsCounts = otherCounts;

      bool otherIsVolatile = other.itsIsWeakRef;
      other.itsIsWeakRef = this->itsIsWeakRef;
      this->itsIsWeakRef = otherIsVolatile;
    }

    mutable T* itsMaster;
    mutable Util::RefCounts* itsCounts;
    mutable bool itsIsWeakRef;

  }; // end helper class WeakHandle


  mutable WeakHandle itsHandle;

  void insertItem()
  {
    if (itsHandle.isValid())
      RefHelper::insertItem(itsHandle.get());
  }

public:
  WeakRef() : itsHandle(0) {}

  explicit WeakRef(Util::UID id) :
    itsHandle(RefHelper::isValidId(id) ?
              RefHelper::getCastedItem<T>(id) : 0)
  {}

  explicit WeakRef(T* master) : itsHandle(master)
  { insertItem(); }

  WeakRef(T* master, bool /*noInsert*/) : itsHandle(master) {}


  template <class U>
  WeakRef(const WeakRef<U>& other) :
    itsHandle(other.isValid() ? other.get() : 0) {}

  template <class U>
  WeakRef(const Ref<U>& other) : itsHandle(other.get()) {}

  // Default destructor, copy constructor, operator=() are fine

  T* operator->() const { return itsHandle.get(); }
  T& operator*()  const { return *(itsHandle.get()); }

  T* get()        const { return itsHandle.get(); }


  bool isValid() const { return itsHandle.isValid(); }
  bool isInvalid() const { return !(isValid()); }

  Util::UID id() const
    { return itsHandle.isValid() ? itsHandle.get()->id() : 0; }
};

// TypeTraits specialization for WeakRef smart pointer

  template <class T>
  struct TypeTraits<WeakRef<T> > {
    typedef T Pointee;
  };

} // end namespace Util

template <class To, class Fr>
WeakRef<To> dynamicCast(WeakRef<Fr> p)
{
  if (p.isValid())
    {
      Fr* f = p.get();
      To& t = dynamic_cast<To&>(*f); // will throw bad_cast on failure
      return WeakRef<To>(&t);
    }
  return WeakRef<To>(p.id());
}

///////////////////////////////////////////////////////////////////////
//
// inline member definitions
//
///////////////////////////////////////////////////////////////////////

template <class T>
template <class U>
inline Util::Ref<T>::Ref(const WeakRef<U>& other) :
  itsHandle(other.get())
{}

static const char vcid_ref_h[] = "$Header$";
#endif // !REF_H_DEFINED
