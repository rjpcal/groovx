///////////////////////////////////////////////////////////////////////
//
// ptrlist.h
// Rob Peters
// created: Fri Apr 23 00:35:31 1999
// written: Mon Jul  5 15:18:33 1999
// $Id$
//
// PtrList is type-parameterized container for pointers. PtrList is
// responsible for the memory management of all the objects to which
// it holds pointers. There are no operations on the capacity of
// PtrList; any necessary resizing is done when necessary in an insert
// call.
//
///////////////////////////////////////////////////////////////////////

#ifndef PTRLIST_H_DEFINED
#define PTRLIST_H_DEFINED

#ifndef VECTOR_DEFINED
#include <vector>
#define VECTOR_DEFINED
#endif

#ifndef TCL_H_DEFINED
#include <tcl.h>
#define TCL_H_DEFINED
#endif

#ifndef IO_H_DEFINED
#include "io.h"
#endif

///////////////////////////////////////////////////////////////////////
//
// PtrList class
//
///////////////////////////////////////////////////////////////////////

template <class T>
class PtrList : public virtual IO {
private:
  Tcl_HashTable* itsHashTable;
  int itsFirstVacant;           // smallest index of a vacant array site

public:
  PtrList(int);

  virtual ~PtrList();

  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);
  // These functions write/read the object's state from/to an
  // output/input stream. All objects that are contained by pointer in
  // the PtrList will be written and read. A PtrList that is written
  // and then re-read will have all of the same objects available at
  // the same indices as before the first serialize operation.

  virtual int charCount() const;

  ///////////////
  // iterators //
  ///////////////

  class iterator {
  private:
	 Tcl_HashSearch itsSearch;
	 Tcl_HashEntry* itsEntry;
	 Tcl_HashTable* itsTable;
	 
	 iterator (Tcl_HashTable* table) :
		itsTable(table)
	 {
		itsEntry = Tcl_FirstHashEntry(itsTable, &itsSearch);
	 }

	 iterator (Tcl_HashTable* table, Tcl_HashEntry* entry) :
		itsTable(table),
		itsEntry(entry)
	 {
		if (itsEntry != 0) {
		  Tcl_HashEntry* entry = Tcl_FirstHashEntry(itsTable, &itsSearch);
		  while (entry != itsEntry && entry != 0) {
			 entry = Tcl_NextHashEntry(&itsSearch);
		  }
		}
	 }

  public:
	 friend class PtrList<T>;

	 iterator (const iterator& rhs) :
		itsSearch(rhs.itsSearch),
		itsEntry(rhs.itsEntry),
		itsTable(rhs.itsTable) {}

	 iterator& operator=(const iterator& rhs) 
	 {
		itsSearch = rhs.itsSearch;
		itsEntry = rhs.itsEntry;
		itsTable = rhs.itsTable;
	 }

	 bool operator== (const iterator& rhs) const 
		{ return (itsTable == rhs.itsTable) && (itsEntry == rhs.itsEntry); }
	 bool operator!= (const iterator& rhs) const
		{ return (itsTable != rhs.itsTable) || (itsEntry != rhs.itsEntry); }

	 T* operator* () const
	   { return static_cast<T*>(Tcl_GetHashValue(itsEntry)); } 

	 iterator& operator++ () { 
		itsEntry = Tcl_NextHashEntry(&itsSearch);
		while (itsEntry != 0 && Tcl_GetHashValue(itsEntry) == 0) {
		  itsEntry = Tcl_NextHashEntry(&itsSearch);
		}
		return *this;
	 }

	 iterator operator++ (int)
		{ iterator tmp = *this; ++*this; return tmp; }

	 int toInt() const 
	   { return reinterpret_cast<int>(Tcl_GetHashKey(itsTable, itsEntry)); }
  };

  class const_iterator {
  private:
	 Tcl_HashSearch itsSearch;
	 Tcl_HashEntry* itsEntry;
	 Tcl_HashTable* itsTable;
	 
	 const_iterator (Tcl_HashTable* table) :
		itsTable(table)
	 {
		itsEntry = Tcl_FirstHashEntry(itsTable, &itsSearch);
	 }

	 const_iterator (Tcl_HashTable* table, Tcl_HashEntry* entry) :
		itsTable(table),
		itsEntry(entry)
	 {
		if (itsEntry != 0) {
		  Tcl_HashEntry* entry = Tcl_FirstHashEntry(itsTable, &itsSearch);
		  while (entry != itsEntry && entry != 0) {
			 entry = Tcl_NextHashEntry(&itsSearch);
		  }
		}
	 }

  public:
	 friend class PtrList<T>;

	 const_iterator (const const_iterator& rhs) :
		itsSearch(rhs.itsSearch),
		itsEntry(rhs.itsEntry),
		itsTable(rhs.itsTable) {}

	 const_iterator& operator=(const const_iterator& rhs) 
	 {
		itsSearch = rhs.itsSearch;
		itsEntry = rhs.itsEntry;
		itsTable = rhs.itsTable;
	 }

	 bool operator== (const const_iterator& rhs) const 
		{ return (itsTable == rhs.itsTable) && (itsEntry == rhs.itsEntry); }
	 bool operator!= (const const_iterator& rhs) const
		{ return (itsTable != rhs.itsTable) || (itsEntry != rhs.itsEntry); }

	 T* operator* () const
	   { return static_cast<T*>(Tcl_GetHashValue(itsEntry)); } 

	 const_iterator& operator++ () { 
		itsEntry = Tcl_NextHashEntry(&itsSearch);
		while (itsEntry != 0 && Tcl_GetHashValue(itsEntry) == 0) {
		  itsEntry = Tcl_NextHashEntry(&itsSearch);
		}
		return *this;
	 }

	 const_iterator operator++ (int)
		{ const_iterator tmp = *this; ++*this; return tmp; }

	 int toInt() const 
	   { return reinterpret_cast<int>(Tcl_GetHashKey(itsTable, itsEntry)); }
  };

  iterator begin()             { return       iterator(itsHashTable); }
  const_iterator begin() const { return const_iterator(itsHashTable); }
  iterator end()               { return       iterator(itsHashTable, 0); }
  const_iterator end() const   { return const_iterator(itsHashTable, 0); }

  ///////////////
  // accessors //
  ///////////////

  int capacity() const { /* ? */ return 10000; }

  int count() const;

  // Returns true if 'id' is a valid index into a non-NULL T* in
  // the PtrList, given its current size.
  bool isValidId(int id) const {
	 Tcl_HashEntry* entry = Tcl_FindHashEntry(itsHashTable,
															reinterpret_cast<char*>(id));
	 return ( (entry != 0) && (Tcl_GetHashValue(entry) != 0) );
  }


  // Return the T* at the index given by 'id'.  There is no
  // range-check performed; this must be done by the client with
  // isValidId().
  T* getPtr (int id) const;

  template <class Iterator>
  void insertValidIds(Iterator inserter) const {
	 for (const_iterator itr = begin(); itr != end(); ++itr) {
		*inserter++ = itr.toInt();
	 }
  }

  // Puts a list of all valid (i.e. within-range and non-null) trial
  // ids into the vector<int> that is passed in by reference.
  void getValidIds(vector<int>& vec) const {
	 vec.clear();
	 insertValidIds(back_inserter(vec));
  }

  //////////////////
  // manipulators //
  //////////////////

  // Add ptr at the next available location, and return the index
  // where it was inserted. If necessary, the list will be expanded to
  // make room for the ptr. The PtrList now assumes control of the
  // memory management for the object *ptr.
  int insert(T* ptr) {
	 int new_site = itsFirstVacant;
	 insertAt(new_site, ptr);
	 return new_site;
  }

  // Add obj at index 'id', destroying any the object was previously
  // pointed to from that that location. The list will be expanded if
  // 'id' exceeds the size of the list. If id is < 0, the function
  // returns without effect.
  void insertAt(int id, T* ptr);

  // delete the Ptr at index 'i', and reset the T* to NULL
  void remove(int id);
  
  // delete all Ptr's held by the list, and reset the hash map
  void clear();

private:
  PtrList(const PtrList&);      // copy constructor not to be used
  PtrList& operator=(const PtrList&); // assignment operator not to be used
};

static const char vcid_ptrlist_h[] = "$Header$";
#endif // !PTRLIST_H_DEFINED
