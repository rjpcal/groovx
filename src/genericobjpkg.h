///////////////////////////////////////////////////////////////////////
//
// listitempkg.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jul  7 13:17:04 1999
// written: Wed Jul  7 13:24:16 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef LISTITEMPKG_H_DEFINED
#define LISTITEMPKG_H_DEFINED

#ifndef TCLITEMPKG_H_DEFINED
#include "tclitempkg.h"
#endif

template <class C, class List>
class ListItemPkg : public CTclIoItemPkg<C> {
public:
  ListItemPkg(Tcl_Interp* interp, List& aList,
				  const char* name, const char* version) :
	 CTclIoItemPkg<C>(interp, name, version, 1),
	 itsList(aList) {}

  List& theList() { return itsList; }

  virtual C* getCItemFromId(int id) {
	 C* p = dynamic_cast<C*>(itsList.getCheckedPtr(id));
	 if ( p == NULL ) {
		throw TclError("object not of correct type");
	 }
	 return p;
  }

  virtual IO& getIoFromId(int id) {
	 return dynamic_cast<IO&>( *(getCItemFromId(id)) );
  }

private:
  List& itsList;
};


static const char vcid_listitempkg_h[] = "$Header$";
#endif // !LISTITEMPKG_H_DEFINED
