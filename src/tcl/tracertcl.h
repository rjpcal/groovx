///////////////////////////////////////////////////////////////////////
//
// tracertcl.h
//
// Copyright (c) 2000-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Thu Feb 17 13:32:11 2000
// written: Wed Mar 19 12:45:41 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRACERTCL_H_DEFINED
#define TRACERTCL_H_DEFINED

namespace Util { class Tracer; }

namespace Tcl
{
  class Pkg;

  void defTracing(Tcl::Pkg* pkg, Util::Tracer& tracer);
}

static const char vcid_tracertcl_h[] = "$Header$";
#endif // !TRACERTCL_H_DEFINED
