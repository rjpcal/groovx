///////////////////////////////////////////////////////////////////////
//
// tracertcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Feb 17 13:34:40 2000
// written: Wed Jul 18 12:21:35 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRACERTCL_CC_DEFINED
#define TRACERTCL_CC_DEFINED

#include "tcl/tracertcl.h"

#include "tcl/tclpkg.h"

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

void Tcl::defTracing(Tcl::Pkg* pkg, Util::Tracer& tracer)
{
  using Util::Tracer;

  pkg->defRaw( "traceOn", 0, "",
               TraceFunc<TraceActionFunc>(tracer, &Tracer::on) );
  pkg->defRaw( "traceOff", 0, "",
               TraceFunc<TraceActionFunc>(tracer, &Tracer::off) );
  pkg->defRaw( "traceToggle", 0, "",
               TraceFunc<TraceActionFunc>(tracer, &Tracer::toggle) );
  pkg->defRaw( "traceStatus", 0, "",
               TraceFunc<TraceGetterFunc>(tracer, &Tracer::status) );
}

static const char vcid_tracertcl_cc[] = "$Header$";
#endif // !TRACERTCL_CC_DEFINED
