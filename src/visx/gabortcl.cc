///////////////////////////////////////////////////////////////////////
//
// gabortcl.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Oct  6 14:16:30 1999
// written: Wed Mar 19 18:00:55 2003
// $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GABORTCL_CC_DEFINED
#define GABORTCL_CC_DEFINED

#include "visx/gabor.h"

#include "tcl/fieldpkg.h"

#include "util/trace.h"

extern "C"
int Gabor_Init(Tcl_Interp* interp)
{
DOTRACE("Gabor_Init");

  Tcl::Pkg* pkg = new Tcl::Pkg(interp, "Gabor", "$Revision$");
  Tcl::defFieldContainer<Gabor>(pkg);
  Tcl::defCreator<Gabor>(pkg);

  return pkg->initStatus();
}

static const char vcid_gabortcl_cc[] = "$Header$";
#endif // !GABORTCL_CC_DEFINED
