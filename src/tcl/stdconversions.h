/** @file tcl/stdconversions.h tcl conversions for std::string and
    other std library classes */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2005-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Sun Jun 26 12:34:02 2005
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

#ifndef GROOVX_TCL_STDCONVERSIONS_H_UTC20050628163220_DEFINED
#define GROOVX_TCL_STDCONVERSIONS_H_UTC20050628163220_DEFINED

#include "rutz/fstring.h"
#include "tcl/conversions.h"
#include "tcl/obj.h"

#include <string>

namespace tcl
{
  template <>
  struct help_convert<std::string>
  {
    static tcl::obj to_tcl(std::string s)
    {
      return convert_from<const char*>(s.c_str());
    }

    static std::string from_tcl(Tcl_Obj* obj)
    {
      return std::string(help_convert<rutz::fstring>::from_tcl(obj).c_str());
    }
  };
}

static const char __attribute__((used)) vcid_groovx_tcl_stdconversions_h_utc20050628163220[] = "$Id$ $HeadURL$";
#endif // !GROOVX_TCL_STDCONVERSIONS_H_UTC20050628163220_DEFINED
