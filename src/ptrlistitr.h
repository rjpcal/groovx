
template <class T>
class ItemWithId {
private:
  PtrHandle<T> itsItem;
  const int itsId;

public:
  ItemIdPair(MasterPtr<T>* master, int id_) : itsItem(master), itsId(id_) {}
  ItemIdPair(PtrHandle<T> item_, int id_) : itsItem(item_), itsId(id_) {}

  // Default destructor, copy constructor, operator=() are fine

        T* operator->()       { return (itsItem.masterPtr()->getPtr()); }
  const T* operator->() const { return (itsItem.masterPtr()->getPtr()); }
        T& operator*()        { return *(itsItem.masterPtr()->getPtr()); }
  const T& operator*()  const { return *(itsItem.masterPtr()->getPtr()); }

        T* get()              { return (itsItem.masterPtr()->getPtr()); }
  const T* get()        const { return (itsItem.masterPtr()->getPtr()); }

  int id() const { return itsId; }
};

template <class T> class PtrList;

template <class T>
class PtrListItr {
public:
  friend class PtrList<T>;

  struct IdItemPair
  {
	 IdItemPair(int id_, PtrHandle<T> item_) : id(id_), item(item_) {}

	 int id;
	 PtrHandle<T> item;
  };

  typedef IdPtrPair value_type;
  typedef IdPtrPair& reference;
  typedef IdPtrPair* pointer;

  PtrListItr (const PtrListItr& rhs) :
	 itsPair(rhs.itsIndex, 0),
	 itsList(rhs.itsList)
	 {
		invariant();
	 }

  PtrListItr& operator=(const PtrListItr& rhs) 
	 { 
		itsList = rhs.itsList;
		itsPair = rhs.itsPair;
		return *this;
		invariant();
	 }

  bool operator== (const PtrListItr& x) const 
	 { return (itsPair.id == x.itsPair.id) && (itsList == x.itsList); }

  bool operator!= (const PtrListItr& x) const
	 { return (itsPair.id != x.itsPair.id) || (itsList != x.itsList); }

  bool operator< (const PtrListItr& other) const
	 { return (itsPair.id < other.itsPair.id); }

  bool operator> (const PtrListItr& other) const
	 { return (itsPair.id > other.itsPair.id); }

  reference operator* () const;

  pointer operator-> () const { return &(operator*()); }

  PtrListItr& operator++ ();

  PtrListItr operator++ (int)
	 { PtrListItr tmp = *this; ++*this; return tmp; }

  PtrListItr& operator-- ();

  PtrListItr operator-- (int)
	 { PtrListItr tmp = *this; --*this; return tmp; }

private:
  IdItemPair itsPair;

  void invariant();

  // The index is not checked.
  PtrListItr (PtrList<T>& aList, int index) :
	 itsPair(index, 0),
	 itsList(&aList)
	 {
		invariant();
	 }

  // 'bool check' is a dummy argument to indicate that the index
  // must be checked
  PtrListItr (PtrList<T>& aList, int index, bool /*check?*/);
};


// .cc file

#include "ptrlist.h"

template <class T>
void PtrListItr<T>::invariant()
{
  Assert(itsPair.id >= -1);	  // we allow -1 as "one-before-the-beginning"
  Assert(itsPair.id <= itsList->capacity());
}

template <class T>
PtrListItr<T>::PtrListItr(PtrList<T>& aList, int index, bool /*check?*/) :
  itsPair(index, 0),
  itsList(&aList)
{
  // First bring the index to within the valid range...
  while ( itsPair.id > itsList->capacity() ) { --itsPair.id; }
  while ( itsPair.id < -1 ) { ++itsPair.id; }

  // Then increment until we get to the next valid id (or reach the end)
  if ( !(itsList->isValidId(itsPair.id)) ) { ++*this; }

  invariant();
}

template <class T>
PtrListItr<T>::reference PtrListItr<T>::operator* () const 
{ 
  invariant();
  itsPair.item = itsList->getCheckedPtr(itsPair.id);
  return itsPair;
}

template <class T>
PtrListItr<T>& PtrListItr<T>::operator++ ()
{
  while ( itsPair.id < itsList->capacity() ) 
	 {
		if (itsList->isValidId(++itsPair.id))
		  break;
	 }
  invariant();
  return *this;
}

template <class T>
PtrListItr<T>& PtrListItr<T>::operator-- ()
{
  while ( itsPair.id > -1 )
	 {
		if (itsList->isValidId(--itsPair.id))
		  break;
	 }
  invariant();
  return *this;
}
