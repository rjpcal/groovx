///////////////////////////////////////////////////////////////////////
//
// tracertcl.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Feb 17 13:32:11 2000
// written: Fri Jan 18 16:06:55 2002
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
