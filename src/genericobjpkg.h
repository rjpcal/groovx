///////////////////////////////////////////////////////////////////////
//
// listitempkg.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jul  7 13:17:04 1999
// written: Wed Jul  7 14:06:01 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef LISTITEMPKG_H_DEFINED
#define LISTITEMPKG_H_DEFINED

#ifndef TCLITEMPKG_H_DEFINED
#include "tclitempkg.h"
#endif

///////////////////////////////////////////////////////////////////////
//
// AbstractListItemPkg --
//
///////////////////////////////////////////////////////////////////////

template <class C, class List>
class AbstractListItemPkg : public CTclIoItemPkg<C> {
public:
  AbstractListItemPkg(Tcl_Interp* interp, List& aList,
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

///////////////////////////////////////////////////////////////////////
//
// ListItemPkg (for concrete list items) --
//
///////////////////////////////////////////////////////////////////////

template <class C, class List>
class ListItemPkg : public AbstractListItemPkg<C, List> {
public:
  ListItemPkg(Tcl_Interp* interp, List& aList,
				  const char* name, const char* version,
				  const char* creator_cmd_name = 0);
};

template <class C, class List>
class DefaultCreatorCmd : public TclCmd {
public:
  DefaultCreatorCmd(ListItemPkg<C, List>* pkg, const char* cmd_name) :
	 TclCmd(pkg->interp(), cmd_name, NULL, 1, 1),
	 itsPkg(pkg) {}
protected:
  virtual void invoke() {
	 C* p = new C;
	 int id = itsPkg->theList().insert(p);
	 returnInt(id);
  }
private:
  ListItemPkg<C, List>* itsPkg;
};

template <class C, class List>
ListItemPkg::ListItemPkg(Tcl_Interp* interp, List& aList,
								 const char* name, const char* version,
								 const char* creator_cmd_name) :
  AbstractListItemPkg<C, List>(interp, aList, name, version)
{
  addCommand( new DefaultCreatorCmd<C, List>(
						  this, TclPkg::makePkgCmdName(
									 creator_cmd_name ? creator_cmd_name : name)) );
}

static const char vcid_listitempkg_h[] = "$Header$";
#endif // !LISTITEMPKG_H_DEFINED
