///////////////////////////////////////////////////////////////////////
//
// fieldpkg.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sun Nov 12 17:45:52 2000
// written: Thu May 10 12:04:37 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FIELDPKG_H_DEFINED
#define FIELDPKG_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IO_H_DEFINED)
#include "io/io.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IOFACTORY_H_DEFINED)
#include "io/iofactory.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IOITEMPKG_H_DEFINED)
#include "tcl/ioitempkg.h"
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
class FieldCntrPkg : public IoItemPkg<C> {
public:
  FieldCntrPkg(Tcl_Interp* interp, const char* name, const char* version) :
	 IoItemPkg<C>(interp, name, version)
  {
	 declareAllFields(this, C::classFields());
	 IO::IoFactory::theOne().registerCreatorFunc(&C::make);
  }
};

} // end namespace Tcl

static const char vcid_fieldpkg_h[] = "$Header$";
#endif // !FIELDPKG_H_DEFINED
