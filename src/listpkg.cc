///////////////////////////////////////////////////////////////////////
//
// listpkg.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Dec 15 17:27:51 1999
// written: Fri Oct 27 16:27:27 2000
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
// ListItemCountCmd --
//
//---------------------------------------------------------------------

class ListItemCountCmd : public TclItemCmd<IoPtrList> {
public:
  ListItemCountCmd(IoPtrListPkg* pkg, const char* cmd_name) :
	 TclItemCmd<IoPtrList>(pkg, cmd_name, (char*)0, 1, 1),
	 itsPkg(pkg)
  {}

protected:
  virtual void invoke() {
	 IoPtrList* theList = TclItemCmd<IoPtrList>::getItem();

	 int count = 0;

	 for (IoPtrList::IdIterator
			  itr = theList->beginIds(),
			  end = theList->endIds();
			itr != end;
			++itr)
		{
		  if (itsPkg->isMyType(itr.getObject()))
			 ++count;
		}

	 returnInt(count);
  }
private:
  IoPtrListPkg* itsPkg;
};

//---------------------------------------------------------------------
//
// ListResetCmd --
//
//---------------------------------------------------------------------

class ListResetCmd : public TclItemCmd<IoPtrList> {
public:
  ListResetCmd(IoPtrListPkg* pkg, const char* cmd_name) :
	 TclItemCmd<IoPtrList>(pkg, cmd_name, (char*)0, 1, 1),
	 itsPkg(pkg)
  {}

protected:
  virtual void invoke() {
	 IoPtrList* theList = TclItemCmd<IoPtrList>::getItem();
	 for (IoPtrList::IdIterator
			  itr = theList->beginIds(),
			  end = theList->endIds();
			itr != end;
			/* increment done in loop body */)
		{
		  if (itsPkg->isMyType(itr.getObject()) &&
				itr.getObject()->isUnshared())
			 {
				int remove_me = *itr;
				++itr;
				theList->remove(remove_me);
			 }
		  else
			 {
				++itr;
			 }
		}
  }
private:
  IoPtrListPkg* itsPkg;
};

//---------------------------------------------------------------------
//
// GetValidIdsCmd --
//
//---------------------------------------------------------------------

class GetValidIdsCmd : public TclItemCmd<IoPtrList> {
public:
  GetValidIdsCmd(IoPtrListPkg* pkg, const char* cmd_name) :
	 TclItemCmd<IoPtrList>(pkg, cmd_name, (char*)0, 1, 1),
	 itsPkg(pkg)
  {}

protected:
  virtual void invoke() {
	 IoPtrList* theList = TclItemCmd<IoPtrList>::getItem();
	 for (IoPtrList::IdIterator
			  itr = theList->beginIds(),
			  end = theList->endIds();
			itr != end;
			++itr)
		{
		  if (itsPkg->isMyType(itr.getObject()))
			 lappendVal(*itr);
		}
  }
private:
  IoPtrListPkg* itsPkg;
};

//---------------------------------------------------------------------
//
// IsValidIdCmd --
//
//---------------------------------------------------------------------

class IsValidIdCmd : public TclItemCmd<IoPtrList> {
public:
  IsValidIdCmd(IoPtrListPkg* pkg, const char* cmd_name) :
	 TclItemCmd<IoPtrList>(pkg, cmd_name, "item_id", 2, 2),
	 itsPkg(pkg)
  {}

protected:
  virtual void invoke() {
	 int id = TclCmd::getIntFromArg(1);
	 IoPtrList* theList = TclItemCmd<IoPtrList>::getItem();
	 if (theList->isValidId(id))
		{
		  IdItem<IO::IoObject> obj =
			 theList->template getCheckedIoPtr<IO::IoObject>(id);
		  returnBool(itsPkg->isMyType(obj.get()));
		}
	 else
		{
		  returnBool(false);
		}
  }
private:
  IoPtrListPkg* itsPkg;
};

//---------------------------------------------------------------------
//
// ListItemRemoveCmd --
//
//---------------------------------------------------------------------

class ListItemRemoveCmd : public TclItemCmd<IoPtrList> {
public:
  ListItemRemoveCmd(IoPtrListPkg* pkg, const char* cmd_name) :
	 TclItemCmd<IoPtrList>(pkg, cmd_name, "item_id(s)", 2, 2),
	 itsPkg(pkg)
  {}

protected:
  virtual void invoke() {
	 IoPtrList* theList = TclItemCmd<IoPtrList>::getItem();

	 Tcl::ListIterator<int>
		itr = beginOfArg(1, (int*)0),
		stop = endOfArg(1, (int*)0);
	 while (itr != stop)
		{
		  IdItem<IO::IoObject> obj =
			 theList->template getCheckedIoPtr<IO::IoObject>(*itr);
		  if (itsPkg->isMyType(obj.get()))
			 {
				// If it's just our IdItem + the list's reference:
				if (obj->refCount() == 2)
				  theList->release(*itr);
				else
				  theList->remove(*itr);
			 }
		  ++itr;
		}
  }
private:
  IoPtrListPkg* itsPkg;
};

} // end namespace Tcl

Tcl::IoPtrListPkg::IoPtrListPkg(Tcl_Interp* interp, IoPtrList& aList,
										  const char* pkg_name, const char* version) :
  CTclIoItemPkg<IoPtrList>(interp, pkg_name, version, 0),
  itsList(aList)
{
DOTRACE("Tcl::IoPtrListPkg::IoPtrListPkg");
  addCommand( new ListItemCountCmd(this, 
											  TclPkg::makePkgCmdName("count")) );
  addCommand( new ListResetCmd(this, 
										 TclPkg::makePkgCmdName("reset")) );
  
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
