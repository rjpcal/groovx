///////////////////////////////////////////////////////////////////////
//
// listitem.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Oct  5 18:03:41 2000
// written: Tue Oct 10 09:14:42 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef LISTITEM_H_DEFINED
#define LISTITEM_H_DEFINED

class PtrListBase;

template <class T> class PtrList;

template <class T>
class ListItem {
private:
  T* itsPtr;
  int itsId;
  const PtrListBase* itsList;

protected:
  friend class PtrList<T>;

  template <class TT>
  ListItem(TT* ptr, const PtrListBase& theList, int id) :
	 itsPtr(ptr),
	 itsId(id),
	 itsList(&theList)
	 {}

public:

  ~ListItem() 
  {
	 itsList->decrRefCount(itsId);
  }

  ListItem(const ListItem<T>& other) :
	 itsPtr(other.itsPtr),
	 itsId(other.itsId),
	 itsList(other.itsList)
	 {
		itsList->incrRefCount(itsId);
	 }

  template <class TT>
  ListItem(const ListItem<TT>& other) :
	 itsPtr(other.itsPtr),
	 itsId(other.itsId),
	 itsList(other.itsList)
	 {
		itsList->incrRefCount(itsId);
	 }

  ListItem<T>& operator=(const ListItem<T>& other)
	 {
		itsList->decrRefCount(itsId);

		itsPtr = other.itsPtr;
		itsId = other.itsId;
		itsList = other.itsList;
		itsList->incrRefCount(itsId);

		return *this;
	 }
 
  template <class TT>
  ListItem<T>& operator=(const ListItem<TT>& other)
	 {
		itsList->decrRefCount(itsId);

		itsPtr = other.itsPtr;
		itsId = other.itsId;
		itsList = other.itsList;
		itsList->incrRefCount(itsId);

		return *this;
	 }
 
        T* operator->()       { return itsPtr; }
  const T* operator->() const { return itsPtr; }
        T& operator*()        { return *itsPtr; }
  const T& operator*()  const { return *itsPtr; }

        T* get()              { return itsPtr; }
  const T* get()        const { return itsPtr; }

  int id() const { return itsId; }

  bool operator==(const ListItem<T>& rhs) const
	 { return itsPtr == rhs.itsPtr; }

  bool operator!=(const ListItem<T>& rhs) const
    { return itsPtr != rhs.itsPtr; }

  bool operator==(T* rhs) const
    { return itsPtr == rhs; }

  bool operator!=(T* rhs) const
    { return itsPtr != rhs; }

  bool operator!() const
    { return itsPtr == 0; }
};

static const char vcid_listitem_h[] = "$Header$";
#endif // !LISTITEM_H_DEFINED
