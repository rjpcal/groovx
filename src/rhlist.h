///////////////////////////////////////////////////////////////////////
//
// rhlist.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jun  9 20:05:28 1999
// written: Wed Jun 30 14:22:50 1999
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

// Certain operations are overridden from PtrList in order to properly
// ensure that all ResponseHandler's stored on an RhList have their
// Tcl_Interp*'s properly maintained.

class RhList : public PtrList<ResponseHandler>, public virtual IO {
private:
  typedef PtrList<ResponseHandler> Base;
protected:
  RhList(int size);
public:
  static RhList& theRhList();

  virtual void deserialize(istream &is, IOFlag flag);

  //////////////////
  // manipulators //
  //////////////////

  virtual int insert(ResponseHandler* ptr);

  virtual void insertAt(int id, ResponseHandler* ptr);

  void setInterp(Tcl_Interp* interp);

private:
  Tcl_Interp* itsInterp;
};

static const char vcid_rhlist_h[] = "$Header$";
#endif // !RHLIST_H_DEFINED
