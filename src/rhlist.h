///////////////////////////////////////////////////////////////////////
//
// rhlist.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jun  9 20:05:28 1999
// written: Mon May 15 19:31:35 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef RHLIST_H_DEFINED
#define RHLIST_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(PTRLIST_H_DEFINED)
#include "ptrlist.h"
#endif

class ResponseHandler;
struct Tcl_Interp;

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

public:
  /// Returns a reference to the singleton instance.
  static RhList& theRhList();

  /** Reimplemented from \c VoidPtrList to ensure that all owned
      ResposneHandler's have their \c Tcl_Interp* set properly. */
  virtual void afterInsertHook(int id, void* ptr);

  /** Set the \c Tcl_Interp* for the \c RhList, which it passes on to
      all \c ResponseHandler's that it owns. */
  void setInterp(Tcl_Interp* interp);

private:
  RhList(const RhList&);
  RhList& operator=(const RhList&);

  Tcl_Interp* itsInterp;
};

static const char vcid_rhlist_h[] = "$Header$";
#endif // !RHLIST_H_DEFINED
