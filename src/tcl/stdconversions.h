///////////////////////////////////////////////////////////////////////
//
// tclstringconvert.h
//
// Copyright (c) 2005-2005
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

namespace Tcl
{
  inline Tcl::Obj toTclImpl(std::string s)
  {
    return toTcl<const char*>(s.c_str());
  }

  inline std::string fromTclImpl(Tcl_Obj* obj, std::string*)
  {
    return std::string(fromTclImpl(obj, (rutz::fstring*)0).c_str());
  }
}

static const char vcid_groovx_tcl_stdconversions_h_utc20050628163220[] = "$Id$ $HeadURL$";
#endif // !GROOVX_TCL_STDCONVERSIONS_H_UTC20050628163220_DEFINED
