///////////////////////////////////////////////////////////////////////
//
// ptrlist.h
// Rob Peters
// created: Fri Apr 23 00:35:31 1999
// written: Sun Nov 21 02:58:19 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PTRLIST_H_DEFINED
#define PTRLIST_H_DEFINED

#ifndef IOPTRLIST_H_DEFINED
#include "ioptrlist.h"
#endif

///////////////////////////////////////////////////////////////////////
/**
 *
 * PtrList is templatized container for pointers. PtrList is
 * responsible for the memory management of all the objects to which
 * it holds pointers. The pointers are accessed by integer indices
 * into the PtrList. There are no operations on the capacity of
 * PtrList; any necessary resizing is done when necessary in an insert
 * call.
 *
 * @memo A template container that stores and owns pointers.
 **/
///////////////////////////////////////////////////////////////////////

template <class T>
class PtrList : public IoPtrList {
public:
  ///
  PtrList (int size);

  ///////////////
  // iterators //
  ///////////////

  ///
  typedef T* pointer;
  ///
  typedef T& reference;
  ///
  typedef T* const_pointer;
  ///
  typedef T& const_reference;
  
  ///
  class iterator {
  private:
	 int itsIndex;
	 PtrList<T>* itsList;
	 
	 // The index is not checked.
	 iterator (PtrList<T>& aList, int index) :
		itsList(&aList),
		itsIndex(index) {}

	 // 'bool check' is a dummy argument to indicate that the index
	 // must be checked
	 iterator (PtrList<T>& aList, int index, bool check) :
		itsList(&aList),
		itsIndex(index)
	 {
		while ( itsIndex > itsList->capacity() ) { --itsIndex; }
		while ( itsIndex < -1 ) { ++itsIndex; }
		if ( !(itsList->isValidId(itsIndex)) ) { ++*this; }
	 }

  public:
	 friend class PtrList<T>;

	 ///
	 iterator (const iterator& rhs) :
		itsList(rhs.itsList),
		itsIndex(rhs.itsIndex) {}

	 ///
	 iterator& operator=(const iterator& rhs) 
		{ itsList = rhs.itsList; itsIndex = rhs.itsIndex; }

	 ///
	 bool operator== (const iterator& x) const 
		{ return (itsIndex == x.itsIndex) && (itsList == x.itsList); }
	 ///
	 bool operator!= (const iterator& x) const
		{ return (itsIndex != x.itsIndex) || (itsList != x.itsList); }

	 ///
	 reference operator* () const { return *(itsList->getPtr(itsIndex)); } 

	 ///
	 pointer operator-> () const { return &(operator*()); }

	 ///
	 iterator& operator++ () { 
		while ( (itsIndex<itsList->capacity()) 
				  && !(itsList->isValidId(++itsIndex)) ) { }
		return *this;
	 }

	 ///
	 iterator operator++ (int)
		{ iterator tmp = *this; ++*this; return tmp; }

	 ///
	 iterator& operator-- () {
		while ( (itsIndex >= -1) 
				  && !(itsList->isValidId(--itsIndex)) ) { }
		return *this;
	 }

	 ///
	 iterator operator-- (int)
		{ iterator tmp = *this; --*this; return tmp; }

	 ///
	 int toInt() const { return itsIndex; }
  };

  ///
  iterator begin() { return iterator(*this, 0); }
  ///
  iterator end() { return iterator(*this, capacity()); }
  ///
  iterator at(int index) { return iterator(*this, index, true); }

  ///////////////
  // accessors //
  ///////////////

public:
  ///
  T* getPtr(int id) const throw () 
	 { return castToT(VoidPtrList::getVoidPtr(id)); }

  ///
  T* getCheckedPtr(int id) const throw (InvalidIdError)
	 { return castToT(VoidPtrList::getCheckedVoidPtr(id)); }

  /** Puts a list of all valid (i.e. within-range and non-null) trial
		ids into the vector<int> that is passed in by reference. */
  void getValidIds(vector<int>& vec) const {
	 vec.clear();
	 insertValidIds(back_inserter(vec));
  }

  //////////////////
  // manipulators //
  //////////////////

  ///
  int insert(T* ptr)
	 { return VoidPtrList::insertVoidPtr(castFromT(ptr)); }

  ///
  void insertAt(int id, T* ptr)
	 { VoidPtrList::insertVoidPtrAt(id, castFromT(ptr)); }

protected:
  /**
	* Casts
	**/
  //@{
  ///
  T* castToT(void* ptr) const;
  ///
  void* castFromT(T* ptr) const;
  ///
  virtual IO* fromVoidToIO(void* ptr) const;
  ///
  virtual void* fromIOToVoid(IO* ptr) const;
  //@}

  ///
  virtual void destroyPtr(void* ptr);
};

static const char vcid_ptrlist_h[] = "$Header$";
#endif // !PTRLIST_H_DEFINED
