///////////////////////////////////////////////////////////////////////
//
// propitempkg.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Oct  4 15:42:59 1999
// written: Tue Dec  7 18:34:09 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PROPITEMPKG_H_DEFINED
#define PROPITEMPKG_H_DEFINED

#ifndef LISTITEMPKG_H_DEFINED
#include "listitempkg.h"
#endif

#ifndef IOMGR_H_DEFINED
#include "iomgr.h"
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
