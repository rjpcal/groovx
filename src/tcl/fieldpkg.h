///////////////////////////////////////////////////////////////////////
//
// fieldpkg.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sun Nov 12 17:45:52 2000
// written: Wed Jul 18 09:55:19 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FIELDPKG_H_DEFINED
#define FIELDPKG_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(OBJFACTORY_H_DEFINED)
#include "util/objfactory.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLITEMPKG_H_DEFINED)
#include "tcl/tclitempkg.h"
#endif

class FieldInfo;
class FieldMap;

namespace Tcl
{

  class TclItemPkg;

  void defField(TclItemPkg* pkg, const FieldInfo& finfo);
  void defAllFields(TclItemPkg* pkg, const FieldMap& fmap);

  template <class C>
  void defFieldContainer(TclItemPkg* pkg)
  {
    Tcl::defGenericObjCmds<C>(pkg);

    Tcl::defAllFields(pkg, C::classFields());

    Util::ObjFactory::theOne().registerCreatorFunc(&C::make);
  }

}

static const char vcid_fieldpkg_h[] = "$Header$";
#endif // !FIELDPKG_H_DEFINED
