///////////////////////////////////////////////////////////////////////
//
// tracertcl.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Feb 17 13:34:40 2000
// written: Fri Sep  7 16:47:51 2001
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
  void traceOn(Util::Tracer& tracer) { tracer.on(); }
  void traceOff(Util::Tracer& tracer) { tracer.off(); }
  void traceToggle(Util::Tracer& tracer) { tracer.toggle(); }
  bool traceStatus(Util::Tracer& tracer) { return tracer.status(); }
}

void Tcl::defTracing(Tcl::Pkg* pkg, Util::Tracer& tracer)
{
  pkg->def( "traceOn", "", Util::bindFirst(traceOn, tracer) );
  pkg->def( "traceOff", "", Util::bindFirst(traceOff, tracer) );
  pkg->def( "traceToggle", "", Util::bindFirst(traceToggle, tracer) );
  pkg->def( "traceStatus", "", Util::bindFirst(traceStatus, tracer) );
}

static const char vcid_tracertcl_cc[] = "$Header$";
#endif // !TRACERTCL_CC_DEFINED
