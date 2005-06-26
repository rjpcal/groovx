///////////////////////////////////////////////////////////////////////
//
// tcldictobj.cc
//
// Copyright (c) 2004-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon May 24 16:21:12 2004
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

#ifndef GROOVX_TCL_TCLDICTOBJ_CC_UTC20050626084017_DEFINED
#define GROOVX_TCL_TCLDICTOBJ_CC_UTC20050626084017_DEFINED

#include "tcl/tcldictobj.h"

#include "rutz/error.h"
#include "rutz/fstring.h"

#include <tcl.h>

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

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
    throw rutz::error("Tcl::Dict requires Tcl version >= 8.5", SRC_POS);
  }
}

void Tcl::Dict::doPut(const char*, Tcl::ObjPtr)
{
  noDictError();
  GVX_ASSERT(0);
}

Tcl::ObjPtr Tcl::Dict::doGet(const char*) const
{
  noDictError();
  GVX_ASSERT(0);
  return Tcl::ObjPtr(); // can't happen, but placate compiler
}

#else

void Tcl::Dict::doPut(const char* key, Tcl::ObjPtr val)
{
GVX_TRACE("Tcl::Dict::doPut");

  Tcl::ObjPtr keyObj = Tcl::toTcl(key);
  if (Tcl_DictObjPut(0, itsDictObj.obj(), keyObj.obj(), val.obj()) != TCL_OK)
    {
      throw rutz::error(rutz::fstring("couldn't put object in dict "
                                      "with key: ", key), SRC_POS);
    }
}

Tcl::ObjPtr Tcl::Dict::doGet(const char* key) const
{
GVX_TRACE("Tcl::Dict::doGet");

  Tcl_Obj* dest = 0;

  Tcl::ObjPtr keyObj = Tcl::toTcl(key);

  if (Tcl_DictObjGet(0, itsDictObj.obj(), keyObj.obj(), &dest) == TCL_OK)
    {
      if (dest != 0)
        return Tcl::ObjPtr(dest);
    }

  throw rutz::error(rutz::fstring("couldn't get value from dict "
                                  "with key: ", key), SRC_POS);
}

#endif // defined(HAVE_TCL_DICT)

static const char vcid_groovx_tcl_tcldictobj_cc_utc20050626084017[] = "$Id$ $HeadURL$";
#endif // !GROOVX_TCL_TCLDICTOBJ_CC_UTC20050626084017_DEFINED
