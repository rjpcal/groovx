///////////////////////////////////////////////////////////////////////
//
// tclstringconvert.h
//
// Copyright (c) 2005-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Sun Jun 26 12:34:02 2005
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

#ifndef GROOVX_TCL_TCLSTRINGCONVERT_H_UTC20050626193402_DEFINED
#define GROOVX_TCL_TCLSTRINGCONVERT_H_UTC20050626193402_DEFINED

#include "rutz/fstring.h"
#include "tcl/tclconvert.h"
#include "tcl/tclobjptr.h"

#include <string>

namespace Tcl
{
  Tcl::ObjPtr toTcl(std::string s) { return toTcl(s.c_str()); }
  std::string fromTcl(Tcl_Obj* obj, std::string*)
  {
    return std::string(fromTcl(obj, (rutz::fstring*)0).c_str());
  }
}

static const char vcid_groovx_tcl_tclstringconvert_h_utc20050626193402[] = "$Id$ $URL$";
#endif // !GROOVX_TCL_TCLSTRINGCONVERT_H_UTC20050626193402DEFINED
