///////////////////////////////////////////////////////////////////////
//
// tcldictobj.cc
//
// Copyright (c) 2004-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon May 24 16:21:12 2004
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

#ifndef GROOVX_TCL_DICT_CC_UTC20050628162420_DEFINED
#define GROOVX_TCL_DICT_CC_UTC20050628162420_DEFINED

#include "tcl/dict.h"

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

void Tcl::Dict::doPut(const char*, Tcl::Obj)
{
  noDictError();
  GVX_ASSERT(0);
}

Tcl::Obj Tcl::Dict::doGet(const char*) const
{
  noDictError();
  GVX_ASSERT(0);
  return Tcl::Obj(); // can't happen, but placate compiler
}

#else

void Tcl::Dict::doPut(const char* key, Tcl::Obj val)
{
GVX_TRACE("Tcl::Dict::doPut");

  Tcl::Obj keyObj = Tcl::toTcl(key);
  if (Tcl_DictObjPut(0, itsDictObj.obj(), keyObj.obj(), val.obj()) != TCL_OK)
    {
      throw rutz::error(rutz::fstring("couldn't put object in dict "
                                      "with key: ", key), SRC_POS);
    }
}

Tcl::Obj Tcl::Dict::doGet(const char* key) const
{
GVX_TRACE("Tcl::Dict::doGet");

  Tcl_Obj* dest = 0;

  Tcl::Obj keyObj = Tcl::toTcl(key);

  if (Tcl_DictObjGet(0, itsDictObj.obj(), keyObj.obj(), &dest) == TCL_OK)
    {
      if (dest != 0)
        return Tcl::Obj(dest);
    }

  throw rutz::error(rutz::fstring("couldn't get value from dict "
                                  "with key: ", key), SRC_POS);
}

#endif // defined(HAVE_TCL_DICT)

static const char vcid_groovx_tcl_dict_cc_utc20050628162420[] = "$Id$ $HeadURL$";
#endif // !GROOVX_TCL_DICT_CC_UTC20050628162420_DEFINED
