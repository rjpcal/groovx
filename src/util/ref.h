///////////////////////////////////////////////////////////////////////
//
// iditem.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Oct 26 17:50:59 2000
// written: Fri Oct 27 11:27:05 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IDITEM_H_DEFINED
#define IDITEM_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(PTRHANDLE_H_DEFINED)
#include "util/ptrhandle.h"
#endif

template <class T> class PtrList;

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
  static PtrList<T>& ptrList();

  PtrHandle<T> itsHandle;
  int itsId;

  void check();

public:
  IdItem(int id);
  IdItem(T* master, int id_) : itsHandle(master), itsId(id_) { check(); }
  IdItem(PtrHandle<T> item_, int id_) : itsHandle(item_), itsId(id_)
    { check(); }

  // Default destructor, copy constructor, operator=() are fine

  /** A symbol class to pass to constructors indicating that the item
		should be inserted into an appropriate PtrList. */
  class Insert {};

  IdItem(T* ptr, Insert /*dummy param*/);
  IdItem(PtrHandle<T> item, Insert /*dummy param*/);

  /** This operation will cause \a new_master to be inserted into an
      appropriate PtrList. */
  IdItem& operator=(T* new_master);

        T* operator->()       { return itsHandle.get(); }
  const T* operator->() const { return itsHandle.get(); }
        T& operator*()        { return *(itsHandle.get()); }
  const T& operator*()  const { return *(itsHandle.get()); }

        T* get()              { return itsHandle.get(); }
  const T* get()        const { return itsHandle.get(); }

  PtrHandle<T> handle() const { return itsHandle; }
  int id() const { return itsId; }


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
  static PtrList<T>& ptrList();

  mutable NullablePtrHandle<T> itsHandle;
  int itsId;

  void check();

public:
  explicit MaybeIdItem(int id_) :
	 itsHandle(0), itsId(id_) { check(); }

  MaybeIdItem(T* master, int id_) :
	 itsHandle(master), itsId(id_) { check(); }

  MaybeIdItem(PtrHandle<T> item_, int id_) :
	 itsHandle(item_), itsId(id_) { check(); }

  MaybeIdItem(NullablePtrHandle<T> item_, int id_) :
	 itsHandle(item_), itsId(id_) { check(); }

  MaybeIdItem(const IdItem<T> other) :
	 itsHandle(other.handle()), itsId(other.id()) { check(); }

  // These will cause the item to be inserted into the relevant list.
  MaybeIdItem(T* master);
  MaybeIdItem(PtrHandle<T> item);

  // Default destructor, copy constructor, operator=() are fine

        T* operator->()       { refresh(); return itsHandle.get(); }
  const T* operator->() const { refresh(); return itsHandle.get(); }
        T& operator*()        { refresh(); return *(itsHandle.get()); }
  const T& operator*()  const { refresh(); return *(itsHandle.get()); }

        T* get()              { refresh(); return itsHandle.get(); }
  const T* get()        const { refresh(); return itsHandle.get(); }

  /** This will try to refresh the handle from the id, but will not
      throw an exception if the operation fails; it will just leave
      the object with an invalid handle. */
  void attemptRefresh() const;

  /** This will try to refresh the handle from the id, and will throw
      an exception if the operation fails (if the id is invalid). */
  void refresh() const;

  bool isValid() const { attemptRefresh(); return itsHandle.isValid(); }

  NullablePtrHandle<T> handle() const { refresh(); return itsHandle; }
  int id() const { return itsId; }


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
