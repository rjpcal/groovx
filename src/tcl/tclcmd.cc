///////////////////////////////////////////////////////////////////////
//
// tclcmd.cc
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Fri Jun 11 14:50:58 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLCMD_CC_DEFINED
#define TCLCMD_CC_DEFINED

#include "tcl/tclcmd.h"

#include "tcl/tclcommandgroup.h"
#include "tcl/tclsafeinterp.h"

#include "util/pointers.h"
#include "util/strings.h"

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

///////////////////////////////////////////////////////////////////////
//
// Tcl::Callback
//
///////////////////////////////////////////////////////////////////////

Tcl::Callback::~Callback() throw() {}

///////////////////////////////////////////////////////////////////////
//
// Tcl::Dispatcher
//
///////////////////////////////////////////////////////////////////////

Tcl::Dispatcher::~Dispatcher() throw() {}

///////////////////////////////////////////////////////////////////////
//
// DefaultDispatcher
//
///////////////////////////////////////////////////////////////////////

namespace
{
  class DefaultDispatcher : public Tcl::Dispatcher
  {
  public:
    virtual void dispatch(Tcl::Interp& interp,
                          unsigned int objc, Tcl_Obj* const objv[],
                          Tcl::Callback& callback)
    {
      Tcl::Context ctx(interp, objc, objv);
      callback.invoke(ctx);
    }
  };

  shared_ptr<DefaultDispatcher>
    theDefaultDispatcher(new DefaultDispatcher);
}

///////////////////////////////////////////////////////////////////////
//
// Tcl::Command::Impl class definition
//
///////////////////////////////////////////////////////////////////////

class Tcl::Command::Impl
{
private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:
  Impl(shared_ptr<Tcl::Callback> cback,
       const char* usg,
       int objc_min, int objc_max, bool exact_objc)
    :
    callback(cback),
    dispatcher(theDefaultDispatcher),
    usage(usg ? usg : ""),
    objcMin(objc_min < 0 ? 0 : (unsigned int) objc_min),
    objcMax( (objc_max > 0) ? (unsigned int) objc_max : objcMin),
    exactObjc(exact_objc)
  {}

  ~Impl() throw() {}

  // These are set once per command object
  shared_ptr<Tcl::Callback> callback;
  shared_ptr<Tcl::Dispatcher> dispatcher;
  const fstring usage;
  const unsigned int objcMin;
  const unsigned int objcMax;
  const bool exactObjc;
};

///////////////////////////////////////////////////////////////////////
//
// Tcl::Command member definitions
//
///////////////////////////////////////////////////////////////////////

Tcl::Command::Command(shared_ptr<Tcl::Callback> callback,
                      // FIXME cmd_name not needed here:
                      const char* /*cmd_name*/, const char* usage,
                      int objc_min, int objc_max, bool exact_objc)
  :
  rep(new Impl(callback, usage, objc_min, objc_max, exact_objc))
{
DOTRACE("Tcl::Command::Command");
}

shared_ptr<Tcl::Command> Tcl::Command::make(
          Tcl::Interp& interp,
          shared_ptr<Tcl::Callback> callback,
          const char* cmd_name, const char* usage,
          int objc_min, int objc_max, bool exact_objc,
          const FilePosition& src_pos)
{
DOTRACE("Tcl::Command::make");

  CommandGroup* const group =
    Tcl::CommandGroup::make(interp, cmd_name, src_pos);

  ASSERT(group != 0);

  shared_ptr<Command> cmd( new Command(callback,
                                       cmd_name, usage,
                                       objc_min, objc_max,
                                       exact_objc) );

  // We don't want to have to keep 'group' as a member of Tcl::Command
  // since it involves circular references -- Tcl::CommandGroup keeps
  // a list of Tcl::Command objects, so we'd prefer not to have a
  // back-reference here. If it becomes necessary to keep a
  // back-reference, then there needs to be a way for
  // Tcl::CommandGroup to notify its Tcl::Command list that it is
  // destructing, so that the Tcl::Command objects can "forget" their
  // back-reference.
  group->add(cmd);

  return cmd;
}

Tcl::Command::~Command() throw()
{
DOTRACE("Tcl::Command::~Command");

  delete rep;
}

bool Tcl::Command::allowsObjc(unsigned int objc) const
{
DOTRACE("Tcl::Command::allowsObjc");
  if (rep->exactObjc)
    {
      return (objc == rep->objcMin || objc == rep->objcMax);
    }

  return (objc >= rep->objcMin && objc <= rep->objcMax);
}

bool Tcl::Command::rejectsObjc(unsigned int objc) const
{
  return !allowsObjc(objc);
}

fstring Tcl::Command::usageString() const
{
DOTRACE("Tcl::Command::usageString");
  return rep->usage;
}

void Tcl::Command::call(Tcl::Interp& interp,
                        unsigned int objc, Tcl_Obj* const objv[])
{
  rep->dispatcher->dispatch(interp, objc, objv, *rep->callback);
}

shared_ptr<Tcl::Dispatcher> Tcl::Command::getDispatcher() const
{
DOTRACE("Tcl::Command::getDispatcher");
  return rep->dispatcher;
}

void Tcl::Command::setDispatcher(shared_ptr<Tcl::Dispatcher> dpx)
{
DOTRACE("Tcl::Command::setDispatcher");
  rep->dispatcher = dpx;
}


///////////////////////////////////////////////////////////////////////
//
// Tcl::Context member definitions
//
///////////////////////////////////////////////////////////////////////

Tcl::Context::Context(Tcl::Interp& interp,
                      unsigned int objc, Tcl_Obj* const* objv) :
  itsInterp(interp),
  itsObjc(objc),
  itsObjv(objv)
{}

Tcl::Context::~Context() throw()
{}

void Tcl::Context::setObjResult(const Tcl::ObjPtr& obj)
{
DOTRACE("Tcl::Context::setObjResult");
  itsInterp.setResult(obj);
}

static const char vcid_tclcmd_cc[] = "$Header$";
#endif // !TCLCMD_CC_DEFINED
