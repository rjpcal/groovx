///////////////////////////////////////////////////////////////////////
//
// poslist.h
// Rob Peters
// created: Fri Mar 12 17:13:53 1999
// written: Wed Oct 25 16:50:50 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef POSLIST_H_DEFINED
#define POSLIST_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(PTRLIST_H_DEFINED)
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

  static PosList theInstance;

protected:
  /// Default constructor makes an empty list.
  PosList();

  /// Virtual destructor.
  virtual ~PosList();

public:
  /// Returns a reference to the singleton instance.
  static PosList& thePosList();
};

static const char vcid_poslist_h[] = "$Header$";
#endif // !POSLIST_H_DEFINED
