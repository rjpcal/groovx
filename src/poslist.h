///////////////////////////////////////////////////////////////////////
// poslist.h
// Rob Peters
// created: Fri Mar 12 17:13:53 1999
// written: Sun Apr 25 12:50:03 1999
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef POSLIST_H_DEFINED
#define POSLIST_H_DEFINED

#ifndef PTRLIST_H_INCLUDED
#include "ptrlist.h"
#endif
#ifndef POSITION_H_INCLUDED
#include "position.h"
#endif

///////////////////////////////////////////////////////////////////////
// PosList class
///////////////////////////////////////////////////////////////////////

class PosList : private PtrList<Position> {
private:
  typedef PtrList<Position> Base;
public:
  //////////////
  // creators //
  //////////////

  PosList (int size) : Base(size) {}
  // construct a list of 'size' Position*'s, initialized to NULL

  virtual ~PosList () {}

  // write/read the object's state from/to an output/input stream
  virtual void serialize(ostream &os, IOFlag flag) const
  { Base::serialize(os, flag); }
  virtual void deserialize(istream &is, IOFlag flag)
  { Base::deserialize(is, flag); }

  ///////////////
  // accessors //
  ///////////////

  int posCount() const { return Base::count(); }
  // returns the number of (both filled and unfilled) sites in the PosList

  bool isValidPosid(int id) const { return Base::isValidId(id); }
  // returns true if 'id' is a valid index into a non-NULL Position* in
  // the PosList, given its current size

  Position* getPos(int id) const { return Base::getPtr(id); }
  // both functions return the Position* at the index given by 'id'.
  // There is no range-checking--this must be done by the client with
  // olIsValidId().

  //////////////////
  // manipulators //
  //////////////////

  int addPos(Position *pos) { return Base::insert(pos); }
  // add pos at the next available location, and return the index
  // where it was inserted. If necessary, the list will be expanded to
  // make room for the pos

  void addPosAt(int id, Position *pos) { Base::insertAt(id, pos); }
  // add pos at index 'id', destroying any Position that previously
  // occupied that location. The list will be expanded if 'id' exceeds
  // the size of the list. If id is < 0, the function returns without
  // effect.

  void removePos(int id) { Base::remove(id); }
  // delete the Position at index 'i', and reset the Position* to NULL

  void clearPos() { Base::clear(); }
  // delete all Position's held by the list, and reset all Position*'s to NULL
};

static const char vcid_poslist_h[] = "$Header$";
#endif // !POSLIST_H_DEFINED
