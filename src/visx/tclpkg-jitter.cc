/** @file visx/tclpkg-jitter.cc tcl interface package for class Jitter */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Apr  7 14:58:40 1999
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

#ifndef GROOVX_VISX_TCLPKG_JITTER_CC_UTC20050628171008_DEFINED
#define GROOVX_VISX_TCLPKG_JITTER_CC_UTC20050628171008_DEFINED

#include "visx/tclpkg-jitter.h"

#include "nub/objfactory.h"

#include "tcl/objpkg.h"
#include "tcl/pkg.h"

#include "visx/jitter.h"

#include "rutz/trace.h"

extern "C"
int Jitter_Init(Tcl_Interp* interp)
{
GVX_TRACE("Jitter_Init");

  GVX_PKG_CREATE(pkg, interp, "Jitter", "4.$Revision$");
  pkg->inherit_pkg("GxTransform");
  tcl::def_basic_type_cmds<Jitter>(pkg, SRC_POS);

  pkg->def_vec( "setJitter", "posid x_jitter y_jitter r_jitter",
                &Jitter::setJitter, 1,
                SRC_POS );

  nub::obj_factory::instance().register_creator(&Jitter::make);

  GVX_PKG_RETURN(pkg);
}

#endif // !GROOVX_VISX_TCLPKG_JITTER_CC_UTC20050628171008_DEFINED
