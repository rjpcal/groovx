///////////////////////////////////////////////////////////////////////
//
// mtxtcl.cc
//
// Copyright (c) 2001-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Jul  9 17:49:07 2001
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

#ifndef MTXTCL_CC_DEFINED
#define MTXTCL_CC_DEFINED

#include "mtxobj.h"

#include "tcl/objpkg.h"
#include "tcl/tclpkg.h"

#include "util/objfactory.h"

#include "util/trace.h"

extern "C"
int Mtx_Init(Tcl_Interp* interp)
{
DOTRACE("Mtx_Init");

  PKG_CREATE(interp, "Mtx", "$Revision$");
  Tcl::defGenericObjCmds<MtxObj>(pkg);

  pkg->defAction<MtxObj>("print", &Mtx::print);

  pkg->defGetter<MtxObj, int>("mrows", &Mtx::mrows);
  pkg->defGetter<MtxObj, int>("ncols", &Mtx::ncols);
  pkg->defGetter<MtxObj, int>("nelems", &Mtx::nelems);

  Util::ObjFactory::theOne().registerCreatorFunc(&MtxObj::make);

  PKG_RETURN;
}

static const char vcid_mtxtcl_cc[] = "$Header$";
#endif // !MTXTCL_CC_DEFINED
