///////////////////////////////////////////////////////////////////////
//
// ref.h
//
// Copyright (c) 2000-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Oct 26 17:50:59 2000
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef REF_H_DEFINED
#define REF_H_DEFINED

#include "util/algo.h"
#include "util/fileposition.h"
#include "util/object.h"
#include "util/stderror.h"
#include "util/traits.h"

#include <typeinfo>

namespace Nub
{
  enum RefType { WEAK, STRONG };

  enum RefVis
    {
      PRIVATE,   // Nub::ObjDb gets no reference to the object
      PROTECTED, // Nub::ObjDb gets a weak reference to the object
      PUBLIC     // Nub::ObjDb gets a strong reference to the object
    };

  template <class T> class Ref;
  template <class T> class SoftRef;

  template <class T> class FloatingRef;

  ///////////////////////////////////////////////////////////
  //
  // Nub::Detail helper functions
  //
  ///////////////////////////////////////////////////////////

  namespace Detail
  {
    bool isValidId(Nub::UID id) throw();
    Nub::Object* getCheckedItem(Nub::UID id);

    void insertItem(Nub::Object* obj);
    void insertItemWeak(Nub::Object* obj);

    void throwRefNull(const std::type_info& info, const rutz::file_pos& pos);
    void throwRefUnshareable(const std::type_info& msg, const rutz::file_pos& pos);
    void throwSoftRefInvalid(const std::type_info& info, const rutz::file_pos& pos);

    template <class T>
    inline T* getCastedItem(Nub::UID id)
    {
      Nub::Object* obj = getCheckedItem(id);
      T* t = dynamic_cast<T*>(obj);
      if (t == 0)
        rutz::throw_bad_cast(typeid(T), typeid(Nub::Object), SRC_POS);
      return t;
    }

    template <>
    inline Nub::Object* getCastedItem<Nub::Object>(Nub::UID id)
    { return getCheckedItem(id); }

    /// Policy class which unrefs objects by decrementing their ref count.
    template <class T>
    struct DefaultUnrefPolicy
    {
      static void unref(T* t) throw() { t->decrRefCount(); }
    };

    /// Policy class which decrements ref count without deletion.
    template <class T>
    struct NoDeleteUnrefPolicy
    {
      static void unref(T* t) throw() { t->decrRefCountNoDelete(); }
    };

    /// A shared implementation class for Nub::FloatingRef and Nub::Ref.
    /** Note that the only operation that can throw is the
        constructor, which throws in case it is passed a null or
        unshareable object pointer .*/
    template <class T, class UnrefPolicy>
    class Handle
    {
    public:
      explicit Handle(T* master) : itsMaster(master)
      {
        if (master == 0)
          throwRefNull(typeid(T), SRC_POS);

        if (master->isNotShareable())
          throwRefUnshareable(typeid(T), SRC_POS);

        itsMaster->incrRefCount();
      }

      ~Handle() throw()
      { UnrefPolicy::unref(itsMaster); }

      Handle(const Handle& other) throw()
        :
        itsMaster(other.itsMaster)
      {
        itsMaster->incrRefCount();
      }

      Handle& operator=(const Handle& other) throw()
      {
        Handle otherCopy(other);
        this->swap(otherCopy);
        return *this;
      }

      T* get() const throw()
      { return itsMaster; }

      bool operator==(const Handle& other) const throw()
      { return itsMaster == other.itsMaster; }

    private:
      void swap(Handle& other) throw()
      {
        rutz::swap2(itsMaster, other.itsMaster);
      }

      T* itsMaster;
    };

  } // end namespace Nub::Detail

  ///////////////////////////////////////////////////////////
  /**
   *
   * Nub::FloatingRef<T> is a ref-counted smart pointer that will NOT
   * delete its pointee when exiting scope. This can be of use to
   * clients who need to allow an object to be returned to a clean
   * state (i.e. with ref count == 0), but need to ref the object
   * temporarily. Since FloatingRef should only be used in restricted
   * scopes, copying and assignment are disallowed.
   *
   **/
  ///////////////////////////////////////////////////////////

  template <class T>
  class FloatingRef
  {
  private:

    typedef Nub::Detail::Handle<T, Nub::Detail::NoDeleteUnrefPolicy<T> >
    Handle;

    Handle itsHandle;

    FloatingRef(const FloatingRef&);
    FloatingRef& operator=(const FloatingRef&);

  public:
    // Default dtor is fine

    explicit FloatingRef(T* ptr) : itsHandle(ptr) {}

    T* operator->() const throw() { return get(); }
    T& operator*()  const throw() { return *(get()); }

    T* get()        const throw() { return itsHandle.get(); }
  };

  ///////////////////////////////////////////////////////////
  /**
   *
   * Nub::Ref<T> is a ref-counted smart pointer for holding RefCounted
   * objects. A Nub::Ref<T> is guaranteed to always point to a valid
   * RefCounted object, and uses RefCounted's strong ref counts to
   * achieve this. In order to provide this guarantee, it is possible
   * that construction of a Nub::Ref<T> may throw an exception. For
   * example, a Nub::Ref<T> cannot be constructed for a volatlie
   * RefCounted object for which only weak references are available.
   *
   **/
  ///////////////////////////////////////////////////////////

  template <class T>
  class Ref
  {
  private:

    typedef Nub::Detail::Handle<T, Nub::Detail::DefaultUnrefPolicy<T> >
    Handle;

    Handle itsHandle;

  public:
    // Default destructor, copy constructor, operator=() are fine

    explicit Ref(Nub::UID id)
      : itsHandle(Detail::getCastedItem<T>(id)) {}

    explicit Ref(T* ptr, RefVis vis = PUBLIC) :
      itsHandle(ptr)
    {
      if      (vis == PUBLIC)    Detail::insertItem(ptr);
      else if (vis == PROTECTED) Detail::insertItemWeak(ptr);
      // else vis == PRIVATE, so don't insert into the Nub::ObjDb
    }

    template <class U>
    Ref(const Ref<U>& other) : itsHandle(other.get()) {}

    // Will raise an exception if the SoftRef is invalid
    template <class U>
    Ref(const SoftRef<U>& other);

    T* operator->() const throw() { return get(); }
    T& operator*()  const throw() { return *(get()); }

    T* get()        const throw() { return itsHandle.get(); }

    bool operator==(const Ref& other) const throw()
    { return itsHandle == other.itsHandle; }

    bool operator!=(const Ref& other) const throw()
    { return !(operator==(other)); }

    Nub::UID id() const throw()
    { return get()->id(); }

    /// Comparison operator for sorting.
    /** E.g. to allow insertion into std::map or std::set, etc. */
    bool operator<(const Ref& other) const throw()
    { return get() < other.get(); }
  };

} // end namespace Nub

namespace rutz
{
  /// type_traits specialization for Nub::Ref smart pointer
  template <class T>
  struct type_traits<Nub::Ref<T> >
  {
    typedef T pointee_t;
  };
}

template <class To, class Fr>
Nub::Ref<To> dynamicCast(Nub::Ref<Fr> p)
{
  Fr* f = p.get();
  To* t = dynamic_cast<To*>(f);
  if (t == 0)
    rutz::throw_bad_cast(typeid(To), typeid(Fr), SRC_POS);
  return Nub::Ref<To>(t);
}



namespace Nub
{
  ///////////////////////////////////////////////////////////
  /**
   *
   * Nub::SoftRef<T> is a ref-counted smart pointer (like Nub::Ref<T>)
   * for holding RefCounted objects. Construction of a Nub::SoftRef<T>
   * is guaranteed not to fail. Because of this, however, a
   * Nub::SoftRef<T> is not guaranteed to always point to a valid
   * object (this must be tested with isValid() before
   * dereferencing). With these characteristics, a Nub::SoftRef<T> can
   * be used with volatile RefCounted objects for which only weak
   * references are available.
   *
   **/
  ///////////////////////////////////////////////////////////

  template <class T>
  class SoftRef
  {
  private:

    /// Internal helper class for SoftRef.
    /** WeakHandle manages memory etc., and provides one important
        guarantee: it will never return an invalid pointer from get()
        (an exception will be raised if this would fail). */
    class WeakHandle
    {
    private:
      static Nub::RefCounts* getCounts(T* master, RefType tp) throw()
      {
        return (master && (tp == WEAK || master->isNotShareable())) ?
          master->refCounts() : 0;
      }

    public:
      WeakHandle(T* master, RefType tp) throw()
        :
        itsMaster(master),
        itsCounts(getCounts(master, tp))
      { acquire(); }

      ~WeakHandle() throw()
      { release(); }

      WeakHandle(const WeakHandle& other) throw()
        :
        itsMaster(other.itsMaster),
        itsCounts(other.itsCounts)
      { acquire(); }

      WeakHandle& operator=(const WeakHandle& other) throw()
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

        return itsCounts->isOwnerAlive();
      }

      T* get()     const         { ensureValid(); return itsMaster; }
      T* getWeak() const throw() { return isValid() ? itsMaster : 0; }

      RefType refType() const throw()
      {
        return (itsMaster && !itsCounts) ? STRONG : WEAK;
      }

    private:
      void acquire() const throw()
      {
        if (itsMaster)
          {
            if (itsCounts) itsCounts->acquireWeak();
            else           itsMaster->incrRefCount();
          }
      }

      void release() const throw()
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
          Nub::Detail::throwSoftRefInvalid(typeid(T), SRC_POS);
      }

      void swap(WeakHandle& other) throw()
      {
        rutz::swap2(itsMaster, other.itsMaster);
        rutz::swap2(itsCounts, other.itsCounts);
      }

      // In order to avoid storing a separate bool indicating whether
      // we are using strong or weak ref's, we use the following
      // convention: if we are strong ref-counting, then itsCounts
      // stays null, but if we are weak ref-counting, then itsCounts
      // will be non-null.

      mutable T* itsMaster;
      mutable Nub::RefCounts* itsCounts;

    }; // end helper class WeakHandle


    mutable WeakHandle itsHandle;

  public:
    SoftRef() : itsHandle(0, STRONG) {}

    explicit SoftRef(Nub::UID id, RefType tp = STRONG)
      :
      itsHandle(Detail::isValidId(id) ?
                Detail::getCastedItem<T>(id) : 0,
                tp)
    {}

    explicit SoftRef(T* master, RefType tp = STRONG, RefVis vis = PUBLIC)
      :
      itsHandle(master,tp)
    {
      if (itsHandle.isValid())
        {
          if      (vis == PUBLIC)    Detail::insertItem(itsHandle.get());
          else if (vis == PROTECTED) Detail::insertItemWeak(itsHandle.get());
          // else vis == PRIVATE, so don't insert into the Nub::ObjDb
        }
    }

    template <class U>
    SoftRef(const SoftRef<U>& other) :
      itsHandle(other.isValid() ? other.get() : 0, STRONG) {}

    template <class U>
    SoftRef(const Ref<U>& other) : itsHandle(other.get(), STRONG) {}

    // Default destructor, copy constructor, operator=() are fine

    /** Returns the pointee, or if throws an exception if there is not a
        valid pointee. */
    T* get()         const { return itsHandle.get(); }

    T* operator->()  const { return itsHandle.get(); }
    T& operator*()   const { return *(itsHandle.get()); }

    /** Returns the pointee, or returns null if there is not a valid
        pointee. Will not throw an exception. */
    T* getWeak() const throw() { return itsHandle.getWeak(); }

    RefType refType() const throw() { return itsHandle.refType(); }

    bool isValid()   const throw() { return itsHandle.isValid(); }
    bool isInvalid() const throw() { return !(isValid()); }

    bool operator==(const SoftRef& other) const throw()
    { return getWeak() == other.getWeak(); }

    bool operator!=(const SoftRef& other) const throw()
    { return getWeak() != other.getWeak(); }

    /// Comparison operator for sorting, to insert in std::map or std::set, etc.
    bool operator<(const SoftRef& other) const throw()
    { return getWeak() < other.getWeak(); }

    Nub::UID id() const throw()
    { return itsHandle.isValid() ? itsHandle.get()->id() : 0; }
  };

} // end namespace Nub

namespace rutz
{
  /// type_traits specialization for SoftRef smart pointer.
  template <class T>
  struct type_traits<Nub::SoftRef<T> >
  {
    typedef T pointee_t;
  };
}

template <class To, class Fr>
Nub::SoftRef<To> dynamicCast(Nub::SoftRef<Fr> p)
{
  if (p.isValid())
    {
      Fr* f = p.get();
      To* t = dynamic_cast<To*>(f);
      if (t == 0)
        rutz::throw_bad_cast(typeid(To), typeid(Fr), SRC_POS);
      return Nub::SoftRef<To>(t);
    }
  return Nub::SoftRef<To>(p.id());
}

///////////////////////////////////////////////////////////////////////
//
// inline member definitions
//
///////////////////////////////////////////////////////////////////////

template <class T>
template <class U>
inline Nub::Ref<T>::Ref(const SoftRef<U>& other) :
  itsHandle(other.get())
{}

static const char vcid_ref_h[] = "$Header$";
#endif // !REF_H_DEFINED
