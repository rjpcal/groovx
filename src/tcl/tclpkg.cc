///////////////////////////////////////////////////////////////////////
//
// tclpkg.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 15 12:33:54 1999
// written: Thu Aug 16 15:14:31 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLPKG_CC_DEFINED
#define TCLPKG_CC_DEFINED

#ifdef ACC_COMPILER // aCC needs this to be happy with the header
#include "util/ref.h"
#endif

#include "tcl/tclpkg.h"

#include "io/io.h"
#include "io/ioutil.h"

#include "util/objdb.h"
#include "util/object.h"
#include "util/strings.h"

#include "tcl/tcllistobj.h"

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
      for (ObjDb::Iterator
             itr = theDb.begin(),
             end = theDb.end();
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

      for (ObjDb::Iterator
             itr = theDb.begin(),
             end = theDb.end();
           itr != end;
           ++itr)
        {
          if (itsCaster->isMyType(*itr))
            result.append((*itr)->id());
        }

      ctx.setResult(result);
    }
  };

  struct RemoveAllFunc
  {
    shared_ptr<ObjCaster> itsCaster;

    RemoveAllFunc(shared_ptr<ObjCaster> caster) : itsCaster(caster) {}

    void operator()(Tcl::Context&)
    {
      ObjDb& theDb = ObjDb::theDb();
      for (ObjDb::Iterator
             itr = theDb.begin(),
             end = theDb.end();
           itr != end;
           /* increment done in loop body */)
        {
          if (itsCaster->isMyType(*itr) && (*itr)->isUnshared())
            {
              int remove_me = (*itr)->id();
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
  defVecRaw( "is", 1, "item_id(s)", IsFunc(caster) );
  defRaw( "countAll", 0, "", CountAllFunc(caster) );
  defRaw( "findAll", 0, "", FindAllFunc(caster) );
  defRaw( "removeAll", 0, "", RemoveAllFunc(caster) );
}

static const char vcid_tclpkg_cc[] = "$Header$";
#endif // !TCLPKG_CC_DEFINED
