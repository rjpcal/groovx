///////////////////////////////////////////////////////////////////////
//
// ioitempkg.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Dec 11 14:38:13 2000
// written: Mon Jun 11 14:39:43 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOITEMPKG_CC_DEFINED
#define IOITEMPKG_CC_DEFINED

#include "tcl/ioitempkg.h"

#include "util/objdb.h"
#include "util/object.h"

//---------------------------------------------------------------------
//
// IoCaster
//
//---------------------------------------------------------------------

Tcl::IoCaster::IoCaster() {}

Tcl::IoCaster::~IoCaster() {}

bool Tcl::IoCaster::isMyType(int id) {
  MaybeRef<Util::Object> item(id);
  return (item.isValid() && isMyType(item.get()));
}

//---------------------------------------------------------------------
//
// IsCmd
//
//---------------------------------------------------------------------

Tcl::IsCmd::IsCmd(Tcl_Interp* interp, IoCaster* caster, const char* cmd_name) :
  Tcl::TclCmd(interp, cmd_name, "item_id", 2, 2),
  itsCaster(caster)
{}

Tcl::IsCmd::~IsCmd() {}

void Tcl::IsCmd::invoke() {
  int id = TclCmd::getIntFromArg(1);
  returnBool(itsCaster->isMyType(id));
}

//---------------------------------------------------------------------
//
// CountAllCmd
//
//---------------------------------------------------------------------

Tcl::CountAllCmd::CountAllCmd(Tcl_Interp* interp, IoCaster* caster,
										const char* cmd_name) :
  Tcl::TclCmd(interp, cmd_name, (char*) 0, 1, 1),
  itsCaster(caster)
{}

Tcl::CountAllCmd::~CountAllCmd() {}

void Tcl::CountAllCmd::invoke() {
  int count = 0;
  ObjDb& theList = ObjDb::theDb();
  for (ObjDb::IdIterator
			itr = theList.beginIds(),
			end = theList.endIds();
		 itr != end;
		 ++itr)
	 {
		if (itsCaster->isMyType(itr.getObject()))
		  ++count;
	 }
  returnInt(count);
}

//---------------------------------------------------------------------
//
// FindAllCmd
//
//---------------------------------------------------------------------

Tcl::FindAllCmd::FindAllCmd(Tcl_Interp* interp, IoCaster* caster,
									 const char* cmd_name) :
  Tcl::TclCmd(interp, cmd_name, (char*) 0, 1, 1),
  itsCaster(caster)
{}

Tcl::FindAllCmd::~FindAllCmd() {}

void Tcl::FindAllCmd::invoke() {
  ObjDb& theList = ObjDb::theDb();
  for (ObjDb::IdIterator
			itr = theList.beginIds(),
			end = theList.endIds();
		 itr != end;
		 ++itr)
	 {
		if (itsCaster->isMyType(itr.getObject()))
		  lappendVal(*itr);
	 }
}

//---------------------------------------------------------------------
//
// RemoveAllCmd
//
//---------------------------------------------------------------------

Tcl::RemoveAllCmd::RemoveAllCmd(Tcl_Interp* interp, IoCaster* caster,
									 const char* cmd_name) :
  Tcl::TclCmd(interp, cmd_name, (char*) 0, 1, 1),
  itsCaster(caster)
{}

Tcl::RemoveAllCmd::~RemoveAllCmd() {}

void Tcl::RemoveAllCmd::invoke() {
  ObjDb& theList = ObjDb::theDb();
  for (ObjDb::IdIterator
			itr = theList.beginIds(),
			end = theList.endIds();
		 itr != end;
		 /* increment done in loop body */)
	 {
		if (itsCaster->isMyType(itr.getObject()) &&
			 itr.getObject()->isUnshared())
		  {
			 int remove_me = *itr;
			 ++itr;
			 theList.remove(remove_me);
		  }
		else
		  {
			 ++itr;
		  }
	 }
}

static const char vcid_ioitempkg_cc[] = "$Header$";
#endif // !IOITEMPKG_CC_DEFINED
