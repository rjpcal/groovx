///////////////////////////////////////////////////////////////////////
//
// ref.h
//
// Copyright (c) 2000-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Thu Oct 26 17:50:59 2000
// written: Wed Mar 19 12:45:37 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef REF_H_DEFINED
#define REF_H_DEFINED

#include "util/algo.h"
#include "util/object.h"
#include "util/traits.h"

namespace Util
{
  enum RefType { WEAK, STRONG };

  enum RefVis
    {
      PRIVATE,   // ObjDb gets no reference to the object
      PROTECTED, // ObjDb gets a weak reference to the object
      PUBLIC     // ObjDb gets a strong reference to the object
    };

  template <class T> class Ref;
  template <class T> class SoftRef;

  template <class T> class FloatingRef;
}

using Util::Ref;
using Util::SoftRef;

///////////////////////////////////////////////////////////////////////
//
// Util::RefHelper helper functions
//
///////////////////////////////////////////////////////////////////////

namespace Util
{
  namespace RefHelper
  {
    bool isValidId(Util::UID id);
    Util::Object* getCheckedItem(Util::UID id);

    void insertItem(Util::Object* obj);
    void insertItemWeak(Util::Object* obj);

    void throwError(const char* msg);

    template <class T>
    inline T* getCastedItem(Util::UID id)
    {
      Util::Object* obj = getCheckedItem(id);
      T& t = dynamic_cast<T&>(*obj);
      return &t;
    }

    template <>
    inline Util::Object* getCastedItem<Util::Object>(Util::UID id)
    { return getCheckedItem(id); }

    /// Policy class which unrefs objects by decrementing their ref count.
    template <class T>
    struct DefaultUnrefPolicy
    {
      static void unref(T* t) { t->decrRefCount(); }
    };

    /// Policy class which decrements ref count without deletion.
    template <class T>
    struct NoDeleteUnrefPolicy
    {
      static void unref(T* t) { t->decrRefCountNoDelete(); }
    };

    /// A shared implementation class for Util::FloatingRef and Util::Ref.
    template <class T, class UnrefPolicy>
    class Handle
    {
    public:
      explicit Handle(T* master) : itsMaster(master)
      {
        if (master == 0)
          throwError("attempted to construct a Ref with a null pointer");

        if (master->isNotShareable())
          throwError("attempted to construct a Ref with an unshareable object");

        itsMaster->incrRefCount();
      }

      ~Handle()
      { UnrefPolicy::unref(itsMaster); }

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
        Util::swap2(itsMaster, other.itsMaster);
      }

      T* itsMaster;

    };

  }
} // end namespace Util::RefHelpers



namespace Util
{

///////////////////////////////////////////////////////////////////////
/**
 *
 * Util::FloatingRef<T> is a ref-counted smart pointer that will NOT
 * delete its pointee when exiting scope. This can be of use to
 * clients who need to allow an object to be returned to a clean state
 * (i.e. with ref count == 0), but need to ref the object
 * temporarily. Since FloatingRef should only be used in restricted
 * scopes, copying and assignment are disallowed.
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class T>
class FloatingRef
{
private:

  typedef Util::RefHelper::Handle<T, Util::RefHelper::NoDeleteUnrefPolicy<T> >
  Handle;

  Handle itsHandle;

  FloatingRef(const FloatingRef&);
  FloatingRef& operator=(const FloatingRef&);

public:
  // Default dtor is fine

  explicit FloatingRef(T* ptr) : itsHandle(ptr) {}

  T* operator->() const { return get(); }
  T& operator*()  const { return *(get()); }

  T* get()        const { return itsHandle.get(); }
};

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
class Ref
{
private:

  typedef Util::RefHelper::Handle<T, Util::RefHelper::DefaultUnrefPolicy<T> >
  Handle;

  Handle itsHandle;

public:
  // Default destructor, copy constructor, operator=() are fine

  explicit Ref(Util::UID id) : itsHandle(RefHelper::getCastedItem<T>(id)) {}

  explicit Ref(T* ptr, RefVis vis = PUBLIC) :
    itsHandle(ptr)
  {
    if      (vis == PUBLIC)    RefHelper::insertItem(ptr);
    else if (vis == PROTECTED) RefHelper::insertItemWeak(ptr);
  }

  template <class U>
  Ref(const Ref<U>& other) : itsHandle(other.get()) {}

  // Will raise an exception if the SoftRef is invalid
  template <class U>
  Ref(const SoftRef<U>& other);

  T* operator->() const { return get(); }
  T& operator*()  const { return *(get()); }

  T* get()        const { return itsHandle.get(); }

  bool operator==(const Ref& other) const
  { return itsHandle == other.itsHandle; }

  bool operator!=(const Ref& other) const
  { return !(operator==(other)); }

  Util::UID id() const { return get()->id(); }
};

/// TypeTraits specialization for Util::Ref smart pointer
template <class T>
struct TypeTraits<Ref<T> >
{
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



namespace Util
{

///////////////////////////////////////////////////////////////////////
/**
 *
 * Util::SoftRef<T> is a ref-counted smart pointer (like
 * Util::Ref<T>) for holding RefCounted objects. Construction of a
 * Util::SoftRef<T> is guaranteed not to fail. Because of this,
 * however, a Util::SoftRef<T> is not guaranteed to always point to a
 * valid object (this must be tested with isValid() before
 * dereferencing). With these characteristics, a Util::SoftRef<T> can
 * be used with volatile RefCounted objects for which only weak
 * references are available.
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class T>
class SoftRef
{
private:

  /// Internal helper class for SoftRef.
  /** WeakHandle manages memory etc., and provides one important guarantee:
      it will never return an invalid pointer from get() (an exception will
      be raised if this would fail). */
  class WeakHandle
  {
  private:
    static Util::RefCounts* getCounts(T* master, RefType tp)
    {
      return (master && (tp == WEAK || master->isNotShareable())) ?
        master->refCounts() : 0;
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

      // else... (itsCounts != 0) implies we are using weak ref's

      return (itsCounts->strongCount() > 0);

      // NOTE: We used to release() before returning if the strongCount()
      // was == 0. But that could cause us to prematurely drop our pointee,
      // since in fact we can't interpret strongCount()==0 to necessarily
      // mean that the pointee is dead -- it could be instead that nobody
      // has yet acquired a strong reference count (e.g., this could happen
      // in the constructor sequence).
    }

    T* get()     const         { ensureValid(); return itsMaster; }
    T* getWeak() const throw() { return isValid() ? itsMaster : 0; }

    RefType refType() const
    {
      return (itsMaster && !itsCounts) ? STRONG : WEAK;
    }

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
      Util::swap2(itsMaster, other.itsMaster);
      Util::swap2(itsCounts, other.itsCounts);
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

public:
  SoftRef() : itsHandle(0, STRONG) {}

  explicit SoftRef(Util::UID id, RefType tp = STRONG) :
    itsHandle(RefHelper::isValidId(id) ?
              RefHelper::getCastedItem<T>(id) : 0,
              tp)
  {}

  void insertItem()
  {
    if (itsHandle.isValid())
      RefHelper::insertItem(itsHandle.get());
  }

  explicit SoftRef(T* master, RefType tp = STRONG, RefVis vis = PUBLIC) :
    itsHandle(master,tp)
  {
    if (itsHandle.isValid())
      {
        if      (vis == PUBLIC)    RefHelper::insertItem(itsHandle.get());
        else if (vis == PROTECTED) RefHelper::insertItemWeak(itsHandle.get());
      }
  }

  template <class U>
  SoftRef(const SoftRef<U>& other) :
    itsHandle(other.isValid() ? other.get() : 0, STRONG) {}

  template <class U>
  SoftRef(const Ref<U>& other) : itsHandle(other.get(), STRONG) {}

  // Default destructor, copy constructor, operator=() are fine

  T* operator->()      const { return itsHandle.get(); }
  T& operator*()       const { return *(itsHandle.get()); }

  /** Returns the pointee, or if throws an exception if there is not a
      valid pointee. */
  T* get()     const         { return itsHandle.get(); }

  /** Returns the pointee, or returns null if there is not a valid
      pointee. Will not throw an exception. */
  T* getWeak() const throw() { return itsHandle.getWeak(); }

  RefType refType() const { return itsHandle.refType(); }

  bool isValid() const { return itsHandle.isValid(); }
  bool isInvalid() const { return !(isValid()); }

  bool operator==(const SoftRef& other) const
  { return itsHandle == other.itsHandle; }

  bool operator!=(const SoftRef& other) const
  { return !(operator==(other)); }

  Util::UID id() const
    { return itsHandle.isValid() ? itsHandle.get()->id() : 0; }
};

/// TypeTraits specialization for SoftRef smart pointer.
template <class T>
struct TypeTraits<SoftRef<T> >
{
  typedef T Pointee;
};

} // end namespace Util

template <class To, class Fr>
SoftRef<To> dynamicCast(SoftRef<Fr> p)
{
  if (p.isValid())
    {
      Fr* f = p.get();
      To& t = dynamic_cast<To&>(*f); // will throw bad_cast on failure
      return SoftRef<To>(&t);
    }
  return SoftRef<To>(p.id());
}

///////////////////////////////////////////////////////////////////////
//
// inline member definitions
//
///////////////////////////////////////////////////////////////////////

template <class T>
template <class U>
inline Util::Ref<T>::Ref(const SoftRef<U>& other) :
  itsHandle(other.get())
{}

static const char vcid_ref_h[] = "$Header$";
#endif // !REF_H_DEFINED
