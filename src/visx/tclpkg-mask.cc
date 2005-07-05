///////////////////////////////////////////////////////////////////////
//
// masktcl.cc
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Thu Sep 23 18:19:05 1999
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_VISX_TCLPKG_MASK_CC_UTC20050628171008_DEFINED
#define GROOVX_VISX_TCLPKG_MASK_CC_UTC20050628171008_DEFINED

#include "visx/tclpkg-mask.h"

#include "io/fieldpkg.h"

#include "visx/maskhatch.h"

#include "rutz/trace.h"

extern "C"
int Maskhatch_Init(Tcl_Interp* interp)
{
GVX_TRACE("Maskhatch_Init");

  GVX_PKG_CREATE(pkg, interp, "MaskHatch", "4.$Revision$");
  pkg->inherit_pkg("GxShapeKit");
  tcl::defFieldContainer<MaskHatch>(pkg, SRC_POS);
  tcl::def_creator<MaskHatch>(pkg);

  GVX_PKG_RETURN(pkg);
}

static const char vcid_groovx_visx_tclpkg_mask_cc_utc20050628171008[] = "$Id$ $HeadURL$";
#endif // !GROOVX_VISX_TCLPKG_MASK_CC_UTC20050628171008_DEFINED