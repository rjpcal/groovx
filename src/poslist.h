///////////////////////////////////////////////////////////////////////
//
// poslist.h
// Rob Peters
// created: Fri Mar 12 17:13:53 1999
// written: Wed Mar 22 16:47:03 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef POSLIST_H_DEFINED
#define POSLIST_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(PTRLIST_H)
#include "ptrlist.h"
#endif

class Position;

///////////////////////////////////////////////////////////////////////
/**
 *
 * PosList is a singleton wrapper for \c PtrList<Position>.
 *
 **/
///////////////////////////////////////////////////////////////////////

class PosList : public PtrList<Position> {
private:
  typedef PtrList<Position> Base;

protected:
  /// Construct and reserve space for \a size objects.
  PosList(int size) : Base(size) {}

public:
  /// Returns a reference to the singleton instance.
  static PosList& thePosList();
};

static const char vcid_poslist_h[] = "$Header$";
#endif // !POSLIST_H_DEFINED
