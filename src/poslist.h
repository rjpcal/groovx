///////////////////////////////////////////////////////////////////////
//
// poslist.h
// Rob Peters
// created: Fri Mar 12 17:13:53 1999
// written: Wed Oct 20 17:59:46 1999
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

class Position;

///////////////////////////////////////////////////////////////////////
//
// PosList class definition
//
///////////////////////////////////////////////////////////////////////

class PosList : public PtrList<Position>, public virtual IO {
private:
  typedef PtrList<Position> Base;

protected:
  // Construct a list of 'size' Position*'s, initialized to NULL
  PosList (int size) : Base(size) {}

public:
  // Returns a reference to the singleton instance of PosList
  static PosList& thePosList();
};

static const char vcid_poslist_h[] = "$Header$";
#endif // !POSLIST_H_DEFINED
