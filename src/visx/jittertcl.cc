///////////////////////////////////////////////////////////////////////
//
// jittertcl.cc
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Apr  7 14:58:40 1999
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

#ifndef JITTERTCL_CC_DEFINED
#define JITTERTCL_CC_DEFINED

#include "visx/jitter.h"

#include "tcl/objpkg.h"
#include "tcl/tclpkg.h"

#include "util/objfactory.h"

#include "util/trace.h"

extern "C"
int Jitter_Init(Tcl_Interp* interp)
{
DOTRACE("Jitter_Init");

  PKG_CREATE(interp, "Jitter", "$Revision$");
  pkg->inheritPkg("GxTransform");
  Tcl::defGenericObjCmds<Jitter>(pkg, SRC_POS);

  pkg->defVec( "setJitter", "posid x_jitter y_jitter r_jitter",
               &Jitter::setJitter, 1,
               SRC_POS );

  Util::ObjFactory::theOne().registerCreatorFunc(&Jitter::make);

  PKG_RETURN;
}

static const char vcid_jittertcl_cc[] = "$Header$";
#endif // !JITTERTCL_CC_DEFINED
