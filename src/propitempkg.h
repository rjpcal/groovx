///////////////////////////////////////////////////////////////////////
//
// propitempkg.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Oct  4 15:42:59 1999
// written: Mon Oct 30 11:13:44 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PROPITEMPKG_H_DEFINED
#define PROPITEMPKG_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(LISTITEMPKG_H_DEFINED)
#include "tcl/listitempkg.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IOFACTORY_H_DEFINED)
#include "io/iofactory.h"
#endif

namespace Tcl {

///////////////////////////////////////////////////////////////////////
/**
 *
 * Tcl::PropItemPkg
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class C>
class PropItemPkg : public ItemPkg<C> {
public:
  PropItemPkg(Tcl_Interp* interp, const char* name, const char* version) :
	 ItemPkg<C>(interp, name, version)
  {
	 CTclIoItemPkg<C>::declareAllProperties();
	 IO::IoFactory::theOne().registerCreatorFunc(&C::make);
  }
};

}

static const char vcid_propitempkg_h[] = "$Header$";
#endif // !PROPITEMPKG_H_DEFINED
