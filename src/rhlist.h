///////////////////////////////////////////////////////////////////////
//
// rhlist.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jun  9 20:05:28 1999
// written: Sun Nov 21 03:13:21 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef RHLIST_H_DEFINED
#define RHLIST_H_DEFINED

#ifndef PTRLIST_H_DEFINED
#include "ptrlist.h"
#endif

class ResponseHandler;
struct Tcl_Interp;

/// A singleton wrapper of PtrList<ResponseHandler>.
class RhList : public PtrList<ResponseHandler>, public virtual IO {
private:
  typedef PtrList<ResponseHandler> Base;
protected:
  ///
  RhList(int size);
public:
  ///
  static RhList& theRhList();

  /** Ensures that all owned ResposneHandler's have their Tcl_Interp*
      set properly. */
  virtual void afterInsertHook(int id, void* ptr);

  /** Set the Tcl_Interp* for the RhList, which it passes on to all
      ResponseHandler's that it owns. */
  void setInterp(Tcl_Interp* interp);

private:
  Tcl_Interp* itsInterp;
};

static const char vcid_rhlist_h[] = "$Header$";
#endif // !RHLIST_H_DEFINED
