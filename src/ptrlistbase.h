///////////////////////////////////////////////////////////////////////
//
// voidptrlist.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Sat Nov 20 23:58:42 1999
// written: Sun Nov 21 00:18:26 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef VOIDPTRLIST_H_DEFINED
#define VOIDPTRLIST_H_DEFINED

#ifndef VECTOR_DEFINED
#include <vector>
#define VECTOR_DEFINED
#endif

#ifndef IO_H_DEFINED
#include "io.h"
#endif

#ifndef ERROR_H_DEFINED
#include "error.h"
#endif

/**
 *
 * InvalidIdError is an exception class that will be thrown from
 * VoidPtrList if an attempt to use an invalid id is made in a checked
 * function.
 *
 **/

class InvalidIdError : public ErrorWithMsg {
public:
  InvalidIdError(const string& msg="") : ErrorWithMsg(msg) {}
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * VoidPtrList provides a non-typesafe implementation of a container
 * that stores pointers and owns them. VoidPtrList delegates the
 * destruction of its contained objects to its subclasses by way of
 * destroyPtr().
 *
 * @memo 
 **/
///////////////////////////////////////////////////////////////////////

class VoidPtrList {
protected:
  VoidPtrList(int size);

public:
  virtual ~VoidPtrList();

  int capacity();
  int count();
  bool isValidId(int id) const;

  template <class Iterator>
  void insertValidIds(Iterator itr) const {
	 for (int i = 0; i < itsVec.size(); ++i) {
		if (isValidId(i)) 
		  *itr++ = i;
	 }
  }

  void remove(int id);

  void clear();

protected:
  void* getVoidPtr(int id) throw ();
  void* getCheckedVoidPtr(int id) throw (InvalidIdError);

  int insertVoidPtr(void* ptr);
  void insertVoidPtrAt(int id, void* ptr);

  virtual void afterInsertHook(int id, void* ptr);

  virtual void destroyPtr(void* ptr) = 0;

private:
  VoidPtrList(const VoidPtrList&);
  VoidPtrList& operator=(const VoidPtrList&);

  int itsFirstVacant;
  vector<void*> itsVec;
};

static const char vcid_voidptrlist_h[] = "$Header$";
#endif // !VOIDPTRLIST_H_DEFINED
