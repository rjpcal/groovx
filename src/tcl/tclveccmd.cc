///////////////////////////////////////////////////////////////////////
//
// tclveccmd.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Jul 12 12:15:46 2001
// written: Thu Jul 12 13:04:16 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLVECCMD_CC_DEFINED
#define TCLVECCMD_CC_DEFINED

#include "tcl/tclveccmd.h"

#include "tcl/tclerror.h"

#include "util/minivec.h"

#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

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

class Tcl::VecContext : public Tcl::Context {
public:
  VecContext(Tcl_Interp* interp, unsigned int objc, Tcl_Obj* const objv[],
             unsigned int num_calls) :
    Tcl::Context(interp, objc, objv),
    itsArgs(),
    itsNumCalls(num_calls),
    itsResult()
  {
    for (unsigned int i = 0; i < objc; ++i)
      {
        Tcl::List arg(objv[i]);
        if (arg.length() == 0)
          {
            throw Tcl::TclError("argument was empty");
          }
        itsArgs.push_back( arg.begin<Tcl_Obj*>() );
      }
  }

  virtual ~VecContext()
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
  virtual Tcl_Obj* getObjv(unsigned int argn)
  {
    return *(itsArgs.at(argn));
  }

  virtual void setObjResult(Tcl_Obj* obj)
  {
    itsResult.append(obj);
  }

private:
  typedef Tcl::List::Iterator<Tcl_Obj*> Iter;

  minivec<Iter> itsArgs;
  unsigned int const itsNumCalls;
  Tcl::List itsResult;
};


Tcl::VecCmd::VecCmd(Tcl_Interp* interp, const char* cmd_name, const char* usage,
                    unsigned int key_argn,
                    int objc_min=0, int objc_max=-1, bool exact_objc=false) :
  Tcl::TclCmd(interp, cmd_name, usage, objc_min, objc_max, exact_objc),
  itsKeyArgn(key_argn)
{}

void Tcl::VecCmd::rawInvoke(Tcl_Interp* interp, unsigned int objc,
                            Tcl_Obj* const objv[])
{
DOTRACE("Tcl::VecCmd::rawInvoke");

  unsigned int ncalls = Tcl::List::getLength(objv[itsKeyArgn]);

  if (ncalls > 1)
    {
      VecContext cx(interp, objc, objv, ncalls);

      for (unsigned int c = 0; c < ncalls; ++c)
        {
          invoke(cx);
          cx.next();
        }
    }
  else if (ncalls == 1)
    {
      Tcl::TclCmd::rawInvoke(interp, objc, objv);
    }
  else // (ncalls == 0)
    {
      throw Tcl::TclError("key argument was empty");
    }
}

static const char vcid_tclveccmd_cc[] = "$Header$";
#endif // !TCLVECCMD_CC_DEFINED
