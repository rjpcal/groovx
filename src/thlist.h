///////////////////////////////////////////////////////////////////////
//
// thlist.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jun  9 20:05:36 1999
// written: Wed Mar 29 14:07:34 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef THLIST_H_DEFINED
#define THLIST_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(PTRLIST_H_DEFINED)
#include "ptrlist.h"
#endif

class TimingHdlr;

///////////////////////////////////////////////////////////////////////
/**
 *
 * ThList is a singleton wrapper for \c PtrList<TimingHdlr>.
 *
 **/
///////////////////////////////////////////////////////////////////////

class ThList : public PtrList<TimingHdlr> {
private:
  typedef PtrList<TimingHdlr> Base;

protected:
  /// Construct and reserve space for \a size objects.
  ThList(int size);

public:
  /// Returns a reference to the singleton instance.
  static ThList& theThList();
};

static const char vcid_thlist_h[] = "$Header$";
#endif // !THLIST_H_DEFINED
