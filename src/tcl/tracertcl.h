///////////////////////////////////////////////////////////////////////
//
// tracertcl.h
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Feb 17 13:32:11 2000
// written: Fri Nov 10 17:03:49 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRACERTCL_H_DEFINED
#define TRACERTCL_H_DEFINED

namespace Tcl  { class TclPkg; }
namespace Util { class Tracer; }

namespace Tcl {
  void addTracing(Tcl::TclPkg* pkg, Util::Tracer& tracer);
}

static const char vcid_tracertcl_h[] = "$Header$";
#endif // !TRACERTCL_H_DEFINED
