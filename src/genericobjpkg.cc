///////////////////////////////////////////////////////////////////////
//
// genericobjpkg.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Dec 11 14:38:13 2000
// written: Wed Jul 11 14:17:59 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GENERICOBJPKG_CC_DEFINED
#define GENERICOBJPKG_CC_DEFINED

#include "tcl/genericobjpkg.h"

#include "util/objdb.h"
#include "util/object.h"

#ifdef FUNCTIONAL_OK
#  include <algorithm>
#  include <functional>
#endif

//---------------------------------------------------------------------
//
// ObjCaster
//
//---------------------------------------------------------------------

Tcl::ObjCaster::ObjCaster() {}

Tcl::ObjCaster::~ObjCaster() {}

bool Tcl::ObjCaster::isIdMyType(Util::UID uid) {
  WeakRef<Util::Object> item(uid);
  return (item.isValid() && isMyType(item.get()));
}

//---------------------------------------------------------------------
//
// IsCmd
//
//---------------------------------------------------------------------

Tcl::IsCmd::IsCmd(Tcl_Interp* interp, ObjCaster* caster, const char* cmd_name) :
  Tcl::TclCmd(interp, cmd_name, "item_id", 2, 2),
  itsCaster(caster)
{}

Tcl::IsCmd::~IsCmd() {}

void Tcl::IsCmd::invoke() {
  int id = TclCmd::getIntFromArg(1);
  returnVal(itsCaster->isIdMyType(id));
}

//---------------------------------------------------------------------
//
// CountAllCmd
//
//---------------------------------------------------------------------

Tcl::CountAllCmd::CountAllCmd(Tcl_Interp* interp, ObjCaster* caster,
                              const char* cmd_name) :
  Tcl::TclCmd(interp, cmd_name, (char*) 0, 1, 1),
  itsCaster(caster)
{}

Tcl::CountAllCmd::~CountAllCmd() {}

void Tcl::CountAllCmd::invoke() {
  ObjDb& theDb = ObjDb::theDb();
#ifndef FUNCTIONAL_OK
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
  returnVal(count);
#else
  returnVal(std::count_if(theDb.beginPtrs(), theDb.endPtrs(),
                          std::bind1st(std::mem_fun(&ObjCaster::isMyType),
                                       itsCaster)));
#endif
}

//---------------------------------------------------------------------
//
// FindAllCmd
//
//---------------------------------------------------------------------

Tcl::FindAllCmd::FindAllCmd(Tcl_Interp* interp, ObjCaster* caster,
                            const char* cmd_name) :
  Tcl::TclCmd(interp, cmd_name, (char*) 0, 1, 1),
  itsCaster(caster)
{}

Tcl::FindAllCmd::~FindAllCmd() {}

void Tcl::FindAllCmd::invoke() {
  ObjDb& theDb = ObjDb::theDb();

#if 1
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

  returnVal(result);

  /*
    STL-style functional implementation... unfortunately, this is
    about 8 times slower than the for-loop version
  */
#else
    std::remove_copy_if(theDb.beginIds(), theDb.endIds(),
                        resultAppender<int>(),
                        std::bind1st(std::mem_fun(&ObjCaster::isIdNotMyType),
                        itsCaster));
#endif
}

//---------------------------------------------------------------------
//
// RemoveAllCmd
//
//---------------------------------------------------------------------

Tcl::RemoveAllCmd::RemoveAllCmd(Tcl_Interp* interp, ObjCaster* caster,
                            const char* cmd_name) :
  Tcl::TclCmd(interp, cmd_name, (char*) 0, 1, 1),
  itsCaster(caster)
{}

Tcl::RemoveAllCmd::~RemoveAllCmd() {}

void Tcl::RemoveAllCmd::invoke() {
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

static const char vcid_genericobjpkg_cc[] = "$Header$";
#endif // !GENERICOBJPKG_CC_DEFINED
