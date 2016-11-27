/** @file tcl/command.cc binds a wrapped c++ function to a
    tcl::arg_spec and a tcl::dispatcher */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Fri Jun 11 14:50:58 1999
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#include "tcl/command.h"

#include "rutz/fstring.h"
#include "rutz/sfmt.h"

#include "tcl/interp.h"

#include <memory>

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

using std::shared_ptr;
using std::unique_ptr;

///////////////////////////////////////////////////////////////////////
//
// tcl::arg_dispatcher
//
///////////////////////////////////////////////////////////////////////

tcl::arg_dispatcher::~arg_dispatcher() noexcept {}

///////////////////////////////////////////////////////////////////////
//
// default_dispatcher
//
///////////////////////////////////////////////////////////////////////

namespace
{
  class default_dispatcher : public tcl::arg_dispatcher
  {
  public:
    virtual void dispatch(tcl::interpreter& interp,
                          unsigned int objc, Tcl_Obj* const objv[],
                          const std::function<void(tcl::call_context&)>& callback) override
    {
      tcl::call_context ctx(interp, objc, objv);
      callback(ctx);
    }
  };

  shared_ptr<default_dispatcher>
    g_default_dispatcher(std::make_shared<default_dispatcher>());
}

///////////////////////////////////////////////////////////////////////
//
// tcl::command::impl class definition
//
///////////////////////////////////////////////////////////////////////

class tcl::command::impl
{
private:
  impl(const impl&);
  impl& operator=(const impl&);

public:
  impl(std::function<void(tcl::call_context&)>&& cback,
       const char* usg, const arg_spec& spec)
    :
    callback(std::move(cback)),
    dispatcher(g_default_dispatcher),
    usage(usg ? usg : ""),
    argspec(spec)
  {}

  ~impl() noexcept {}

  // These are set once per command object
  std::function<void(tcl::call_context&)> callback;
  shared_ptr<tcl::arg_dispatcher>       dispatcher;
  rutz::fstring                   const usage;
  arg_spec                        const argspec;
};

///////////////////////////////////////////////////////////////////////
//
// tcl::command member definitions
//
///////////////////////////////////////////////////////////////////////

tcl::command::command(std::function<void(tcl::call_context&)>&& callback,
                      const char* usage, const arg_spec& spec)
  :
  rep(new impl(std::move(callback), usage, spec))
{
GVX_TRACE("tcl::command::command");
}

tcl::command::~command() noexcept
{
GVX_TRACE("tcl::command::~command");

  delete rep;
}

bool tcl::command::allows_argc(unsigned int objc) const
{
GVX_TRACE("tcl::command::allows_argc");
  return rep->argspec.allows_argc(objc);
}

bool tcl::command::rejects_argc(unsigned int objc) const
{
  return !this->allows_argc(objc);
}

rutz::fstring tcl::command::usage_string() const
{
GVX_TRACE("tcl::command::usage_string");
  if (rep->argspec.argc_min() ==
      rep->argspec.argc_max())
    return rutz::sfmt("%s (argc=%u)", rep->usage.c_str(),
                      rep->argspec.argc_min());

  // else...
  return rutz::sfmt("%s (argc=[%u..%u])", rep->usage.c_str(),
                    rep->argspec.argc_min(),
                    rep->argspec.argc_max());
}

void tcl::command::call(tcl::interpreter& interp,
                        unsigned int objc, Tcl_Obj* const objv[])
{
  rep->dispatcher->dispatch(interp, objc, objv, rep->callback);
}

shared_ptr<tcl::arg_dispatcher> tcl::command::get_dispatcher() const
{
GVX_TRACE("tcl::command::get_dispatcher");
  return rep->dispatcher;
}

void tcl::command::set_dispatcher(shared_ptr<tcl::arg_dispatcher> dpx)
{
GVX_TRACE("tcl::command::set_dispatcher");
  rep->dispatcher = dpx;
}


///////////////////////////////////////////////////////////////////////
//
// tcl::call_context member definitions
//
///////////////////////////////////////////////////////////////////////

tcl::call_context::call_context(tcl::interpreter& interp,
                                unsigned int objc, Tcl_Obj* const* objv) :
  m_interp(interp),
  m_objc(objc),
  m_objv(objv)
{}

tcl::call_context::~call_context() noexcept
{}

void tcl::call_context::set_obj_result(const tcl::obj& obj)
{
GVX_TRACE("tcl::call_context::set_obj_result");
  m_interp.set_result(obj);
}
