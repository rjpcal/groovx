///////////////////////////////////////////////////////////////////////
//
// tclcmd.cc
//
// Copyright (c) 1999-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Fri Jun 11 14:50:58 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
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
                          Tcl::Command& cmd)
    {
      Tcl::Context ctx(interp, objc, objv);
      cmd.invoke(ctx);
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
  Impl(const char* cmd_name, const char* usg,
       int objc_min, int objc_max, bool exact_objc) :
    dispatcher(theDefaultDispatcher),
    usage(usg ? usg : ""),
    objcMin(objc_min < 0 ? 0 : (unsigned int) objc_min),
    objcMax( (objc_max > 0) ? (unsigned int) objc_max : objcMin),
    exactObjc(exact_objc),
    overloads(0)
  {}

  ~Impl() throw() {}

  // These are set once per command object
  shared_ptr<Tcl::Dispatcher> dispatcher;
  const fstring usage;
  const unsigned int objcMin;
  const unsigned int objcMax;
  const bool exactObjc;
  CommandGroup* overloads;
};

///////////////////////////////////////////////////////////////////////
//
// Tcl::Command member definitions
//
///////////////////////////////////////////////////////////////////////

Tcl::Command::Command(Tcl::Interp& interp,
                      const char* cmd_name, const char* usage,
                      int objc_min, int objc_max, bool exact_objc)
  :
  rep(new Impl(cmd_name, usage, objc_min, objc_max, exact_objc))
{
DOTRACE("Tcl::Command::Command");

  rep->overloads = Tcl::CommandGroup::make(interp, cmd_name);

  Assert(rep->overloads != 0);

  rep->overloads->add(this);
}

Tcl::Command::~Command() throw()
{
DOTRACE("Tcl::Command::~Command");

  rep->overloads->remove(this);

  rep->overloads = 0;

  delete rep;
}

const fstring& Tcl::Command::name() const
{
DOTRACE("Tcl::Command::name");
  return rep->overloads->cmdName();
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

fstring Tcl::Command::rawUsage() const
{
DOTRACE("Tcl::Command::rawUsage");
  fstring result(rep->overloads->cmdName());
  if (!rep->usage.is_empty())
    result.append(" ", rep->usage);
  return result;
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
