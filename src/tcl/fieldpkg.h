///////////////////////////////////////////////////////////////////////
//
// fieldpkg.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sun Nov 12 17:45:52 2000
// written: Fri Jan 18 16:06:56 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FIELDPKG_H_DEFINED
#define FIELDPKG_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(OBJFACTORY_H_DEFINED)
#include "util/objfactory.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLPKG_H_DEFINED)
#include "tcl/tclpkg.h"
#endif

class Field;
class FieldMap;

namespace Tcl
{

  class Pkg;

  void defField(Pkg* pkg, const Field& field);
  void defAllFields(Pkg* pkg, const FieldMap& fmap);

  template <class C>
  void defFieldContainer(Pkg* pkg)
  {
    Tcl::defGenericObjCmds<C>(pkg);

    Tcl::defAllFields(pkg, C::classFields());
  }

  template <class C>
  void defCreator(Pkg*)
  {
    Util::ObjFactory::theOne().registerCreatorFunc(&C::make);
  }
}

static const char vcid_fieldpkg_h[] = "$Header$";
#endif // !FIELDPKG_H_DEFINED
