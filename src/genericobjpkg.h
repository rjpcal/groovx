///////////////////////////////////////////////////////////////////////
//
// listitempkg.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jul  7 13:17:04 1999
// written: Tue Oct 24 15:29:45 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef LISTITEMPKG_H_DEFINED
#define LISTITEMPKG_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(DEMANGLE_H_DEFINED)
#include "system/demangle.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLITEMPKG_H_DEFINED)
#include "tcl/tclitempkg.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TYPEINFO_DEFINED)
#include <typeinfo>
#define TYPEINFO_DEFINED
#endif

namespace Tcl {

///////////////////////////////////////////////////////////////////////
/**
 *
 * TypeCmd
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class C>
class TypeCmd : public TclItemCmd<C> {
public:
  TypeCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<C>(pkg, cmd_name, "item_id", 2, 2) {}
protected:
  virtual void invoke() {
	 C* p = TclItemCmd<C>::getItem();
	 returnCstring(demangle_cstr(typeid(*p).name()));
  }
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * RefCountCmd
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class C>
class RefCountCmd : public TclItemCmd<C> {
public:
  RefCountCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<C>(pkg, cmd_name, "item_id", 2, 2) {}
protected:
  virtual void invoke() {
	 C* p = TclItemCmd<C>::getItem();
	 returnInt(p->refCount());
  }
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * AbstractListItemPkg
 *
 **/
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
	 addCommand( new RefCountCmd<C>(this, TclPkg::makePkgCmdName("refCount")));
  }

  List& theList() { return itsList; }

  virtual C* getCItemFromId(int id) {
	 typename List::SharedPtr item = itsList.getCheckedPtr(id);
	 // will throw bad_cast if cast fails
	 C& p = dynamic_cast<C&>(*item);
	 return &p;
  }

  virtual IO::IoObject& getIoFromId(int id) {
	 return dynamic_cast<IO::IoObject&>( *(getCItemFromId(id)) );
  }

private:
  List& itsList;
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * ListItemPkg is a subclass of AbstractListItemPkg for concrete list
 * items.
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class C, class List>
class ListItemPkg : public AbstractListItemPkg<C, List> {
public:
  ListItemPkg(Tcl_Interp* interp, List& aList,
				  const char* name, const char* version,
				  const char* creator_cmd_name = 0);
};

/**
 *
 * DefaultCreatorCmd
 *
 **/

template <class C, class List>
class DefaultCreatorCmd : public TclCmd {
public:
  DefaultCreatorCmd(ListItemPkg<C, List>* pkg, const char* cmd_name) :
	 TclCmd(pkg->interp(), cmd_name, (char*) 0 /* usage */, 1, 1),
	 itsPkg(pkg) {}
protected:
  virtual void invoke() {
	 C* newObject = C::make();
	 typedef typename List::SharedPtr ItemType;
	 ItemType item(newObject, ItemType::INSERT);
	 returnInt(item.id());
  }
private:
  DefaultCreatorCmd(const DefaultCreatorCmd&);
  DefaultCreatorCmd& operator=(const DefaultCreatorCmd&);

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
