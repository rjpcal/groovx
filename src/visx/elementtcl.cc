///////////////////////////////////////////////////////////////////////
//
// elementtcl.cc
//
// Copyright (c) 2003-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue May 27 15:38:33 2003
// commit: $Id$
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

#ifndef ELEMENTTCL_CC_DEFINED
#define ELEMENTTCL_CC_DEFINED

#include "gfx/toglet.h"

#include "tcl/objpkg.h"
#include "tcl/tclpkg.h"

#include "util/strings.h"

#include "visx/element.h"

#include "util/trace.h"

extern "C"
int Element_Init(Tcl_Interp* interp)
{
DOTRACE("Element_Init");

  PKG_CREATE(interp, "Element", "$Revision$");
  pkg->inheritPkg("IO");
  Tcl::defGenericObjCmds<Element>(pkg);

  pkg->defGetter("widget", &Element::getWidget);
  pkg->defGetter("trialType", &Element::trialType);
  pkg->defGetter("lastResponse", &Element::lastResponse);
  pkg->defGetter("info", &Element::vxInfo);
  pkg->defAction("halt", &Element::vxHalt);
  pkg->defAction("undo", &Element::vxUndo);
  pkg->defAction("reset", &Element::vxReset);

  PKG_RETURN;
}

static const char vcid_elementtcl_cc[] = "$Header$";
#endif // !ELEMENTTCL_CC_DEFINED
