///////////////////////////////////////////////////////////////////////
//
// iditem.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Oct 26 17:50:59 2000
// written: Fri May 11 20:42:21 2001
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
  IdItem(IO::UID id) :
	 itsHandle(IdItemUtils::getCastedItem<T>(id))
  {}

  IdItem(T* master) : itsHandle(master) {}
  IdItem(PtrHandle<T> item_) : itsHandle(item_) {}

  // Default destructor, copy constructor, operator=() are fine

  template <class U>
  IdItem(const IdItem<U>& other) : itsHandle(other.handle()) {}

  /** A symbol class to pass to constructors indicating that the item
		should be inserted into an appropriate PtrList. */
  class Insert {};

  IdItem(T* ptr, Insert /*dummy param*/) :
	 itsHandle(ptr)
  {
	 IdItemUtils::insertItem(ptr);
  }

  IdItem(PtrHandle<T> item, Insert /*dummy param*/) :
	 itsHandle(item)
  {
	 IdItemUtils::insertItem(itsHandle.get());
  }

  /** This operation will cause \a new_master to be inserted into an
      appropriate PtrList. */
  IdItem& operator=(T* new_master)
  {
	 itsHandle = PtrHandle<T>(new_master);
	 IdItemUtils::insertItem(new_master);
	 return *this;
  }

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

template <class Container, class T>
inline IdItemInserter<Container, T>&
IdItemInserter<Container, T>::operator=(T* obj)
{
  itsContainer.push_back(IdItem<T>(obj, IdItem<T>::Insert()));
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
	 itsId(-1)
  {
	 if (master != 0)
		{
		  IdItemUtils::insertItem(master);
		  itsId = master->id();
		}
  }

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
		  IdItem<T> p = IdItemUtils::getCastedItem<T>(itsId);
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
			 IdItem<T> p = IdItemUtils::getCastedItem<T>(itsId);
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

template <class Container, class T>
inline MIdItemInserter<Container, T>&
MIdItemInserter<Container, T>::operator=(T* obj)
{
  itsContainer.push_back(MaybeIdItem<T>(obj));
  return *this;
}

static const char vcid_iditem_h[] = "$Header$";
#endif // !IDITEM_H_DEFINED
