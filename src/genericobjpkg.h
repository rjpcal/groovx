///////////////////////////////////////////////////////////////////////
//
// listitempkg.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Jul  7 13:17:04 1999
// written: Wed Nov  1 17:42:45 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef LISTITEMPKG_H_DEFINED
#define LISTITEMPKG_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLITEMPKG_H_DEFINED)
#include "tcl/tclitempkg.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IDITEM_H_DEFINED)
#include "io/iditem.h"
#endif

namespace Tcl {

///////////////////////////////////////////////////////////////////////
/**
 *
 * IsCmd
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class C>
class IsCmd : public TclItemCmd<C> {
public:
  IsCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<C>(pkg, cmd_name, "item_id", 2, 2) {}
protected:
  virtual void invoke() {
	 int id = TclCmd::getIntFromArg(1);
	 MaybeIdItem<IO::IoObject> item(id);
	 returnBool(item.isValid() && dynamic_cast<C*>(item.get()) != 0);
  }
};

///////////////////////////////////////////////////////////////////////
/**
 *
 * ItemPkg
 *
 **/
///////////////////////////////////////////////////////////////////////

template <class C>
class ItemPkg : public CTclItemPkg<C>,
                public IoFetcher
{
public:
  ItemPkg(Tcl_Interp* interp, const char* name, const char* version) :
	 CTclItemPkg<C>(interp, name, version, 1)
  {
	 TclItemPkg::addIoCommands(this);
	 addCommand( new IsCmd<C>(this, TclPkg::makePkgCmdName("is")));
  }

  virtual C* getCItemFromId(int id) {
	 IdItem<C> item(id);
	 // will throw bad_cast if cast fails
	 C& p = dynamic_cast<C&>(*item);
	 return &p;
  }

  virtual IO::IoObject& getIoFromId(int id) {
	 return dynamic_cast<IO::IoObject&>( *(getCItemFromId(id)) );
  }
};

} // end namespace Tcl

static const char vcid_listitempkg_h[] = "$Header$";
#endif // !LISTITEMPKG_H_DEFINED
