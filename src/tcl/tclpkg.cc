///////////////////////////////////////////////////////////////////////
//
// tclpkg.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 15 12:33:54 1999
// written: Wed Jul 18 11:25:50 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLPKG_CC_DEFINED
#define TCLPKG_CC_DEFINED

#ifdef ACC_COMPILER // aCC needs this to be happy with the header
#include "util/ref.h"
#endif

#include "tcl/tclpkg.h"

#include "util/objdb.h"
#include "util/object.h"

#include "tcl/tcllistobj.h"
#include "tcl/stringifycmd.h"

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

//---------------------------------------------------------------------
//
// ObjCaster
//
//---------------------------------------------------------------------

Tcl::ObjCaster::ObjCaster() {}

Tcl::ObjCaster::~ObjCaster() {}

bool Tcl::ObjCaster::isIdMyType(Util::UID uid)
{
  WeakRef<Util::Object> item(uid);
  return (item.isValid() && isMyType(item.get()));
}

namespace Tcl
{

  struct CountAllFunc
  {
    shared_ptr<ObjCaster> itsCaster;

    CountAllFunc(shared_ptr<ObjCaster> caster) : itsCaster(caster) {}

	 void operator()(Tcl::Context& ctx)
	 {
		ObjDb& theDb = ObjDb::theDb();
		int count = 0;
		for (ObjDb::PtrIterator
				 itr = theDb.beginPtrs(),
				 end = theDb.endPtrs();
			  itr != end;
			  ++itr)
		  {
			 if (itsCaster->isMyType(*itr))
				++count;
		  }
		ctx.setResult(count);
	 }
  };

  struct FindAllFunc
  {
    shared_ptr<ObjCaster> itsCaster;

    FindAllFunc(shared_ptr<ObjCaster> caster) : itsCaster(caster) {}

	 void operator()(Tcl::Context& ctx)
	 {
		ObjDb& theDb = ObjDb::theDb();

		Tcl::List result;

		for (ObjDb::PtrIterator
				 itr = theDb.beginPtrs(),
				 end = theDb.endPtrs();
			  itr != end;
			  ++itr)
		  {
			 if (itsCaster->isMyType(*itr))
				result.append(itr.getId());
		  }

		ctx.setResult(result);
	 }
  };

  struct RemoveAllFunc
  {
    shared_ptr<ObjCaster> itsCaster;

    RemoveAllFunc(shared_ptr<ObjCaster> caster) : itsCaster(caster) {}

	 void operator()(Tcl::Context& ctx)
	 {
		ObjDb& theDb = ObjDb::theDb();
		for (ObjDb::IdIterator
				 itr = theDb.beginIds(),
				 end = theDb.endIds();
			  itr != end;
			  /* increment done in loop body */)
		  {
			 if (itsCaster->isMyType(itr.getObject()) &&
				  itr.getObject()->isUnshared())
				{
				  int remove_me = *itr;
				  ++itr;
				  theDb.remove(remove_me);
				}
			 else
				{
				  ++itr;
				}
		  }
	 }
  };

  struct IsFunc
  {
    shared_ptr<ObjCaster> itsCaster;

    IsFunc(shared_ptr<ObjCaster> caster) : itsCaster(caster) {}

	 void operator()(Tcl::Context& ctx)
	 {
		Util::UID id = ctx.getValFromArg(1, TypeCue<Util::UID>());
		ctx.setResult(itsCaster->isIdMyType(id));
	 }
  };

}

///////////////////////////////////////////////////////////////////////
//
// Pkg member definitions
//
///////////////////////////////////////////////////////////////////////

Tcl::Pkg::Pkg(Tcl_Interp* interp, const char* name, const char* version) :
  Tcl::PkgBase(interp, name, version)
{}

Tcl::Pkg::~Pkg() {}

const char* Tcl::Pkg::actionUsage(const char* usage)
{
  return usage ? usage : "item_id(s)";
}

const char* Tcl::Pkg::getterUsage(const char* usage)
{
  return usage ? usage : "item_id(s)";
}

const char* Tcl::Pkg::setterUsage(const char* usage)
{
  return usage ? usage : "item_id(s) new_value(s)";
}

void Tcl::Pkg::addIoCommands()
{
DOTRACE("Tcl::Pkg::addIoCommands");
  addCommand( new StringifyCmd(interp(), makePkgCmdName("stringify")) );
  addCommand( new DestringifyCmd(interp(), makePkgCmdName("destringify")) );

  addCommand( new WriteCmd(interp(), makePkgCmdName("write")) );
  addCommand( new ReadCmd(interp(), makePkgCmdName("read")) );

  addCommand( new ASWSaveCmd(interp(), makePkgCmdName("save")) );
  addCommand( new ASRLoadCmd(interp(), makePkgCmdName("load")) );
}

void Tcl::Pkg::addGenericObjCmds(shared_ptr<Tcl::ObjCaster> caster)
{
  defVecRaw( IsFunc(caster), "is", "item_id(s)", 1 );
  defRaw( CountAllFunc(caster), "countAll", 0, 0 );
  defRaw( FindAllFunc(caster), "findAll", 0, 0 );
  defRaw( RemoveAllFunc(caster), "removeAll", 0, 0 );
}

static const char vcid_tclpkg_cc[] = "$Header$";
#endif // !TCLPKG_CC_DEFINED
