///////////////////////////////////////////////////////////////////////
//
// listpkg.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Dec 15 17:27:51 1999
// written: Mon Dec 11 15:34:17 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef LISTPKG_CC_DEFINED
#define LISTPKG_CC_DEFINED

#include "tcl/listpkg.h"

#include "util/trace.h"

namespace Tcl {

} // end namespace Tcl

Tcl::IoPtrListPkg::IoPtrListPkg(Tcl_Interp* interp,
										  const char* pkg_name, const char* version) :
  CTclItemPkg<IoPtrList>(interp, pkg_name, version, 0)
{
DOTRACE("Tcl::IoPtrListPkg::IoPtrListPkg");
}

Tcl::IoPtrListPkg::~IoPtrListPkg() {}

IO::IoObject& Tcl::IoPtrListPkg::getIoFromId(int) {
DOTRACE("Tcl::IoPtrListPkg::getIoFromId");
  return dynamic_cast<IO::IoObject&>(IoPtrList::theList());
}

IoPtrList* Tcl::IoPtrListPkg::getCItemFromId(int) {
DOTRACE("Tcl::IoPtrListPkg::getCItemFromId");
  return &(IoPtrList::theList());
}

static const char vcid_listpkg_cc[] = "$Header$";
#endif // !LISTPKG_CC_DEFINED
