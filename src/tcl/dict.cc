/** @file tcl/dict.cc c++ wrapper class for tcl "dict" objects (new in
    tcl 8.5) */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon May 24 16:21:12 2004
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

tcl::dict tcl::help_convert<tcl::dict>::from_tcl(Tcl_Obj* obj)
{
GVX_TRACE("tcl::help_convert<tcl::dict>::from_tcl");

  return tcl::dict(obj);
}

tcl::obj tcl::help_convert<tcl::dict>::to_tcl(tcl::dict dict_value)
{
GVX_TRACE("tcl::help_convert<tcl::dict>::to_tcl");

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

  Tcl_Obj* dest = nullptr;

  tcl::obj key_obj = tcl::convert_from(key);

  if (Tcl_DictObjGet(0, m_obj.get(), key_obj.get(), &dest) == TCL_OK)
    {
      if (dest != nullptr)
        return tcl::obj(dest);
    }

  throw rutz::error(rutz::sfmt("couldn't get value from dict "
                               "with key: %s", key), SRC_POS);
}

#endif // defined(HAVE_TCL_DICT)
