///////////////////////////////////////////////////////////////////////
//
// tracertcl.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Feb 17 13:32:11 2000
// written: Wed Jul 18 10:38:40 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TRACERTCL_H_DEFINED
#define TRACERTCL_H_DEFINED

namespace Tcl  { class TclItemPkg; }
namespace Util { class Tracer; }

namespace Tcl
{
  void defTracing(Tcl::TclItemPkg* pkg, Util::Tracer& tracer);
}

static const char vcid_tracertcl_h[] = "$Header$";
#endif // !TRACERTCL_H_DEFINED
