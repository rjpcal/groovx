///////////////////////////////////////////////////////////////////////
//
// listpkg.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Jun 29 17:23:03 1999
// written: Tue Jun 29 17:40:22 1999
// $Id$
//
// This file defines a template TclPkg intended to be used with List's
// that implement the basic PtrList interface.
//
///////////////////////////////////////////////////////////////////////

#ifndef LISTPKG_H_DEFINED
#define LISTPKG_H_DEFINED

#ifndef VECTOR_DEFINED
#include <vector>
#define VECTOR_DEFINED
#endif

#ifndef TCLITEMPKG_H_DEFINED
#include "tclitempkg.h"
#endif

//---------------------------------------------------------------------
//
// GetValidIdsCmd --
//
//---------------------------------------------------------------------

template <class List>
class GetValidIdsCmd : public TclItemCmd<List> {
public:
  GetValidIdsCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<List>(pkg, cmd_name, NULL, 1, 1) {}
protected:
  virtual void invoke() {
	 vector<int> ids;
	 List* list = TclItemCmd<List>::getItem();
	 list->getValidIds(ids);
	 TclCmd::returnSequence(ids.begin(), ids.end());
  }
};

//---------------------------------------------------------------------
//
// IsValidIdCmd --
//
//---------------------------------------------------------------------

template <class List>
class IsValidIdCmd : public TclItemCmd<List> {
public:
  IsValidIdCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<List>(pkg, cmd_name, "item_id", 2, 2) {}
protected:
  virtual void invoke() {
	 int id = TclCmd::getIntFromArg(1);
	 List* list = TclItemCmd<List>::getItem();
	 TclCmd::returnBool(list->isValidId(id));
  }
};

//---------------------------------------------------------------------
//
// ListPkg --
//
//---------------------------------------------------------------------

template <class List>
class ListPkg : public CTclIoItemPkg<List> {
public:
  ListPkg(Tcl_Interp* interp, const char* pkg_name, const char* version) :
	 CTclIoItemPkg<List>(interp, pkg_name, version, 0)
  {
	 declareGetter("count", new CGetter<List, int>(&List::count));
	 declareAction("reset", new CAction<List>(&List::clear));
	 declareSetter("remove", new CSetter<List, int>(&List::remove), "item_id");
	 addCommand( new GetValidIdsCmd<List>(this, 
											 TclPkg::makePkgCmdName("getValidIds")) );
	 addCommand( new IsValidIdCmd<List>(this,
											 TclPkg::makePkgCmdName("isValidId")) );
  }

  virtual IO& getIoFromId(int) = 0;
  virtual List* getCItemFromId(int) = 0;
};

static const char vcid_listpkg_h[] = "$Header$";
#endif // !LISTPKG_H_DEFINED
