///////////////////////////////////////////////////////////////////////
// objlist.h 
// Rob Peters
// Created: Nov-98
// written: Tue Mar 16 19:37:49 1999
// $Id$
//
// The ObjList class implements a list of GrObj pointers. The
// interface allows for management of a collection of GrObj's (i.e.,
// adding and removing objects), and for rendering individual GrObj's.
///////////////////////////////////////////////////////////////////////

#ifndef OBJLIST_H_DEFINED
#define OBJLIST_H_DEFINED

#ifndef VECTOR_INCLUDED
#include <vector>
#define VECTOR_INCLUDED
#endif

#ifndef IO_H_INCLUDED
#include "io.h"
#endif
#ifndef GROBJ_H_INCLUDED
#include "grobj.h"
#endif

///////////////////////////////////////////////////////////////////////
// ObjList class
///////////////////////////////////////////////////////////////////////

class ObjList : public virtual IO {
public:
  //////////////
  // creators //
  //////////////

  ObjList (int size);
  // construct a list of 'size' GrObj*'s, initialized to NULL

  ~ObjList ();

  // write/read the object's state from/to an output/input stream
  virtual IOResult serialize(ostream &os, IOFlag flag = NO_FLAGS) const;
  virtual IOResult deserialize(istream &is, IOFlag flag = NO_FLAGS);

  ///////////////
  // accessors //
  ///////////////

  int nobjs() const;
  // returns the number of filled sites in the ObjList

  bool isValidObjid(int id) const;
  // returns true if 'id' is a valid index into a non-NULL GrObj* in
  // the ObjList, given its current size

  GrObj* getObj (int id) const { return itsObjVec[id]; }
  // both functions return the GrObj* at the index given by 'id'.
  // There is no range-checking--this must be done by the client with
  // isValidObjid().

  void getValidObjids(vector<int>& vec) const;
  // Puts a list of all valid (i.e. within-range and non-null) objid's
  // into the vector<int> that is passed in by reference

  //////////////////
  // manipulators //
  //////////////////

  int addObj(GrObj *obj);
  // add obj at the next available location, and return the index
  // where it was inserted. If necessary, the list will be expanded to
  // make room for the obj

  void addObjAt(int id, GrObj *obj);
  // add obj at index 'id', destroying any GrObj that previously
  // occupied that location. The list will be expanded if 'id' exceeds
  // the size of the list. If id is < 0, the function returns without
  // effect.

  void removeObj(int id);
  // delete the GrObj at index 'i', and reset the GrObj* to NULL

  void clearObjs();
  // delete all GrObj's held by the list, and reset all GrObj*'s to NULL

  /////////////
  // actions //
  /////////////

  void drawObj(int id) const { getObj(id)->grAction(); }
  // call the action function for the GrObj at site 'id'

private:
  ObjList(const ObjList&);      // copy constructor not to be used
  ObjList& operator=(const ObjList&); // assignment operator not to be used

  typedef vector<GrObj *> ObjVec;
  int itsFirstVacant;           // smallest index of a vacant array site
  ObjVec itsObjVec;             // associative array from objid's to GrObj*'s
};

static const char vcid_objlist_h[] = "$Header$";
#endif // !OBJLIST_H_DEFINED
