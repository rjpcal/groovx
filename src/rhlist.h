///////////////////////////////////////////////////////////////////////
//
// rhlist.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jun  9 20:05:28 1999
// written: Wed Oct 11 14:31:49 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef RHLIST_H_DEFINED
#define RHLIST_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(PTRLIST_H_DEFINED)
#include "ptrlist.h"
#endif

class ResponseHandler;

///////////////////////////////////////////////////////////////////////
/**
 *
 * RhList is a singleton wrapper for \c PtrList<ResponseHandler>.
 *
 **/
///////////////////////////////////////////////////////////////////////

class RhList : public PtrList<ResponseHandler> {
private:
  typedef PtrList<ResponseHandler> Base;

  static RhList theInstance;

protected:
  /// Construct and reserve space for \a size objects.
  RhList(int size);

  /// Virtual destructor.
  virtual ~RhList();

public:
  /// Returns a reference to the singleton instance.
  static RhList& theRhList();

private:
  RhList(const RhList&);
  RhList& operator=(const RhList&);
};

static const char vcid_rhlist_h[] = "$Header$";
#endif // !RHLIST_H_DEFINED
