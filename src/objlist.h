///////////////////////////////////////////////////////////////////////
//
// objlist.h 
// Rob Peters
// Created: Nov-98
// written: Fri Jun 11 21:53:00 1999
// $Id$
//
//
// This file defines ObjList, a singleton class whose instance is a
// global repository of GrObj pointers. It allows GrObj's to be
// uniquely identified by an objid from anywhere in the
// application. In addition, the entire collection can be written to
// or read from a stream. The singleton instance is retrieved with the
// function theObjList().
//
///////////////////////////////////////////////////////////////////////

#ifndef OBJLIST_H_DEFINED
#define OBJLIST_H_DEFINED

#ifndef PTRLIST_H_DEFINED
#include "ptrlist.h"
#endif

#include "id.h"

class GrObj;

///////////////////////////////////////////////////////////////////////
//
// ObjList class definition
//
///////////////////////////////////////////////////////////////////////

class ObjList : private PtrList<GrObj>, public virtual IO {
private:
  typedef PtrList<GrObj> Base;

  //////////////
  // creators //
  //////////////

protected:
  // Construct a list of 'size' GrObj*'s, initialized to NULL
  ObjList (int size) : Base(size) {}

public:
  // Returns a reference to the singleton instance of ObjList
  static ObjList& theObjList();

  // This function should remain out of line so that the vtable is
  // created correctly
  virtual ~ObjList ();

  // write/read the object's state from/to an output/input stream
  virtual void serialize(ostream &os, IOFlag flag) const
  { Base::serialize(os, flag); }
  virtual void deserialize(istream &is, IOFlag flag)
  { Base::deserialize(is, flag); }
  virtual int charCount() const
  { return Base::charCount(); }

  ///////////////
  // accessors //
  ///////////////

  int capacity() const { return Base::capacity(); }

  // returns the number of filled sites in the ObjList
  int objCount() const { return Base::count(); }

  // returns true if 'objid' is a valid index into a non-NULL GrObj* in
  // the ObjList, given its current size
  bool isValidObjid(ObjId objid) const { return Base::isValidId(objid.toInt()); }

  // both functions return the GrObj* at the index given by 'objid'.
  // There is no range-checking--this must be done by the client with
  // isValidObjid().
  GrObj* getObj (ObjId objid) const { return Base::getPtr(objid.toInt()); }

  // Puts a list of all valid (i.e. within-range and non-null) objid's
  // into the vector<ObjId> that is passed in by reference
  void getValidObjids(vector<int>& vec) const { Base::getValidIds(vec); }

  //////////////////
  // manipulators //
  //////////////////

  // add obj at the next available location, and return the index
  // where it was inserted. If necessary, the list will be expanded to
  // make room for the obj
  ObjId addObj(GrObj* obj) { return ObjId(Base::insert(obj)); }

  // add obj at index 'objid', destroying any GrObj that previously
  // occupied that location. The list will be expanded if 'objid' exceeds
  // the size of the list. If objid is < 0, the function returns without
  // effect.
  void addObjAt(ObjId objid, GrObj* obj) { Base::insertAt(objid.toInt(), obj); }

  // delete the GrObj at index 'i', and reset the GrObj* to NULL
  void removeObj(ObjId objid) { Base::remove(objid.toInt()); }

  // delete all GrObj's held by the list, and reset all GrObj*'s to
  // NULL
  void clearObjs() { Base::clear(); }
};

static const char vcid_objlist_h[] = "$Header$";
#endif // !OBJLIST_H_DEFINED
