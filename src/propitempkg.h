///////////////////////////////////////////////////////////////////////
//
// propitempkg.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Oct  4 15:42:59 1999
// written: Fri Oct 20 17:59:59 2000
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
 * Tcl::PropertyListItemPkg
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class C, class List>
class PropertyListItemPkg : public ListItemPkg<C, List> {
public:
  PropertyListItemPkg(Tcl_Interp* interp, List& aList,
							 const char* name, const char* version) :
	 ListItemPkg<C, List>(interp, aList, name, version)
  {
	 CTclIoItemPkg<C>::declareAllProperties();
	 IO::IoFactory::theOne().registerCreatorFunc(&C::make);
  }
};

}

static const char vcid_propitempkg_h[] = "$Header$";
#endif // !PROPITEMPKG_H_DEFINED
