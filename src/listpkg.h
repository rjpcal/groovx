///////////////////////////////////////////////////////////////////////
//
// listpkg.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Jun 29 17:23:03 1999
// written: Thu Mar 30 08:29:17 2000
// $Id$
//
// This file defines a TclPkg to be used with IoPtrList's.
//
///////////////////////////////////////////////////////////////////////

#ifndef LISTPKG_H_DEFINED
#define LISTPKG_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLITEMPKG_H_DEFINED)
#include "tcl/tclitempkg.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IOPTRLIST_H_DEFINED)
#include "ioptrlist.h"
#endif

namespace Tcl {
  class IoPtrListPkg;
}

//---------------------------------------------------------------------
//
// Tcl::IoPtrListPkg --
//
//---------------------------------------------------------------------

class Tcl::IoPtrListPkg : public Tcl::CTclIoItemPkg<IoPtrList> {
public:
  IoPtrListPkg(Tcl_Interp* interp, IoPtrList& aList,
					const char* pkg_name, const char* version);

  virtual IO::IoObject& getIoFromId(int);
  virtual IoPtrList* getCItemFromId(int);

private:
  IoPtrList& itsList;
};

static const char vcid_listpkg_h[] = "$Header$";
#endif // !LISTPKG_H_DEFINED
