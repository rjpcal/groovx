///////////////////////////////////////////////////////////////////////
//
// listpkg.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Dec 15 17:27:51 1999
// written: Wed Oct 25 16:18:40 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef LISTPKG_CC_DEFINED
#define LISTPKG_CC_DEFINED

#include "tcl/listpkg.h"

#include "util/trace.h"

namespace Tcl {

//---------------------------------------------------------------------
//
// GetValidIdsCmd --
//
//---------------------------------------------------------------------

class GetValidIdsCmd : public TclItemCmd<IoPtrList> {
public:
  GetValidIdsCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<IoPtrList>(pkg, cmd_name, (char*)0, 1, 1) {}
protected:
  virtual void invoke() {
	 IoPtrList* theList = TclItemCmd<IoPtrList>::getItem();
	 for (IoPtrList::IdIterator
			  itr = theList->beginIds(),
			  end = theList->endIds();
			itr != end;
			++itr)
		{
		  lappendVal(*itr);
		}
  }
};

//---------------------------------------------------------------------
//
// IsValidIdCmd --
//
//---------------------------------------------------------------------

class IsValidIdCmd : public TclItemCmd<IoPtrList> {
public:
  IsValidIdCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<IoPtrList>(pkg, cmd_name, "item_id", 2, 2) {}
protected:
  virtual void invoke() {
	 int id = TclCmd::getIntFromArg(1);
	 IoPtrList* theList = TclItemCmd<IoPtrList>::getItem();
	 TclCmd::returnBool(theList->isValidId(id));
  }
};

//---------------------------------------------------------------------
//
// ListItemRemoveCmd --
//
//---------------------------------------------------------------------

class ListItemRemoveCmd : public TclItemCmd<IoPtrList> {
public:
  ListItemRemoveCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<IoPtrList>(pkg, cmd_name, "item_id(s)", 2, 2) {}
protected:
  virtual void invoke() {
	 IoPtrList* theList = TclItemCmd<IoPtrList>::getItem();

	 Tcl::ListIterator<int>
		itr = beginOfArg(1, (int*)0),
		stop = endOfArg(1, (int*)0);
	 while (itr != stop)
		{
		  theList->remove(*itr);
		  ++itr;
		}
  }
};

} // end namespace Tcl

Tcl::IoPtrListPkg::IoPtrListPkg(Tcl_Interp* interp, IoPtrList& aList,
										  const char* pkg_name, const char* version) :
  CTclIoItemPkg<IoPtrList>(interp, pkg_name, version, 0),
  itsList(aList)
{
DOTRACE("Tcl::IoPtrListPkg::IoPtrListPkg");
  declareGetter("count",
					 new CGetter<IoPtrList, int>(&IoPtrList::count));
  declareAction("reset",
					 new CAction<IoPtrList>(&IoPtrList::clear));
  addCommand( new GetValidIdsCmd(this, 
											TclPkg::makePkgCmdName("getValidIds")) );
  addCommand( new IsValidIdCmd(this,
										 TclPkg::makePkgCmdName("isValidId")) );
  addCommand( new ListItemRemoveCmd(this,
												TclPkg::makePkgCmdName("remove")) );
}	 

Tcl::IoPtrListPkg::~IoPtrListPkg() {}

IO::IoObject& Tcl::IoPtrListPkg::getIoFromId(int) {
DOTRACE("Tcl::IoPtrListPkg::getIoFromId");
  return dynamic_cast<IO::IoObject&>(itsList);
}

IoPtrList* Tcl::IoPtrListPkg::getCItemFromId(int) {
DOTRACE("Tcl::IoPtrListPkg::getCItemFromId");
  return &itsList;
}

static const char vcid_listpkg_cc[] = "$Header$";
#endif // !LISTPKG_CC_DEFINED
