///////////////////////////////////////////////////////////////////////
//
// iditem.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Oct 26 17:50:59 2000
// written: Tue Jun  5 10:41:32 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IDITEM_H_DEFINED
#define IDITEM_H_DEFINED

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


template <class T> class IdItem;
template <class T> class MaybeIdItem;


///////////////////////////////////////////////////////////////////////
//
// IdItemUtils helper functions
//
///////////////////////////////////////////////////////////////////////

namespace IdItemUtils {
  bool isValidId(Util::UID id);
  Util::Object* getCheckedItem(Util::UID id);

  void insertItem(Util::Object* obj);

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
}



///////////////////////////////////////////////////////////////////////
/**
 *
 * IdItem<T> is a wrapper of a PtrHandle<T> along with an integer
 * index from a PtrList<T>.
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class T>
class IdItem {
private:
  PtrHandle<T> itsHandle;

public:
  // Default destructor, copy constructor, operator=() are fine

  explicit IdItem(Util::UID id) : itsHandle(IdItemUtils::getCastedItem<T>(id)) {}

  explicit IdItem(T* ptr) : itsHandle(ptr)
    { IdItemUtils::insertItem(ptr); }

  IdItem(T* ptr, bool /*noInsert*/) : itsHandle(ptr) {}

  IdItem(PtrHandle<T> item) : itsHandle(item)
    { IdItemUtils::insertItem(item.get()); }

  IdItem(PtrHandle<T> item, bool /*noInsert*/) : itsHandle(item) {}

  template <class U>
  IdItem(const IdItem<U>& other) : itsHandle(other.handle()) {}

  // Will raise an exception if the MaybeIdItem is invalid
  template <class U>
  IdItem(const MaybeIdItem<U>& other);

  T* operator->() const { return itsHandle.get(); }
  T& operator*()  const { return *(itsHandle.get()); }

  T* get()        const { return itsHandle.get(); }

  PtrHandle<T> handle() const { return itsHandle; }
  Util::UID id() const { return itsHandle->id(); }
};

template <class To, class Fr>
IdItem<To> dynamicCast(IdItem<Fr> p)
{
  Fr* f = p.get();
  To& t = dynamic_cast<To&>(*f); // will throw bad_cast on failure
  return IdItem<To>(&t);
}

// TypeTraits specialization for IdItem smart pointer

namespace Util {
  template <class T>
  struct TypeTraits<IdItem<T> > {
	 typedef T Pointee;
  };
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * MaybeIdItem<T> is a wrapper of a NullablePtrHandle<T> along
 * with an integer index from a PtrList<T>.
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class T>
class MaybeIdItem {
private:
  mutable NullablePtrHandle<T> itsHandle;
  Util::UID itsId;

  void insertItem()
  {
	 if (itsHandle.isValid())
		IdItemUtils::insertItem(itsHandle.get());
  }

public:
  MaybeIdItem() : itsHandle(0), itsId(0) {}

  explicit MaybeIdItem(Util::UID id_) : itsHandle(0), itsId(id_) {}

  explicit MaybeIdItem(T* master) : itsHandle(master), itsId(0)
  {
	 if (master != 0) itsId = master->id();
	 insertItem();
  }

  MaybeIdItem(T* master, bool /*noInsert*/) : itsHandle(master), itsId(0)
    { if (master != 0) itsId = master->id(); }


  MaybeIdItem(PtrHandle<T> item) : itsHandle(item), itsId(item->id())
    { insertItem(); }

  MaybeIdItem(PtrHandle<T> item, bool /*noInsert*/) :
	 itsHandle(item), itsId(item->id())
    {}


  MaybeIdItem(NullablePtrHandle<T> item) :
	 itsHandle(item), itsId(0)
  {
	 if (itsHandle.isValid()) itsId = itsHandle->id();
    insertItem();
  }

  MaybeIdItem(NullablePtrHandle<T> item, bool /*noInsert*/) :
	 itsHandle(item), itsId(0)
  {
	 if (itsHandle.isValid()) itsId = itsHandle->id();
  }


  template <class U>
  MaybeIdItem(const MaybeIdItem<U>& other) :
	 itsHandle(other.handle()), itsId(other.id()) {}

  template <class U>
  MaybeIdItem(const IdItem<U>& other) :
	 itsHandle(other.handle()), itsId(other.id()) {}

  // Default destructor, copy constructor, operator=() are fine

  T* operator->() const { refresh(); return itsHandle.get(); }
  T& operator*()  const { refresh(); return *(itsHandle.get()); }
  
  T* get()        const { refresh(); return itsHandle.get(); }

  /** This will try to refresh the handle from the id, and will throw
      an exception if the operation fails (if the id is invalid). */
  void refresh() const {
	 if ( !itsHandle.isValid() )
		{
		  IdItem<T> p(itsId);
		  itsHandle = p.handle();
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
		  if (IdItemUtils::isValidId(itsId)) {
			 IdItem<T> p(itsId);
			 itsHandle = p.handle();
			 if (itsId != itsHandle->id())
				throw ErrorWithMsg("assertion failed in attemptRefresh");
		  }
		}
  }


  bool isValid() const { attemptRefresh(); return itsHandle.isValid(); }

  NullablePtrHandle<T> handle() const { attemptRefresh(); return itsHandle; }
  Util::UID id() const { return itsId; }
};

template <class To, class Fr>
MaybeIdItem<To> dynamicCast(MaybeIdItem<Fr> p)
{
  if (p.isValid())
	 {
		Fr* f = p.get();
		To& t = dynamic_cast<To&>(*f); // will throw bad_cast on failure
		return MaybeIdItem<To>(&t);
	 }
  return MaybeIdItem<To>(p.id());
}

// TypeTraits specialization for MaybeIdItem smart pointer

namespace Util {
  template <class T>
  struct TypeTraits<MaybeIdItem<T> > {
	 typedef T Pointee;
  };
};

///////////////////////////////////////////////////////////////////////
//
// inline member definitions
//
///////////////////////////////////////////////////////////////////////

template <class T>
template <class U>
inline IdItem<T>::IdItem(const MaybeIdItem<U>& other) :
  itsHandle(other.get())
{}

static const char vcid_iditem_h[] = "$Header$";
#endif // !IDITEM_H_DEFINED
