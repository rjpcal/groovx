///////////////////////////////////////////////////////////////////////
//
// tclitempkg.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Jun 15 12:33:54 1999
// written: Mon Jul 16 13:01:23 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLITEMPKG_CC_DEFINED
#define TCLITEMPKG_CC_DEFINED

#ifdef ACC_COMPILER // aCC needs this to be happy with the header
#include "util/ref.h"
#endif

#include "util/objdb.h"
#include "util/object.h"

#include "tcl/tclitempkg.h"
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

bool Tcl::ObjCaster::isIdMyType(Util::UID uid) {
  WeakRef<Util::Object> item(uid);
  return (item.isValid() && isMyType(item.get()));
}

namespace Tcl
{

/** CountAllCmd **/

class CountAllCmd : public TclCmd {
public:
  CountAllCmd(Tcl_Interp* interp, shared_ptr<ObjCaster> caster,
              const char* cmd_name);
  virtual ~CountAllCmd();

protected:
  virtual void invoke(Tcl::Context& ctx);

private:
  shared_ptr<ObjCaster> itsCaster;
};

/** FindAllCmd **/

class FindAllCmd : public TclCmd {
public:
  FindAllCmd(Tcl_Interp* interp, shared_ptr<ObjCaster> caster,
             const char* cmd_name);
  virtual ~FindAllCmd();

protected:
  virtual void invoke(Tcl::Context& ctx);

private:
  shared_ptr<ObjCaster> itsCaster;
};

/** RemoveAllCmd **/

class RemoveAllCmd : public TclCmd {
public:
  RemoveAllCmd(Tcl_Interp* interp, shared_ptr<ObjCaster> caster,
               const char* cmd_name);
  virtual ~RemoveAllCmd();

protected:
  virtual void invoke(Tcl::Context& ctx);

private:
  shared_ptr<ObjCaster> itsCaster;
};

/** IsCmd **/

class IsCmd : public TclCmd {
public:
  IsCmd(Tcl_Interp* interp, shared_ptr<ObjCaster> caster,
        const char* cmd_name);
  virtual ~IsCmd();

protected:
  virtual void invoke(Tcl::Context& ctx);

private:
  shared_ptr<ObjCaster> itsCaster;
};

}

//---------------------------------------------------------------------
//
// IsCmd
//
//---------------------------------------------------------------------

Tcl::IsCmd::IsCmd(Tcl_Interp* interp,
                  shared_ptr<ObjCaster> caster,
                  const char* cmd_name) :
  Tcl::TclCmd(interp, cmd_name, "item_id", 2, 2),
  itsCaster(caster)
{}

Tcl::IsCmd::~IsCmd() {}

void Tcl::IsCmd::invoke(Tcl::Context& ctx)
{
  Util::UID id = ctx.getValFromArg(1, TypeCue<Util::UID>());
  ctx.setResult(itsCaster->isIdMyType(id));
}

//---------------------------------------------------------------------
//
// CountAllCmd
//
//---------------------------------------------------------------------

Tcl::CountAllCmd::CountAllCmd(Tcl_Interp* interp,
                              shared_ptr<ObjCaster> caster,
                              const char* cmd_name) :
  Tcl::TclCmd(interp, cmd_name, (char*) 0, 1, 1),
  itsCaster(caster)
{}

Tcl::CountAllCmd::~CountAllCmd() {}

void Tcl::CountAllCmd::invoke(Tcl::Context& ctx)
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

//---------------------------------------------------------------------
//
// FindAllCmd
//
//---------------------------------------------------------------------

Tcl::FindAllCmd::FindAllCmd(Tcl_Interp* interp,
                            shared_ptr<ObjCaster> caster,
                            const char* cmd_name) :
  Tcl::TclCmd(interp, cmd_name, (char*) 0, 1, 1),
  itsCaster(caster)
{}

Tcl::FindAllCmd::~FindAllCmd() {}

void Tcl::FindAllCmd::invoke(Tcl::Context& ctx)
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

//---------------------------------------------------------------------
//
// RemoveAllCmd
//
//---------------------------------------------------------------------

Tcl::RemoveAllCmd::RemoveAllCmd(Tcl_Interp* interp,
                                shared_ptr<ObjCaster> caster,
                                const char* cmd_name) :
  Tcl::TclCmd(interp, cmd_name, (char*) 0, 1, 1),
  itsCaster(caster)
{}

Tcl::RemoveAllCmd::~RemoveAllCmd() {}

void Tcl::RemoveAllCmd::invoke(Tcl::Context& ctx)
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

void Tcl::doAddGenericObjCmds(Tcl::TclPkg* pkg,
                              shared_ptr<Tcl::ObjCaster> caster)
{
  pkg->addCommand( new IsCmd(pkg->interp(), caster,
                             pkg->makePkgCmdName("is")));
  pkg->addCommand( new CountAllCmd(pkg->interp(), caster,
                                   pkg->makePkgCmdName("countAll")));
  pkg->addCommand( new FindAllCmd(pkg->interp(), caster,
                                  pkg->makePkgCmdName("findAll")));
  pkg->addCommand( new RemoveAllCmd(pkg->interp(), caster,
                                    pkg->makePkgCmdName("removeAll")));
}

///////////////////////////////////////////////////////////////////////
//
// TclItemPkg member definitions
//
///////////////////////////////////////////////////////////////////////

Tcl::TclItemPkg::TclItemPkg(Tcl_Interp* interp,
                            const char* name, const char* version) :
  TclPkg(interp, name, version)
{}

Tcl::TclItemPkg::~TclItemPkg() {}

const char* Tcl::TclItemPkg::actionUsage(const char* usage)
{
  return usage ? usage : "item_id(s)";
}

const char* Tcl::TclItemPkg::getterUsage(const char* usage)
{
  return usage ? usage : "item_id(s)";
}

const char* Tcl::TclItemPkg::setterUsage(const char* usage)
{
  return usage ? usage : "item_id(s) new_value(s)";
}


///////////////////////////////////////////////////////////////////////
//
// IO command definitions
//
///////////////////////////////////////////////////////////////////////

void Tcl::TclItemPkg::addIoCommands()
{
DOTRACE("Tcl::TclItemPkg::addIoCommands");
  addCommand( new StringifyCmd(interp(), makePkgCmdName("stringify")) );
  addCommand( new DestringifyCmd(interp(), makePkgCmdName("destringify")) );

  addCommand( new WriteCmd(interp(), makePkgCmdName("write")) );
  addCommand( new ReadCmd(interp(), makePkgCmdName("read")) );

  addCommand( new ASWSaveCmd(interp(), makePkgCmdName("save")) );
  addCommand( new ASRLoadCmd(interp(), makePkgCmdName("load")) );
}

static const char vcid_tclitempkg_cc[] = "$Header$";
#endif // !TCLITEMPKG_CC_DEFINED
