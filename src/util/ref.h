///////////////////////////////////////////////////////////////////////
//
// ref.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Oct 26 17:50:59 2000
// written: Sun Aug 19 15:51:16 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef REF_H_DEFINED
#define REF_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(ALGO_H_DEFINED)
#include "util/algo.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(OBJECT_H_DEFINED)
#include "util/object.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TRAITS_H_DEFINED)
#include "util/traits.h"
#endif

namespace Util
{
  enum RefType { WEAK, STRONG };

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

  void throwError(const char* msg);

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



namespace Util {

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

template <class T>
class Ref {
private:

  class Handle {
  public:
    explicit Handle(T* master) : itsMaster(master)
    {
      if (master == 0) Util::RefHelper::throwError(
                            "attempted to construct a Ref with a null pointer");

      if (master->isVolatile()) Util::RefHelper::throwError(
                         "attempted to construct a Ref with a volatile object");

      itsMaster->incrRefCount();
    }

    ~Handle()
    { itsMaster->decrRefCount(); }

    Handle(const Handle& other) : itsMaster(other.itsMaster)
    { itsMaster->incrRefCount(); }

    Handle& operator=(const Handle& other)
    {
      Handle otherCopy(other);
      this->swap(otherCopy);
      return *this;
    }

    T* get() const { return itsMaster; }

    bool operator==(const Handle& other) const
    { return itsMaster == other.itsMaster; }

  private:
    void swap(Handle& other)
    {
      Util::swap(itsMaster, other.itsMaster);
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

  bool operator==(const Ref& other) const
  { return itsHandle == other.itsHandle; }

  bool operator!=(const Ref& other) const
  { return !(operator==(other)); }

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



namespace Util {

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

template <class T>
class WeakRef {
private:

  // This internal helper class manages memory etc., and provides one
  // important guarantee: it will never return an invalid pointer from
  // get() (an exception will be raised if this would fail)
  class WeakHandle {
  private:
    static Util::RefCounts* getCounts(T* master, RefType tp)
    {
      return (master && (tp == WEAK || master->isVolatile())) ?
        master->refCounts() : 0;
    }

    RefType refType() const
    {
      return (itsMaster && itsCounts) ? WEAK : STRONG;
    }

  public:
    WeakHandle(T* master, RefType tp) :
       itsMaster(master),
       itsCounts(getCounts(master, tp))
    { acquire(); }

    ~WeakHandle()
    { release(); }

    WeakHandle(const WeakHandle& other) :
      itsMaster(other.itsMaster),
      itsCounts(other.itsCounts)
    { acquire(); }

    WeakHandle& operator=(const WeakHandle& other)
    {
      WeakHandle otherCopy(other);
      this->swap(otherCopy);
      return *this;
    }

    bool isValid() const throw()
    {
      if (itsCounts == 0) // implies we are using strong ref's
        {
          return (itsMaster != 0);
        }
      else // (itsCounts != 0) implies we are using weak ref's
        {
          if (itsCounts->strongCount() > 0) return true;
        }

      release(); return false;
    }

    T* get()     const         { ensureValid(); return itsMaster; }
    T* getWeak() const throw() { return isValid() ? itsMaster : 0; }

    bool operator==(const WeakHandle& other) const
    {
      this->isValid(); other.isValid(); // force update
      return itsMaster == other.itsMaster;
    }

  private:
    void acquire() const
    {
      if (itsMaster)
        {
          if (itsCounts) itsCounts->acquireWeak();
          else           itsMaster->incrRefCount();
        }
    }

    void release() const
    {
      if (itsMaster)
        {
          if (itsCounts) itsCounts->releaseWeak();
          else           itsMaster->decrRefCount();
        }

      itsCounts = 0; itsMaster = 0;
    }

    void ensureValid() const
    {
      if (!isValid())
        Util::RefHelper::throwError(
                         "attempted to access invalid object");
    }

    void swap(WeakHandle& other)
    {
      Util::swap(itsMaster, other.itsMaster);
      Util::swap(itsCounts, other.itsCounts);
    }

    // In order to avoid storing a separate bool indicating whether we
    // are using strong or weak ref's, we use the following
    // convention: if we are strong ref-counting, then itsCounts stays
    // null, but if we are weak ref-counting, then itsCounts will be
    // non-null.

    mutable T* itsMaster;
    mutable Util::RefCounts* itsCounts;

  }; // end helper class WeakHandle


  mutable WeakHandle itsHandle;

  void insertItem()
  {
    if (itsHandle.isValid())
      RefHelper::insertItem(itsHandle.get());
  }

public:
  WeakRef() : itsHandle(0, STRONG) {}

  explicit WeakRef(Util::UID id, RefType tp = STRONG) :
    itsHandle(RefHelper::isValidId(id) ?
              RefHelper::getCastedItem<T>(id) : 0,
              tp)
  {}

  explicit WeakRef(T* master, RefType tp = STRONG) : itsHandle(master, tp)
  { insertItem(); }

  WeakRef(T* master, bool /*noInsert*/, RefType tp = STRONG) :
    itsHandle(master,tp)
  {}


  template <class U>
  WeakRef(const WeakRef<U>& other) :
    itsHandle(other.isValid() ? other.get() : 0, STRONG) {}

  template <class U>
  WeakRef(const Ref<U>& other) : itsHandle(other.get(), STRONG) {}

  // Default destructor, copy constructor, operator=() are fine

  T* operator->()      const { return itsHandle.get(); }
  T& operator*()       const { return *(itsHandle.get()); }

  /** Returns the pointee, or if throws an exception if there is not a
      valid pointee. */
  T* get()     const         { return itsHandle.get(); }

  /** Returns the pointee, or returns null if there is not a valid
      pointee. Will not throw an exception. */
  T* getWeak() const throw() { return itsHandle.getWeak(); }

  bool isValid() const { return itsHandle.isValid(); }
  bool isInvalid() const { return !(isValid()); }

  bool operator==(const WeakRef& other) const
  { return itsHandle == other.itsHandle; }

  bool operator!=(const WeakRef& other) const
  { return !(operator==(other)); }

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
