///////////////////////////////////////////////////////////////////////
// objlist.h 
// Rob Peters
// Created: Nov-98
// written: Sun Apr 25 12:50:03 1999
// $Id$
//
// The ObjList class implements a list of GrObj pointers. The
// interface allows for management of a collection of GrObj's (i.e.,
// adding and removing objects), and for rendering individual GrObj's.
///////////////////////////////////////////////////////////////////////

#ifndef OBJLIST_H_DEFINED
#define OBJLIST_H_DEFINED

#ifndef PTRLIST_H_INCLUDED
#include "ptrlist.h"
#endif
#ifndef GROBJ_H_INCLUDED
#include "grobj.h"
#endif

///////////////////////////////////////////////////////////////////////
// ObjList class
///////////////////////////////////////////////////////////////////////

class ObjList : private PtrList<GrObj> {
private:
  typedef PtrList<GrObj> Base;
public:
  //////////////
  // creators //
  //////////////

  ObjList (int size) : Base(size) {}
  // construct a list of 'size' GrObj*'s, initialized to NULL

  virtual ~ObjList () {}

  // write/read the object's state from/to an output/input stream
  virtual void serialize(ostream &os, IOFlag flag) const
  { Base::serialize(os, flag); }
  virtual void deserialize(istream &is, IOFlag flag)
  { Base::deserialize(is, flag); }

  ///////////////
  // accessors //
  ///////////////

  int objCount() const { return Base::count(); }
  // returns the number of filled sites in the ObjList

  bool isValidObjid(int id) const { return Base::isValidId(id); }
  // returns true if 'id' is a valid index into a non-NULL GrObj* in
  // the ObjList, given its current size

  GrObj* getObj (int id) const { return Base::getPtr(id); }
  // both functions return the GrObj* at the index given by 'id'.
  // There is no range-checking--this must be done by the client with
  // isValidObjid().

  void getValidObjids(vector<int>& vec) const { Base::getValidIds(vec); }
  // Puts a list of all valid (i.e. within-range and non-null) objid's
  // into the vector<int> that is passed in by reference

  //////////////////
  // manipulators //
  //////////////////

  int addObj(GrObj *obj) { return Base::insert(obj); }
  // add obj at the next available location, and return the index
  // where it was inserted. If necessary, the list will be expanded to
  // make room for the obj

  void addObjAt(int id, GrObj *obj) { Base::insertAt(id, obj); }
  // add obj at index 'id', destroying any GrObj that previously
  // occupied that location. The list will be expanded if 'id' exceeds
  // the size of the list. If id is < 0, the function returns without
  // effect.

  void removeObj(int id) { Base::remove(id); }
  // delete the GrObj at index 'i', and reset the GrObj* to NULL

  void clearObjs() { Base::clear(); }
  // delete all GrObj's held by the list, and reset all GrObj*'s to NULL
};

static const char vcid_objlist_h[] = "$Header$";
#endif // !OBJLIST_H_DEFINED
