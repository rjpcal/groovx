///////////////////////////////////////////////////////////////////////
//
// tracertcl.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Feb 17 13:34:40 2000
// written: Fri Nov 10 17:03:55 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRACERTCL_CC_DEFINED
#define TRACERTCL_CC_DEFINED

#include "tcl/tracertcl.h"

#include "tcl/tclcmd.h"
#include "tcl/tclpkg.h"
#include "util/tracer.h"

#define LOCAL_ASSERT
#include "util/debug.h"

namespace {
  typedef void (Util::Tracer::* TraceActionFunc) ();
  typedef bool (Util::Tracer::* TraceGetterFunc) () const;
}

namespace Tcl {

///////////////////////////////////////////////////////////////////////
//
// Template command class that runs member functions of Tracer's.
//
///////////////////////////////////////////////////////////////////////

template <class FuncType>
class TraceCmd : public TclCmd {
public:
  TraceCmd(TclPkg* pkg, const char* cmd_name,
			  Util::Tracer& tracer, FuncType func) :
	 TclCmd(pkg->interp(), pkg->makePkgCmdName(cmd_name), (char*) 0, 1, 1),
	 itsTracer(tracer),
	 itsFunc(func)
	 {}

protected:
  virtual void invoke();

private:
  Util::Tracer& itsTracer;
  FuncType itsFunc;
};


//---------------------------------------------------------------------
//
// Specializations of TraceCmd::invoke for different member function types
//
//---------------------------------------------------------------------

// Specialization of TraceCmd::invoke for action functions
template <>
void TraceCmd<TraceActionFunc>::invoke() {
  (itsTracer.*itsFunc)();
}


// Specialization of TraceCmd::invoke for getter functions
template <>
void TraceCmd<TraceGetterFunc>::invoke() {
  returnVal( (itsTracer.*itsFunc)() );
}


} // end namespace Tcl

namespace {

template <class FuncType>
inline Tcl::TclCmd* newTraceCmd(Tcl::TclPkg* pkg, const char* cmd_name,
										  Util::Tracer& tracer, FuncType func)
{
  return new Tcl::TraceCmd<FuncType>(pkg, cmd_name, tracer, func);
}

} // end unnamed namespace

void Tcl::addTracing(Tcl::TclPkg* pkg, Util::Tracer& tracer) {
  using Util::Tracer;

  pkg->addCommand(newTraceCmd(pkg, "traceOn", tracer, &Tracer::on));
  pkg->addCommand(newTraceCmd(pkg, "traceOff", tracer, &Tracer::off));
  pkg->addCommand(newTraceCmd(pkg, "traceToggle", tracer, &Tracer::toggle));
  pkg->addCommand(newTraceCmd(pkg, "traceStatus", tracer, &Tracer::status));
}

static const char vcid_tracertcl_cc[] = "$Header$";
#endif // !TRACERTCL_CC_DEFINED
