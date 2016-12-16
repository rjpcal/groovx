/** @file pkgs/mtx/tclpkg-mtx.cc tcl interface package for class mtx */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Mon Jul  9 17:49:07 2001
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#include "pkgs/mtx/tclpkg-mtx.h"

#include "mtxobj.h"

#include "nub/objfactory.h"

#include "tcl/objpkg.h"
#include "tcl/pkg.h"

#include "rutz/trace.h"

namespace tcl
{
  template <>
  struct help_convert<rutz::this_pointer<mtx>>
  {
    static nub::soft_ref<MtxObj> from_tcl(Tcl_Obj* obj)
    {
      nub::uid uid = tcl::convert_to<nub::uid>(obj);
      return nub::soft_ref<MtxObj>(uid);
    }
  };
  template <>
  struct help_convert<rutz::this_pointer<mtx_specs>>
  {
    static nub::soft_ref<MtxObj> from_tcl(Tcl_Obj* obj)
    {
      nub::uid uid = tcl::convert_to<nub::uid>(obj);
      return nub::soft_ref<MtxObj>(uid);
    }
  };
}

extern "C"
int Mtx_Init(Tcl_Interp* interp)
{
GVX_TRACE("Mtx_Init");

  return tcl::pkg::init
    (interp, "mtx", "4.0",
     [](tcl::pkg* pkg) {
      pkg->inherit_pkg("Obj");
      tcl::def_basic_type_cmds<MtxObj>(pkg, SRC_POS);

      pkg->def("print", "?precision=17?", &MtxObj::as_string, SRC_POS, 17);

      pkg->def_setter("scan", &MtxObj::scan_string, SRC_POS);
      pkg->def_getter("mrows", &mtx::mrows, SRC_POS);
      pkg->def_getter("ncols", &mtx::ncols, SRC_POS);
      pkg->def_getter("nelems", &mtx::nelems, SRC_POS);

      nub::obj_factory::instance().register_creator(&MtxObj::make);
      nub::obj_factory::instance().register_alias("MtxObj", "mtx");
    });
}

