///////////////////////////////////////////////////////////////////////
//
// iotcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Oct 30 10:00:39 2000
// written: Mon Oct 30 16:07:35 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOTCL_CC_DEFINED
#define IOTCL_CC_DEFINED

#include "tcl/listpkg.h"
#include "tcl/listitempkg.h"

#include "io/io.h"
#include "io/iomgr.h"
#include "io/ioptrlist.h"

#include "system/demangle.h"

#include <typeinfo>

#define NO_TRACE
#include "util/trace.h"

namespace Tcl {

//---------------------------------------------------------------------
//
// TypeCmd
//
//---------------------------------------------------------------------

class IoTypeCmd : public TclItemCmd<IO::IoObject> {
public:
  IoTypeCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<IO::IoObject>(pkg, cmd_name, "item_id", 2, 2) {}
protected:
  virtual void invoke() {
	 IO::IoObject* p = TclItemCmd<IO::IoObject>::getItem();
	 returnCstring(demangle_cstr(typeid(*p).name()));
  }
};

//---------------------------------------------------------------------
//
// RefCountCmd
//
//---------------------------------------------------------------------

class IoRefCountCmd : public TclItemCmd<IO::IoObject> {
public:
  IoRefCountCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<IO::IoObject>(pkg, cmd_name, "item_id", 2, 2) {}
protected:
  virtual void invoke() {
	 IO::IoObject* p = TclItemCmd<IO::IoObject>::getItem();
	 returnInt(p->refCount());
  }
};

//---------------------------------------------------------------------
//
// IncrRefCountCmd
//
//---------------------------------------------------------------------

class IoIncrRefCountCmd : public TclItemCmd<IO::IoObject> {
public:
  IoIncrRefCountCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<IO::IoObject>(pkg, cmd_name, "item_id", 2, 2) {}
protected:
  virtual void invoke() {
	 IO::IoObject* p = TclItemCmd<IO::IoObject>::getItem();
	 p->incrRefCount2();
  }
};

//---------------------------------------------------------------------
//
// DecrRefCountCmd
//
//---------------------------------------------------------------------

class IoDecrRefCountCmd : public TclItemCmd<IO::IoObject> {
public:
  IoDecrRefCountCmd(TclItemPkg* pkg, const char* cmd_name) :
	 TclItemCmd<IO::IoObject>(pkg, cmd_name, "item_id", 2, 2) {}
protected:
  virtual void invoke() {
	 IO::IoObject* p = TclItemCmd<IO::IoObject>::getItem();
	 p->decrRefCount2();
  }
};

//---------------------------------------------------------------------
//
// IoNewCmd
//
//---------------------------------------------------------------------

class IoNewCmd : public TclCmd {
public:
  IoNewCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name, "typename", 2, 2)
	 {}

protected:
  virtual void invoke() {
	 const char* type = getCstringFromArg(1);

	 IO::IoObject* obj = IO::IoMgr::newIO(type);

	 IdItem<IO::IoObject> item(obj, IdItem<IO::IoObject>::Insert());

	 returnInt(item.id());
  }
};

//---------------------------------------------------------------------
//
// IoDeleteCmd
//
//---------------------------------------------------------------------

class IoDeleteCmd : public TclCmd {
public:
  IoDeleteCmd(Tcl_Interp* interp, const char* cmd_name) :
	 TclCmd(interp, cmd_name, "item_id(s)", 2, 2)
	 {}

protected:
  virtual void invoke() {
	 Tcl::ListIterator<int>
		itr = beginOfArg(1, (int*)0),
		stop = endOfArg(1, (int*)0);
	 while (itr != stop)
		{
		  IoPtrList::theList().remove(*itr);
		  ++itr;
		}
  }
};

class IoItemPkg : public ItemPkg<IO::IoObject> {
public:
  IoItemPkg(Tcl_Interp* interp) :
	 ItemPkg<IO::IoObject>(interp, "IO", "$Revision$")
  {
	 addCommand( new IoTypeCmd(this, TclPkg::makePkgCmdName("type")) );
	 addCommand( new IoRefCountCmd(this, TclPkg::makePkgCmdName("refCount")));
	 addCommand( new IoIncrRefCountCmd(this,
                        TclPkg::makePkgCmdName("incrRefCount")));
	 addCommand( new IoDecrRefCountCmd(this,
								TclPkg::makePkgCmdName("decrRefCount")));

	 addCommand( new IoNewCmd(interp, TclPkg::makePkgCmdName("new")));
	 addCommand( new IoDeleteCmd(interp, TclPkg::makePkgCmdName("delete")));
  }
};

class IoListPkg : public PtrListPkg<IO::IoObject> {
public:
  IoListPkg(Tcl_Interp* interp) :
	 PtrListPkg<IO::IoObject>(interp, "IO", "$Revision$")
  {
	 declareCAction("clear", &IoPtrList::clear);
	 declareCAction("purge", &IoPtrList::purge);
  }
};

} // end namespace Tcl

extern "C"
int Io_Init(Tcl_Interp* interp) {
DOTRACE("Io_Init");

  Tcl::TclPkg* pkg1 = new Tcl::IoListPkg(interp);
  Tcl::TclPkg* pkg2 = new Tcl::IoItemPkg(interp);

  return pkg1->combineStatus(pkg2->initStatus());
}

static const char vcid_iotcl_cc[] = "$Header$";
#endif // !IOTCL_CC_DEFINED
