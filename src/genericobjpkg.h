///////////////////////////////////////////////////////////////////////
//
// listitempkg.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jul  7 13:17:04 1999
// written: Tue Feb  1 18:06:37 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef LISTITEMPKG_H_DEFINED
#define LISTITEMPKG_H_DEFINED

#ifndef TYPEINFO_DEFINED
#include <typeinfo>
#define TYPEINFO_DEFINED
#endif

#ifndef TCLITEMPKG_H_DEFINED
#include "tclitempkg.h"
#endif

#ifndef DEMANGLE_H_DEFINED
#include "demangle.h"
#endif

namespace Tcl {

///////////////////////////////////////////////////////////////////////
//
// TypeCmd --
//
///////////////////////////////////////////////////////////////////////

template <class C>
class TypeCmd : public TclItemCmd<C> {
public:
  TypeCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<C>(pkg, cmd_name, "item_id", 2, 2) {}
protected:
  virtual void invoke() {
	 C* p = TclItemCmd<C>::getItem();
	 returnCstring(demangle(typeid(*p).name()).c_str());
  }
};

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
	 itsList(aList)
  {
	 addCommand( new TypeCmd<C>(this, TclPkg::makePkgCmdName("type")) );
  }

  List& theList() { return itsList; }

  virtual C* getCItemFromId(int id) {
	 // will throw bad_cast if cast fails
	 C& p = dynamic_cast<C&>(*(itsList.getCheckedPtr(id)));
	 return &p;
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
	 int id = itsPkg->theList().insert(List::Ptr(p));
	 returnInt(id);
  }
private:
  ListItemPkg<C, List>* itsPkg;
};

template <class C, class List>
ListItemPkg<C, List>::ListItemPkg(Tcl_Interp* interp, List& aList,
								 const char* name, const char* version,
								 const char* creator_cmd_name) :
  AbstractListItemPkg<C, List>(interp, aList, name, version)
{
  addCommand( new DefaultCreatorCmd<C, List>(
						  this, TclPkg::makePkgCmdName(
									 creator_cmd_name ? creator_cmd_name : name)) );
}

} // end namespace Tcl

static const char vcid_listitempkg_h[] = "$Header$";
#endif // !LISTITEMPKG_H_DEFINED
