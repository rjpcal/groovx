///////////////////////////////////////////////////////////////////////
//
// fieldpkg.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sun Nov 12 17:45:52 2000
// written: Mon Jul 16 10:23:16 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FIELDPKG_H_DEFINED
#define FIELDPKG_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IO_H_DEFINED)
#include "io/io.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(OBJFACTORY_H_DEFINED)
#include "util/objfactory.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLITEMPKG_H_DEFINED)
#include "tcl/tclitempkg.h"
#endif

class FieldInfo;
class FieldMap;

namespace Tcl {

class TclItemPkg;

void declareField(TclItemPkg* pkg, const FieldInfo& finfo);
void declareAllFields(TclItemPkg* pkg, const FieldMap& fmap);

///////////////////////////////////////////////////////////////////////
/**
 *
 * FieldCntrPkg is a Tcl package that provides script-level access to
 * the fields of a FieldContainer subclass.
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class C>
class FieldCntrPkg : public TclItemPkg {
public:
  FieldCntrPkg(Tcl_Interp* interp, const char* name, const char* version) :
    TclItemPkg(interp, name, version)
  {
    Tcl::defGenericObjCmds<C>(this);

    declareAllFields(this, C::classFields());

    Util::ObjFactory::theOne().registerCreatorFunc(&C::make);
  }
};

} // end namespace Tcl

static const char vcid_fieldpkg_h[] = "$Header$";
#endif // !FIELDPKG_H_DEFINED
