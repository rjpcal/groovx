///////////////////////////////////////////////////////////////////////
//
// tcllistpkg.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun 14 12:49:29 1999
// written: Mon Jun 14 13:03:39 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLLISTPKG_H_DEFINED
#define TCLLISTPKG_H_DEFINED

#include "tclpkg.h"

class IO;

class TclListPkg : public TclPkg {
public:
  TclListPkg(Tcl_Interp* interp, const char* name, const char* version);
  virtual IO& getList() = 0;
  virtual int getBufSize() = 0;

  virtual void reset() = 0;
  virtual int count() = 0;
};


static const char vcid_tcllistpkg_h[] = "$Header$";
#endif // !TCLLISTPKG_H_DEFINED
