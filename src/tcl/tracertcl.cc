///////////////////////////////////////////////////////////////////////
//
// tracertcl.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Feb 17 13:34:40 2000
// written: Wed Mar 15 11:01:12 2000
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

// Template class to run member functions on Tracer's.
template <class FuncType>
class TraceCmd : public Tcl::TclCmd {
public:
  TraceCmd(Tcl::TclPkg* pkg, const char* cmd_name,
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


namespace {
  typedef void (Util::Tracer::* TraceActionFunc) ();
  typedef bool (Util::Tracer::* TraceGetterFunc) () const;
}

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

template <class FuncType>
Tcl::TclCmd* newTraceCmd(Tcl::TclPkg* pkg, const char* cmd_name,
								 Util::Tracer& tracer, FuncType func)
{
  return new TraceCmd<FuncType>(pkg, cmd_name, tracer, func);
}

void addTracing(Tcl::TclPkg* pkg, Util::Tracer& tracer) {
  using Util::Tracer;

  pkg->addCommand(newTraceCmd(pkg, "traceOn", tracer, &Tracer::on));
  pkg->addCommand(newTraceCmd(pkg, "traceOff", tracer, &Tracer::off));
  pkg->addCommand(newTraceCmd(pkg, "traceToggle", tracer, &Tracer::toggle));
  pkg->addCommand(newTraceCmd(pkg, "traceStatus", tracer, &Tracer::status));
}

static const char vcid_tracertcl_cc[] = "$Header$";
#endif // !TRACERTCL_CC_DEFINED
