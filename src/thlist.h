///////////////////////////////////////////////////////////////////////
//
// thlist.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jun  9 20:05:36 1999
// written: Mon Jun 21 15:38:51 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef THLIST_H_DEFINED
#define THLIST_H_DEFINED

#ifndef PTRLIST_H_DEFINED
#include "ptrlist.h"
#endif

class TimingHdlr;

class ThList : public PtrList<TimingHdlr>, public virtual IO {
private:
  typedef PtrList<TimingHdlr> Base;
protected:
  ThList(int size);
public:
  static ThList& theThList();

  virtual ~ThList();
};

static const char vcid_thlist_h[] = "$Header$";
#endif // !THLIST_H_DEFINED
