/** @file tcl/dict.cc c++ wrapper class for tcl "dict" objects (new in
    tcl 8.5) */
///////////////////////////////////////////////////////////////////////
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
#include "rutz/sfmt.h"

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

tcl::dict tcl::aux_convert_to(Tcl_Obj* obj, tcl::dict*)
{
GVX_TRACE("tcl::aux_convert_to(tcl::dict*)");

  return tcl::dict(obj);
}

tcl::obj tcl::aux_convert_from(tcl::dict dict_value)
{
GVX_TRACE("tcl::aux_convert_from(tcl::dict)");

  return dict_value.as_obj();
}

#ifndef HAVE_TCL_DICT
namespace
{
  void error_no_dict_support()
  {
    throw rutz::error("tcl::dict requires Tcl version >= 8.5", SRC_POS);
  }
}

void tcl::dict::do_put(const char*, tcl::obj)
{
  error_no_dict_support();
  GVX_ASSERT(0);
}

tcl::obj tcl::dict::do_get(const char*) const
{
  error_no_dict_support();
  GVX_ASSERT(0);
  return tcl::obj(); // can't happen, but placate compiler
}

#else

void tcl::dict::do_put(const char* key, tcl::obj val)
{
GVX_TRACE("tcl::dict::do_put");

  tcl::obj key_obj = tcl::convert_from(key);
  if (Tcl_DictObjPut(0, m_obj.get(), key_obj.get(), val.get()) != TCL_OK)
    {
      throw rutz::error(rutz::sfmt("couldn't put object in dict "
                                   "with key: %s", key), SRC_POS);
    }
}

tcl::obj tcl::dict::do_get(const char* key) const
{
GVX_TRACE("tcl::dict::do_get");

  Tcl_Obj* dest = 0;

  tcl::obj key_obj = tcl::convert_from(key);

  if (Tcl_DictObjGet(0, m_obj.get(), key_obj.get(), &dest) == TCL_OK)
    {
      if (dest != 0)
        return tcl::obj(dest);
    }

  throw rutz::error(rutz::sfmt("couldn't get value from dict "
                               "with key: %s", key), SRC_POS);
}

#endif // defined(HAVE_TCL_DICT)

static const char __attribute__((used)) vcid_groovx_tcl_dict_cc_utc20050628162420[] = "$Id$ $HeadURL$";
#endif // !GROOVX_TCL_DICT_CC_UTC20050628162420_DEFINED
