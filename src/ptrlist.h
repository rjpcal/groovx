///////////////////////////////////////////////////////////////////////
//
// ptrlist.h
// Rob Peters
// created: Fri Apr 23 00:35:31 1999
// written: Wed Feb 16 08:17:00 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PTRLIST_H_DEFINED
#define PTRLIST_H_DEFINED

#ifndef DUMBPTR_H_DEFINED
#include "dumbptr.h"
#endif

#ifndef IOPTRLIST_H_DEFINED
#include "ioptrlist.h"
#endif

///////////////////////////////////////////////////////////////////////
/**
 *
 * PtrList is templatized container that stores pointers and owns
 * (memory-manages) the objects to which the pointers refer. PtrList
 * is responsible for the memory management of all the objects to
 * which it holds pointers. The pointers are accessed by integer
 * indices into the PtrList. There are no operations on the capacity
 * of PtrList; any necessary resizing is done when necessary in an
 * insert call.
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class T>
class PtrList : public IoPtrList {
public:
  /// Construct and reserve space for \a size objects.
  PtrList (int size);

  /** Virtual destructor calls \c clear(), according to the
      requirement of \c VoidPtrList, to ensure no memory
      leaks. Further subclasses of \c PtrList do not need to call \c
      clear() from their destructors. */
  virtual ~PtrList();

  //////////////
  // pointers //
  //////////////

  typedef DumbPtr<T> Ptr;

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
		itsIndex(index),
		itsList(&aList) {}

	 // 'bool check' is a dummy argument to indicate that the index
	 // must be checked
	 iterator (PtrList<T>& aList, int index, bool check) :
		itsIndex(index),
		itsList(&aList)
	 {
		while ( itsIndex > itsList->capacity() ) { --itsIndex; }
		while ( itsIndex < -1 ) { ++itsIndex; }
		if ( !(itsList->isValidId(itsIndex)) ) { ++*this; }
	 }

  public:
	 friend class PtrList<T>;

	 ///
	 iterator (const iterator& rhs) :
		itsIndex(rhs.itsIndex),
		itsList(rhs.itsList) {}

	 ///
	 iterator& operator=(const iterator& rhs) 
		{ itsList = rhs.itsList; itsIndex = rhs.itsIndex; return *this; }

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
  /** Return the object at index \a id. No range check is performed on
      \a id, so the behavior is undefined if \a id is not a valid
      index into the list. If \a id is out of range, a segmentation
      fault may occur. If \a is in range, but is not an index for a
      valid object, the pointer returned may be null. */
  Ptr getPtr(int id) const throw () 
	 { return DumbPtr<T>(castToT(VoidPtrList::getVoidPtr(id))); }

  /** Returns the object at index \a id, after a check is performed to
      ensure that \a id is in range, and refers to a valid object. If
      the check fails, an \c InvalidIdError exception is thrown. */
  Ptr getCheckedPtr(int id) const throw (InvalidIdError)
	 { return DumbPtr<T>(castToT(VoidPtrList::getCheckedVoidPtr(id))); }

  /** Puts a list of all valid (i.e. within-range and non-null) trial
		ids into the vector<int> that is passed in by reference. */
  void getValidIds(vector<int>& vec) const {
	 vec.clear();
	 insertValidIds(back_inserter(vec));
  }

  //////////////////
  // manipulators //
  //////////////////

  /// Insert \a ptr into the list, and return its id.
  int insert(Ptr ptr)
	 { return VoidPtrList::insertVoidPtr(castFromT(ptr.get())); }

  /** Insert \a ptr into the list at index \a id. If an object
      previously existed at index \a id, that object will be properly
      destroyed. */
  void insertAt(int id, Ptr ptr)
	 { VoidPtrList::insertVoidPtrAt(id, castFromT(ptr.get())); }

protected:
  /// Safely cast \a ptr to the correct type for this list.
  T* castToT(void* ptr) const;

  /// Cast \a ptr back to \c void*.
  void* castFromT(T* ptr) const;

  virtual IO* fromVoidToIO(void* ptr) const;
  virtual void* fromIOToVoid(IO* ptr) const;

  /// Casts \a ptr to the correct type, then \c delete's it.
  virtual void destroyPtr(void* ptr);
};

static const char vcid_ptrlist_h[] = "$Header$";
#endif // !PTRLIST_H_DEFINED
