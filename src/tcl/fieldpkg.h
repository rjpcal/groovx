///////////////////////////////////////////////////////////////////////
//
// fieldpkg.h
//
// Copyright (c) 2000-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Sun Nov 12 17:45:52 2000
// written: Wed Mar 19 12:45:46 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FIELDPKG_H_DEFINED
#define FIELDPKG_H_DEFINED

#include "tcl/objpkg.h"
#include "tcl/tclpkg.h"

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
}

static const char vcid_fieldpkg_h[] = "$Header$";
#endif // !FIELDPKG_H_DEFINED
