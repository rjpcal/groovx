///////////////////////////////////////////////////////////////////////
//
// ptrlist.h
// Rob Peters
// created: Fri Apr 23 00:35:31 1999
// written: Thu May 27 20:05:23 1999
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
public:
  //////////////
  // creators //
  //////////////

  PtrList (int size);
  // Construct a PtrList with an initial capacity of 'size'. All of
  // the contained T*'s will be initialized to NULL.

  virtual ~PtrList ();

  virtual void serialize(ostream &os, IOFlag flag) const;
  virtual void deserialize(istream &is, IOFlag flag);
  // These functions write/read the object's state from/to an
  // output/input stream. All objects that are contained by pointer in
  // the PtrList will be written and read. A PtrList that is written
  // and then re-read will have all of the same objects available at
  // the same indices as before the first serialize operation.

  virtual int charCount() const;

  ///////////////
  // accessors //
  ///////////////

  int capacity() const;
  // Returns the size of the internal array. The number returned also
  // refers to the one-past-the-end index into the PtrList.

  int count() const;
  // Returns the number of filled sites in the PtrList.

  bool isValidId(int id) const;
  // Returns true if 'id' is a valid index into a non-NULL T* in
  // the PtrList, given its current size.

  T* getPtr (int id) const { return itsVec[id]; }
  // Return the T* at the index given by 'id'.  There is no
  // range-check performed; this must be done by the client with
  // isValidId().

  void getValidIds(vector<int>& vec) const;
  // Puts a list of all valid (i.e. within-range and non-null) ptrid's
  // into the vector<int> that is passed in by reference.

  //////////////////
  // manipulators //
  //////////////////

  int insert(T* ptr);
  // Add ptr at the next available location, and return the index
  // where it was inserted. If necessary, the list will be expanded to
  // make room for the ptr. The PtrList now assumes control of the
  // memory management for the object *ptr.

  void insertAt(int id, T* ptr);
  // Add obj at index 'id', destroying any the object was previously
  // pointed to from that that location. The list will be expanded if
  // 'id' exceeds the size of the list. If id is < 0, the function
  // returns without effect.

  void remove(int id);
  // delete the Ptr at index 'i', and reset the T* to NULL

  void clear();
  // delete all Ptr's held by the list, and reset all T*'s to NULL

private:
  PtrList(const PtrList&);      // copy constructor not to be used
  PtrList& operator=(const PtrList&); // assignment operator not to be used

  int itsFirstVacant;           // smallest index of a vacant array site
  vector<T *> itsVec;		  // array of T*'s
};

static const char vcid_ptrlist_h[] = "$Header$";
#endif // !PTRLIST_H_DEFINED
