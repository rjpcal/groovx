///////////////////////////////////////////////////////////////////////
//
// rhlist.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jun  9 20:05:28 1999
// written: Thu Nov 11 14:59:31 1999
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

  /** This does the same as PtrList<ResponseHandler>::deserialize(),
		except that it ensures that the any new ResponseHandler's that
		are created have setInterp() properly called on them */
  virtual void deserialize(istream &is, IOFlag flag);

  /** This does the same as PtrList<ResponseHandler>::readFrom(),
		except that it ensures that the any new ResponseHandler's that
		are created have setInterp() properly called on them */
  virtual void readFrom(Reader* reader);

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
