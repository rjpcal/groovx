///////////////////////////////////////////////////////////////////////
//
// tracertcl.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Feb 17 13:32:11 2000
// written: Thu May 10 12:04:36 2001
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
