///////////////////////////////////////////////////////////////////////
//
// listpkg.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Dec 15 17:27:51 1999
// written: Wed Nov  1 17:41:26 2000
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

} // end namespace Tcl

Tcl::IoPtrListPkg::IoPtrListPkg(Tcl_Interp* interp,
										  const char* pkg_name, const char* version) :
  CTclItemPkg<IoPtrList>(interp, pkg_name, version, 0)
{
DOTRACE("Tcl::IoPtrListPkg::IoPtrListPkg");
  TclItemPkg::addIoCommands(this);

  addCommand( new ListItemCountCmd(this, 
											  TclPkg::makePkgCmdName("count")) );
  addCommand( new ListResetCmd(this, 
										 TclPkg::makePkgCmdName("reset")) );
  
  addCommand( new GetValidIdsCmd(this, 
											TclPkg::makePkgCmdName("getValidIds")) );
}

Tcl::IoPtrListPkg::~IoPtrListPkg() {}

IO::IoObject& Tcl::IoPtrListPkg::getIoFromId(int) {
DOTRACE("Tcl::IoPtrListPkg::getIoFromId");
  return dynamic_cast<IO::IoObject&>(IoPtrList::theList());
}

IoPtrList* Tcl::IoPtrListPkg::getCItemFromId(int) {
DOTRACE("Tcl::IoPtrListPkg::getCItemFromId");
  return &(IoPtrList::theList());
}

static const char vcid_listpkg_cc[] = "$Header$";
#endif // !LISTPKG_CC_DEFINED
