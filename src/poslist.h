///////////////////////////////////////////////////////////////////////
//
// poslist.h
// Rob Peters
// created: Fri Mar 12 17:13:53 1999
// written: Thu May 27 11:35:26 1999
// $Id$
//
// This file defines PosList, a singleton class whose instance is a
// global repository of Position pointers. It allows Position's to be
// uniquely identified by an posid from anywhere in the
// application. In addition, the entire collection can be written to
// or read from a stream. The singleton instance is retrieved with the
// function thePosList().
//
///////////////////////////////////////////////////////////////////////

#ifndef POSLIST_H_DEFINED
#define POSLIST_H_DEFINED

#ifndef PTRLIST_H_DEFINED
#include "ptrlist.h"
#endif
#ifndef POSITION_H_DEFINED
#include "position.h"
#endif

///////////////////////////////////////////////////////////////////////
//
// PosList class definition
//
///////////////////////////////////////////////////////////////////////

class PosList : private PtrList<Position> {
private:
  typedef PtrList<Position> Base;

  //////////////
  // creators //
  //////////////

protected:
  // Construct a list of 'size' Position*'s, initialized to NULL
  PosList (int size) : Base(size) {}

public:
  // Returns a reference to the singleton instance of PosList
  static PosList& thePosList();

  // This function should remain out of line so that the vtable is
  // created uniquely
  virtual ~PosList ();

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

  // returns the number of (both filled and unfilled) sites in the PosList
  int posCount() const { return Base::count(); }

  // returns true if 'id' is a valid index into a non-NULL Position* in
  // the PosList, given its current size
  bool isValidPosid(int id) const { return Base::isValidId(id); }

  // both functions return the Position* at the index given by 'id'.
  // There is no range-checking--this must be done by the client with
  // olIsValidId().
  Position* getPos(int id) const { return Base::getPtr(id); }

  //////////////////
  // manipulators //
  //////////////////

  // add pos at the next available location, and return the index
  // where it was inserted. If necessary, the list will be expanded to
  // make room for the pos
  int addPos(Position* pos) { return Base::insert(pos); }

  // add pos at index 'id', destroying any Position that previously
  // occupied that location. The list will be expanded if 'id' exceeds
  // the size of the list. If id is < 0, the function returns without
  // effect.
  void addPosAt(int id, Position* pos) { Base::insertAt(id, pos); }

  // delete the Position at index 'i', and reset the Position* to NULL
  void removePos(int id) { Base::remove(id); }

  // delete all Position's held by the list, and reset all Position*'s
  // to NULL
  void clearPos() { Base::clear(); }
};

static const char vcid_poslist_h[] = "$Header$";
#endif // !POSLIST_H_DEFINED
