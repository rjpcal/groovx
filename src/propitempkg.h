///////////////////////////////////////////////////////////////////////
//
// propitempkg.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Oct  4 15:42:59 1999
// written: Wed Mar 15 11:11:24 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PROPITEMPKG_H_DEFINED
#define PROPITEMPKG_H_DEFINED

#ifndef LISTITEMPKG_H_DEFINED
#include "tcl/listitempkg.h"
#endif

#ifndef IOFACTORY_H_DEFINED
#include "iofactory.h"
#endif

namespace Tcl {

template <class C, class List>
class PropertyListItemPkg : public ListItemPkg<C, List> {
public:
  PropertyListItemPkg(Tcl_Interp* interp, List& aList,
							 const char* name, const char* version) :
	 ListItemPkg<C, List>(interp, aList, name, version)
  {
	 CTclIoItemPkg<C>::declareAllProperties();
	 FactoryRegistrar<IO, C>::registerWith(IoFactory::theOne());
  }
};

}

static const char vcid_propitempkg_h[] = "$Header$";
#endif // !PROPITEMPKG_H_DEFINED
