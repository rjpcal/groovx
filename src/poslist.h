///////////////////////////////////////////////////////////////////////
// poslist.h
// Rob Peters
// created: Fri Mar 12 17:13:53 1999
// written: Fri Mar 12 17:21:11 1999
///////////////////////////////////////////////////////////////////////

#ifndef POSLIST_H_DEFINED
#define POSLIST_H_DEFINED

#ifndef VECTOR_INCLUDED
#include <vector>
#define VECTOR_INCLUDED
#endif

#ifndef IO_H_INCLUDED
#include "io.h"
#endif
#ifndef POSITION_H_INCLUDED
#include "position.h"
#endif

///////////////////////////////////////////////////////////////////////
// PosList class
///////////////////////////////////////////////////////////////////////

class PosList {
public:
  //////////////
  // creators //
  //////////////

  PosList (int size);
  // construct a list of 'size' Position*'s, initialized to NULL

  ~PosList ();

  ///////////////
  // accessors //
  ///////////////

  int npos() const;
  // returns the number of (both filled and unfilled) sites in the PosList

  bool isValidPos(int id) const;
  // returns true if 'id' is a valid index into a non-NULL Position* in
  // the PosList, given its current size

  Position* getPos(int id) const { return itsPosVec[id]; }
  // both functions return the Position* at the index given by 'id'.
  // There is no range-checking--this must be done by the client with
  // olIsValidId().

  //////////////////
  // manipulators //
  //////////////////

  int addPos(Position *pos);
  // add pos at the next available location, and return the index
  // where it was inserted. If necessary, the list will be expanded to
  // make room for the pos

  void addPosAt(int id, Position *pos);
  // add pos at index 'id', destroying any Position that previously
  // occupied that location. The list will be expanded if 'id' exceeds
  // the size of the list. If id is < 0, the function returns without
  // effect.

  void removePos(int id);
  // delete the Position at index 'i', and reset the Position* to NULL

  void clearPos();
  // delete all Position's held by the list, and reset all Position*'s to NULL

private:
  PosList(const PosList&);		  // copy constructor not to be used
  PosList& operator=(const PosList&); // assignment operator not to be used

  typedef vector<Position *> PosVec;
  int itsFirstVacant;			  // smallest index of a vacant array site
  PosVec itsPosVec;				  // associative array from posid's to Position*'s
};

static const char vcid_poslist_h[] = "$Id$";
#endif // !POSLIST_H_DEFINED
