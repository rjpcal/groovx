///////////////////////////////////////////////////////////////////////
//
// tclpkg.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 15 12:33:54 1999
// written: Sat Sep  8 08:54:58 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLPKG_CC_DEFINED
#define TCLPKG_CC_DEFINED

#include "tcl/tclpkg.h"

#include "io/io.h"
#include "io/ioutil.h"

#include "util/iter.h"
#include "util/objdb.h"
#include "util/object.h"
#include "util/strings.h"

#include "tcl/tcllistobj.h"

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
  SoftRef<Util::Object> item(uid);
  return (item.isValid() && isMyType(item.get()));
}

namespace
{
  int countAll(shared_ptr<Tcl::ObjCaster> caster)
  {
    ObjDb& theDb = ObjDb::theDb();
    int count = 0;
    for (ObjDb::Iterator itr(theDb.objects()); itr.isValid(); ++itr)
      {
        if (caster->isMyType((*itr).getWeak()))
          ++count;
      }
    return count;
  }

  Tcl::List findAll(shared_ptr<Tcl::ObjCaster> caster)
  {
    ObjDb& theDb = ObjDb::theDb();

    Tcl::List result;

    for (ObjDb::Iterator itr(theDb.objects()); itr.isValid(); ++itr)
      {
        if (caster->isMyType((*itr).getWeak()))
          result.append((*itr).id());
      }

    return result;
  }

  void removeAll(shared_ptr<Tcl::ObjCaster> caster)
  {
    ObjDb& theDb = ObjDb::theDb();
    for (ObjDb::Iterator itr(theDb.objects());
         itr.isValid();
         /* increment done in loop body */)
      {
        DebugEval((*itr)->id());
        DebugEval((*itr)->refCounts()->strongCount());
        DebugEvalNL((*itr)->refCounts()->weakCount());

        if (caster->isMyType((*itr).getWeak()) && (*itr)->isUnshared())
          {
            Util::UID remove_me = (*itr)->id();
            ++itr;
            theDb.remove(remove_me);
          }
        else
          {
            ++itr;
          }
      }
  }

  bool isMyType(shared_ptr<Tcl::ObjCaster> caster, Util::UID id)
  {
    return caster->isIdMyType(id);
  }
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
  if (usage != 0 && *usage != 0)
    return usage;
  return "item_id(s)";
}

const char* Tcl::Pkg::getterUsage(const char* usage)
{
  if (usage != 0 && *usage != 0)
    return usage;
  return "item_id(s)";
}

const char* Tcl::Pkg::setterUsage(const char* usage)
{
  if (usage != 0 && *usage != 0)
    return usage;
  return "item_id(s) new_value(s)";
}

void Tcl::Pkg::defIoCommands()
{
DOTRACE("Tcl::Pkg::defIoCommands");
  defVec( "stringify", "item_id(s)", IO::stringify );
  defVec( "destringify", "item_id(s) string(s)", IO::destringify );

  defVec( "write", "item_id(s)", IO::write );
  defVec( "read", "item_id(s) string(s)", IO::read );

  def( "save", "item_id filename", IO::saveASW );
  def( "load", "item_id filename", IO::loadASR );
}

void Tcl::Pkg::defGenericObjCmds(shared_ptr<Tcl::ObjCaster> caster)
{
DOTRACE("Tcl::Pkg::defGenericObjCmds");
  defVec( "is", "item_id(s)", Util::bindFirst(isMyType, caster) );
  def( "countAll", "", Util::bindFirst(countAll, caster) );
  def( "findAll", "", Util::bindFirst(findAll, caster) );
  def( "removeAll", "", Util::bindFirst(removeAll, caster) );
}

static const char vcid_tclpkg_cc[] = "$Header$";
#endif // !TCLPKG_CC_DEFINED
