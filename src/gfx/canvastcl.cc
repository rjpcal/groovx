///////////////////////////////////////////////////////////////////////
//
// canvastcl.cc
//
// Copyright (c) 2004-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Sat Oct 23 19:24:11 2004
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef CANVASTCL_CC_DEFINED
#define CANVASTCL_CC_DEFINED

#include "gfx/canvas.h"
#include "gfx/glcanvas.h"
#include "gfx/recttcl.h"

#include "tcl/objpkg.h"
#include "tcl/tclpkg.h"

#include "util/trace.h"

extern "C"
int Canvas_Init(Tcl_Interp* interp)
{
DOTRACE("Canvas_Init");

  PKG_CREATE(interp, "Canvas", "$Revision$");
  pkg->inheritPkg("Obj");
  Tcl::defGenericObjCmds<Gfx::Canvas>(pkg, SRC_POS);

  pkg->defGetter("worldViewport", &Gfx::Canvas::getWorldViewport, SRC_POS);

  PKG_RETURN;
}

extern "C"
int Glcanvas_Init(Tcl_Interp* interp)
{
DOTRACE("Glcanvas_Init");

  PKG_CREATE(interp, "GLCanvas", "$Revision$");
  pkg->inheritPkg("Canvas");
  Tcl::defGenericObjCmds<GLCanvas>(pkg, SRC_POS);

  PKG_RETURN;
}

static const char vcid_canvastcl_cc[] = "$Header$";
#endif // !CANVASTCL_CC_DEFINED
