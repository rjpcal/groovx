///////////////////////////////////////////////////////////////////////
//
// thlist.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jun  9 20:05:36 1999
// written: Wed Oct 25 16:52:34 2000
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

  static ThList theInstance;

protected:
  /// Default constructor makes an empty list.
  ThList();

  /// Virtual destructor.
  virtual ~ThList();

public:
  /// Returns a reference to the singleton instance.
  static ThList& theThList();
};

static const char vcid_thlist_h[] = "$Header$";
#endif // !THLIST_H_DEFINED
