///////////////////////////////////////////////////////////////////////
//
// iditem.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Oct 26 17:50:59 2000
// written: Thu May 17 10:59:22 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IDITEM_H_DEFINED
#define IDITEM_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(PTRHANDLE_H_DEFINED)
#include "util/ptrhandle.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(ERROR_H_DEFINED)
#include "util/error.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IODECLS_H_DEFINED)
#include "io/iodecls.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IDITEMUTILS_H_DEFINED)
#include "io/iditemutils.h"
#endif

template <class T> class IdItem;
template <class T> class MaybeIdItem;

template <class Container, class T>
class IdItemInserter {
public:
  Container& itsContainer;

  IdItemInserter(Container& c) : itsContainer(c) {}

  IdItemInserter& operator=(T* obj);

  IdItemInserter& operator*() { return *this; }
  IdItemInserter& operator++() { return *this; }
};

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

  explicit IdItem(IO::UID id) :
	 itsHandle(IdItemUtils::getCastedItem<T>(id))
  {}

  explicit IdItem(T* ptr) : itsHandle(ptr)
    { IdItemUtils::insertItem(ptr); }

  explicit IdItem(PtrHandle<T> item) : itsHandle(item)
    { IdItemUtils::insertItem(itsHandle.get()); }

  template <class U>
  IdItem(const IdItem<U>& other) : itsHandle(other.handle()) {}

  // Will raise an exception if the MaybeIdItem is invalid
  template <class U>
  IdItem(const MaybeIdItem<U>& other);

        T* operator->()       { return itsHandle.get(); }
  const T* operator->() const { return itsHandle.get(); }
        T& operator*()        { return *(itsHandle.get()); }
  const T& operator*()  const { return *(itsHandle.get()); }

        T* get()              { return itsHandle.get(); }
  const T* get()        const { return itsHandle.get(); }

  PtrHandle<T> handle() const { return itsHandle; }
  IO::UID id() const { return itsHandle->id(); }


  template <class Container>
  static IdItemInserter<Container, T> makeInserter(Container& c)
	 { return IdItemInserter<Container, T>(c); }
};

template <class To, class Fr>
IdItem<To> dynamicCast(IdItem<Fr> p)
{
  Fr* f = p.get();
  To& t = dynamic_cast<To&>(*f); // will throw bad_cast on failure
  return IdItem<To>(&t);
}

template <class Container, class T>
inline IdItemInserter<Container, T>&
IdItemInserter<Container, T>::operator=(T* obj)
{
  itsContainer.push_back(IdItem<T>(obj));
  return *this;
}

template <class Container, class T>
class MIdItemInserter {
public:
  Container& itsContainer;

  MIdItemInserter(Container& c) : itsContainer(c) {}

  MIdItemInserter& operator=(T* obj);

  MIdItemInserter& operator*() { return *this; }
  MIdItemInserter& operator++() { return *this; }
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
  IO::UID itsId;

public:
  explicit MaybeIdItem(IO::UID id_) :
	 itsHandle(0), itsId(id_) {}

  MaybeIdItem(T* master, IO::UID id_) :
	 itsHandle(master), itsId(id_) {}

  MaybeIdItem(PtrHandle<T> item_, IO::UID id_) :
	 itsHandle(item_), itsId(id_) {}

  MaybeIdItem(NullablePtrHandle<T> item_, IO::UID id_) :
	 itsHandle(item_), itsId(id_) {}

  MaybeIdItem(const IdItem<T> other) :
	 itsHandle(other.handle()), itsId(other.id()) {}

  // These will cause the item to be inserted into the relevant list.
  MaybeIdItem(T* master) :
	 itsHandle(master),
	 itsId(0)
  {
	 if (master != 0)
		{
		  IdItemUtils::insertItem(master);
		  itsId = master->id();
		}
  }

  MaybeIdItem() : itsHandle(0), itsId(0) {}

  MaybeIdItem(PtrHandle<T> item) :
	 itsHandle(item),
	 itsId(item->id())
  {
	 IdItemUtils::insertItem(itsHandle.get());
  }

  // Default destructor, copy constructor, operator=() are fine

        T* operator->()       { refresh(); return itsHandle.get(); }
  const T* operator->() const { refresh(); return itsHandle.get(); }
        T& operator*()        { refresh(); return *(itsHandle.get()); }
  const T& operator*()  const { refresh(); return *(itsHandle.get()); }

        T* get()              { refresh(); return itsHandle.get(); }
  const T* get()        const { refresh(); return itsHandle.get(); }

  /** This will try to refresh the handle from the id, and will throw
      an exception if the operation fails (if the id is invalid). */
  void refresh() const {
	 if ( !itsHandle.isValid() )
		{
		  IdItem<T> p(IdItemUtils::getCastedItem<T>(itsId));
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
			 IdItem<T> p(IdItemUtils::getCastedItem<T>(itsId));
			 itsHandle = p.handle();
			 if (itsId != itsHandle->id())
				throw ErrorWithMsg("assertion failed in attemptRefresh");
		  }
		}
  }


  bool isValid() const { attemptRefresh(); return itsHandle.isValid(); }

  NullablePtrHandle<T> handle() const { refresh(); return itsHandle; }
  IO::UID id() const { return itsId; }


  //
  // Related types
  //

  typedef T ValueType;

  template <class Container>
  static MIdItemInserter<Container, T> makeInserter(Container& c)
	 { return MIdItemInserter<Container, T>(c); }
};

template <class To, class Fr>
MaybeIdItem<To> dynamicCast(MaybeIdItem<Fr> p)
{
  if (p.isValid())
	 {
		Fr* f = p.get();
		To& t = dynamic_cast<To&>(*f); // will throw bad_cast on failure
		return MaybeIdItem<To>(&t, p.id());
	 }
  return MaybeIdItem<To>((To*)0, p.id());
}

template <class Container, class T>
inline MIdItemInserter<Container, T>&
MIdItemInserter<Container, T>::operator=(T* obj)
{
  itsContainer.push_back(MaybeIdItem<T>(obj));
  return *this;
}

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
