///////////////////////////////////////////////////////////////////////
//
// tclveccmd.cc
//
// Copyright (c) 2001-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Jul 12 12:15:46 2001
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

#ifndef TCLVECCMD_CC_DEFINED
#define TCLVECCMD_CC_DEFINED

#include "tcl/tclveccmd.h"

#include "tcl/tclcmd.h"
#include "tcl/tcllistobj.h"

#include "util/error.h"
#include "util/pointers.h"

#include <vector>

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

namespace Tcl
{
  class VecContext;
}

///////////////////////////////////////////////////////////////////////
//
// Tcl::VecContext implements the Context interface in such a way as
// to treat each of the arguments as lists, and provide access to
// slices across those lists, thus allowing "vectorized" command
// invocations.
//
///////////////////////////////////////////////////////////////////////

class Tcl::VecContext : public Tcl::Context
{
public:
  VecContext(Tcl::Interp& interp, unsigned int objc, Tcl_Obj* const objv[],
             unsigned int num_calls) :
    Context(interp, objc, objv),
    itsArg0(objv[0]),
    itsArgs(),
    itsNumCalls(num_calls),
    itsResult()
  {
    for (unsigned int i = 1; i < objc; ++i)
      {
        Tcl::List arg(objv[i]);
        if (arg.length() == 0)
          {
            throw Util::Error("argument was empty");
          }
        itsArgs.push_back( arg.begin<Tcl_Obj*>() );
      }
  }

  virtual ~VecContext() throw() {}

  void flushResult()
  {
    Tcl::Context::setObjResult(itsResult.asObj());
  }

  void next()
  {
    for (unsigned int i = 0; i < itsArgs.size(); ++i)
      {
        if (itsArgs[i].hasMore())
          ++(itsArgs[i]);
      }
  }

protected:
  virtual Tcl_Obj* getObjv(unsigned int argn) throw()
  {
    if (argn == 0) return itsArg0;

    return *(itsArgs.at(argn-1));
  }

  virtual void setObjResult(const Tcl::ObjPtr& obj)
  {
    itsResult.append(obj);
  }

private:
  typedef Tcl::List::Iterator<Tcl_Obj*> Iter;

  Tcl_Obj* itsArg0;
  std::vector<Iter> itsArgs;
  unsigned int const itsNumCalls;
  Tcl::List itsResult;
};

namespace Tcl
{
  class VecDispatcher;
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c Tcl::VecDispatcher reimplements dispatch() to use a specialized
 * \c Context class that treats each of the arguments as lists, and
 * provide access to slices across those lists, thus allowing
 * "vectorized" command invocations.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Tcl::VecDispatcher : public Tcl::Dispatcher
{
public:
  VecDispatcher(unsigned int key_argn) : itsKeyArgn(key_argn) {}

  virtual ~VecDispatcher() throw() {}

  virtual void dispatch(Tcl::Interp& interp,
                        unsigned int objc, Tcl_Obj* const objv[],
                        Tcl::Callback& callback);

private:
  unsigned int itsKeyArgn;
};


void Tcl::VecDispatcher::dispatch(Tcl::Interp& interp,
                                  unsigned int objc, Tcl_Obj* const objv[],
                                  Tcl::Callback& callback)
{
DOTRACE("Tcl::VecDispatcher::dispatch");

  unsigned int ncalls = Tcl::List::getLength(objv[itsKeyArgn]);

  if (ncalls > 1)
    {
      VecContext cx(interp, objc, objv, ncalls);

      for (unsigned int c = 0; c < ncalls; ++c)
        {
          callback.invoke(cx);
          cx.next();
        }

      cx.flushResult();
    }
  else if (ncalls == 1)
    {
      Context cx(interp, objc, objv);
      callback.invoke(cx);
    }
  else // (ncalls == 0)
    {
      ;// do nothing, so we gracefully handle empty lists
    }
}


void Tcl::useVecDispatch(Tcl::Command& cmd, unsigned int key_argn)
{
  cmd.setDispatcher(shared_ptr<Tcl::Dispatcher>(new VecDispatcher(key_argn)));
}

static const char vcid_tclveccmd_cc[] = "$Header$";
#endif // !TCLVECCMD_CC_DEFINED
