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

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(PTRHANDLE_H_DEFINED)
#include "util/ptrhandle.h"
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



template <class T>
class PrivPtrHandle {
public:
  explicit PrivPtrHandle(T* master) : itsMaster(master)
  {
    if (master == 0) Util::RefHelper::throwErrorWithMsg(
                        "T* was null in PrivPtrHandle<T>()");
    itsMaster->incrRefCount();
  }

  ~PrivPtrHandle()
    { itsMaster->decrRefCount(); }

  PrivPtrHandle(const PrivPtrHandle& other) : itsMaster(other.itsMaster)
    { itsMaster->incrRefCount(); }

  template <class U> friend class PrivPtrHandle;

  template <class U>
  PrivPtrHandle(const PrivPtrHandle<U>& other) : itsMaster(other.itsMaster)
    { itsMaster->incrRefCount(); }

  PrivPtrHandle& operator=(const PrivPtrHandle& other)
    {
      PrivPtrHandle otherCopy(other);
      this->swap(otherCopy);
      return *this;
    }

  T* operator->() const { return itsMaster; }
  T& operator*()  const { return *itsMaster; }

  T* get()        const { return itsMaster; }

  void swap(PrivPtrHandle& other)
    {
      T* otherMaster = other.itsMaster;
      other.itsMaster = this->itsMaster;
      this->itsMaster = otherMaster;
    }

  T* itsMaster;
};


///////////////////////////////////////////////////////////////////////
/**
 *
 * Util::Ref<T> is a wrapper of a PrivPtrHandle<T> along with an integer
 * index from a PtrList<T>.
 *
 **/
///////////////////////////////////////////////////////////////////////

namespace Util {

template <class T>
class Ref : private PrivPtrHandle<T> {
public:
  // Default destructor, copy constructor, operator=() are fine

  explicit Ref(Util::UID id) : PrivPtrHandle<T>(RefHelper::getCastedItem<T>(id)) {}

  explicit Ref(T* ptr) : PrivPtrHandle<T>(ptr)
    { RefHelper::insertItem(ptr); }

  Ref(T* ptr, bool /*noInsert*/) : PrivPtrHandle<T>(ptr) {}

  template <class U>
  Ref(const Ref<U>& other) : PrivPtrHandle<T>(other.get()) {}

  // Will raise an exception if the MaybeRef is invalid
  template <class U>
  Ref(const MaybeRef<U>& other);

  T* operator->() const { return get(); }
  T& operator*()  const { return *(get()); }

  T* get()        const { return PrivPtrHandle<T>::get(); }

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
  mutable NullablePtrHandle<T> itsHandle;
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


//    MaybeRef(PtrHandle<T> item) : itsHandle(item), itsId(item->id())
//      { insertItem(); }

//    MaybeRef(PtrHandle<T> item, bool /*noInsert*/) :
//      itsHandle(item), itsId(item->id())
//      {}


  MaybeRef(NullablePtrHandle<T> item) :
    itsHandle(item), itsId(0)
  {
    if (itsHandle.isValid()) itsId = itsHandle->id();
    insertItem();
  }

  MaybeRef(NullablePtrHandle<T> item, bool /*noInsert*/) :
    itsHandle(item), itsId(0)
  {
    if (itsHandle.isValid()) itsId = itsHandle->id();
  }


  template <class U>
  MaybeRef(const MaybeRef<U>& other) :
    itsHandle(other.handle()), itsId(other.id()) {}

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
        itsHandle = NullablePtrHandle<T>(p.get());
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
          itsHandle = NullablePtrHandle<T>(p.get());
          if (itsId != itsHandle->id())
            throw ErrorWithMsg("assertion failed in attemptRefresh");
        }
      }
  }


  bool isValid() const { attemptRefresh(); return itsHandle.isValid(); }

  NullablePtrHandle<T> handle() const { attemptRefresh(); return itsHandle; }
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
  PrivPtrHandle<T>(other.get())
{}

static const char vcid_ref_h[] = "$Header$";
#endif // !REF_H_DEFINED
