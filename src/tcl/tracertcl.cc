///////////////////////////////////////////////////////////////////////
//
// tracertcl.cc
//
// Copyright (c) 2000-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Feb 17 13:34:40 2000
// written: Mon Jan 13 11:08:26 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRACERTCL_CC_DEFINED
#define TRACERTCL_CC_DEFINED

#include "tcl/tracertcl.h"

#include "tcl/tclpkg.h"

#include "util/tracer.h"

using namespace Util;

void Tcl::defTracing(Tcl::Pkg* pkg, Tracer& tracer)
{
  pkg->def( "traceOn",     "", bindFirst(memFunc(&Tracer::on), &tracer) );
  pkg->def( "traceOff",    "", bindFirst(memFunc(&Tracer::off), &tracer) );
  pkg->def( "traceToggle", "", bindFirst(memFunc(&Tracer::toggle), &tracer) );
  pkg->def( "traceStatus", "", bindFirst(memFunc(&Tracer::status), &tracer) );
}

static const char vcid_tracertcl_cc[] = "$Header$";
#endif // !TRACERTCL_CC_DEFINED
