///////////////////////////////////////////////////////////////////////
//
// tracertcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Feb 17 13:34:40 2000
// written: Wed Jul 18 10:39:52 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRACERTCL_CC_DEFINED
#define TRACERTCL_CC_DEFINED

#include "tcl/tracertcl.h"

#include "tcl/tclitempkg.h"

#include "util/tracer.h"

namespace
{
  typedef void (Util::Tracer::* TraceActionFunc) ();
  typedef bool (Util::Tracer::* TraceGetterFunc) () const;

  //
  // Template functor that runs member functions of bound Tracer's.
  //

  template <class FuncType>
  struct TraceFunc
  {
    Util::Tracer& itsTracer;
    FuncType itsFunc;

    TraceFunc(Util::Tracer& tracer, FuncType func) :
      itsTracer(tracer),
      itsFunc(func)
    {}

    void operator()(Tcl::Context& ctx);
  };

  // Specialization of TraceCmd::invoke for action functions
  template <>
  void TraceFunc<TraceActionFunc>::operator()(Tcl::Context& ctx)
  {
    (itsTracer.*itsFunc)();
  }


  // Specialization of TraceCmd::invoke for getter functions
  template <>
  void TraceFunc<TraceGetterFunc>::operator()(Tcl::Context& ctx)
  {
    ctx.setResult( (itsTracer.*itsFunc)() );
  }


} // end namespace Tcl

void Tcl::defTracing(Tcl::TclItemPkg* pkg, Util::Tracer& tracer)
{
  using Util::Tracer;

  pkg->defRaw( TraceFunc<TraceActionFunc>(tracer, &Tracer::on),
               "traceOn", 0, 0 );
  pkg->defRaw( TraceFunc<TraceActionFunc>(tracer, &Tracer::off),
               "traceOff", 0, 0 );
  pkg->defRaw( TraceFunc<TraceActionFunc>(tracer, &Tracer::toggle),
               "traceToggle", 0, 0 );
  pkg->defRaw( TraceFunc<TraceGetterFunc>(tracer, &Tracer::status),
               "traceStatus", 0, 0 );
}

static const char vcid_tracertcl_cc[] = "$Header$";
#endif // !TRACERTCL_CC_DEFINED
