///////////////////////////////////////////////////////////////////////
//
// mtxtcl.cc
//
// Copyright (c) 2001-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Jul  9 17:49:07 2001
// commit: $Id$
// $HeadURL$
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

#ifndef GROOVX_PKGS_MTX_MTXTCL_CC_UTC20050626084022_DEFINED
#define GROOVX_PKGS_MTX_MTXTCL_CC_UTC20050626084022_DEFINED

#include "pkgs/mtx/mtxtcl.h"

#include "mtxobj.h"

#include "nub/objfactory.h"

#include "tcl/objpkg.h"
#include "tcl/pkg.h"

#include "rutz/trace.h"

extern "C"
int Mtx_Init(Tcl_Interp* interp)
{
GVX_TRACE("Mtx_Init");

  GVX_PKG_CREATE(pkg, interp, "mtx", "4.$Revision$");
  pkg->inheritPkg("Obj");
  Tcl::defGenericObjCmds<MtxObj>(pkg, SRC_POS);

  pkg->defGetter<MtxObj, rutz::fstring>("print", &mtx::as_string,
                                        SRC_POS);

  pkg->defSetter<MtxObj, const char*>("scan", &MtxObj::scan_string,
                                      SRC_POS);

  pkg->defGetter<MtxObj, int>("mrows", &mtx::mrows, SRC_POS);
  pkg->defGetter<MtxObj, int>("ncols", &mtx::ncols, SRC_POS);
  pkg->defGetter<MtxObj, int>("nelems", &mtx::nelems, SRC_POS);

  Nub::ObjFactory::theOne().register_creator(&MtxObj::make);
  Nub::ObjFactory::theOne().register_alias("MtxObj", "mtx");

  GVX_PKG_RETURN(pkg);
}

static const char vcid_groovx_pkgs_mtx_mtxtcl_cc_utc20050626084022[] = "$Id$ $HeadURL$";
#endif // !GROOVX_PKGS_MTX_MTXTCL_CC_UTC20050626084022_DEFINED
