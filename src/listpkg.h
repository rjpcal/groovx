///////////////////////////////////////////////////////////////////////
//
// listpkg.h
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 29 17:23:03 1999
// written: Mon Dec 11 15:42:34 2000
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
#include "io/ioptrlist.h"
#endif

namespace Tcl {

///////////////////////////////////////////////////////////////////////
/**
 *
 * Tcl::IoPtrListPkg
 *
 **/
///////////////////////////////////////////////////////////////////////

class IoPtrListPkg : public CTclItemPkg<IoPtrList>,
                     public IoFetcher
{
public:
  IoPtrListPkg(Tcl_Interp* interp,
					const char* pkg_name, const char* version);

  virtual ~IoPtrListPkg();

  virtual IO::IoObject& getIoFromId(int);
  virtual IoPtrList* getCItemFromId(int);

  virtual bool isMyType(IO::IoObject* obj) = 0;
};

} // end namespace Tcl

static const char vcid_listpkg_h[] = "$Header$";
#endif // !LISTPKG_H_DEFINED
