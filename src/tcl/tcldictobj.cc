///////////////////////////////////////////////////////////////////////
//
// tcldictobj.cc
//
// Copyright (c) 2004-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon May 24 16:21:12 2004
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

#ifndef TCLDICTOBJ_CC_DEFINED
#define TCLDICTOBJ_CC_DEFINED

#include "tcl/tcldictobj.h"

#include "util/error.h"
#include "util/strings.h"

#include <tcl.h>

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

#if ((TCL_MAJOR_VERSION == 8) \
      && (TCL_MINOR_VERSION >= 5)) \
    || (TCL_MAJOR_VERSION > 8)
#  define HAVE_TCL_DICT
#else
#  undef HAVE_TCL_DICT
#endif


#ifndef HAVE_TCL_DICT
namespace
{
  void noDictError()
  {
    throw Util::Error("Tcl::Dict requires Tcl version >= 8.5");
  }
}
#else

void Tcl::Dict::doPut(const char* key, Tcl::ObjPtr val)
{
DOTRACE("Tcl::Dict::doPut");

  Tcl::ObjPtr keyObj = Tcl::toTcl(key);
  if (Tcl_DictObjPut(0, itsDictObj.obj(), keyObj.obj(), val.obj()) != TCL_OK)
    {
      throw Util::Error(fstring("couldn't put object in dict with key: ",
                                key));
    }
}

Tcl::ObjPtr Tcl::Dict::doGet(const char* key) const
{
DOTRACE("Tcl::Dict::doGet");

  Tcl_Obj* dest = 0;

  Tcl::ObjPtr keyObj = Tcl::toTcl(key);

  if (Tcl_DictObjGet(0, itsDictObj.obj(), keyObj.obj(), &dest) == TCL_OK)
    {
      if (dest != 0)
        return Tcl::ObjPtr(dest);
    }

  throw Util::Error(fstring("couldn't get value from dict with key: ",
                            key));
}

#endif // defined(HAVE_TCL_DICT)

static const char vcid_tcldictobj_cc[] = "$Header$";
#endif // !TCLDICTOBJ_CC_DEFINED
